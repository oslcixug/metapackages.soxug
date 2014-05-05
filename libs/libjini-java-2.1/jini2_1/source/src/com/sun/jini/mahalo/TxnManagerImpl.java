/*
 * 
 * Copyright 2005 Sun Microsystems, Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * 	http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
package com.sun.jini.mahalo;

import com.sun.jini.admin.DestroyAdmin;
import com.sun.jini.config.Config;
import com.sun.jini.landlord.FixedLeasePeriodPolicy;
import com.sun.jini.landlord.Landlord;
import com.sun.jini.landlord.Landlord.RenewResults;
import com.sun.jini.landlord.LandlordUtil;
import com.sun.jini.landlord.LeaseFactory;
import com.sun.jini.landlord.LeasedResource;
import com.sun.jini.landlord.LeasePeriodPolicy;
import com.sun.jini.landlord.LeasePeriodPolicy.Result;
import com.sun.jini.landlord.LocalLandlord;
import com.sun.jini.logging.Levels;
import com.sun.jini.mahalo.log.*;
import com.sun.jini.start.LifeCycle;
import com.sun.jini.thread.InterruptedStatusThread;
import com.sun.jini.thread.ReadyState;
import com.sun.jini.thread.TaskManager;
import com.sun.jini.thread.WakeupManager;


import net.jini.admin.JoinAdmin;
import net.jini.admin.Administrable;
import net.jini.core.discovery.LookupLocator;
import net.jini.core.entry.*;
import net.jini.core.event.*;
import net.jini.core.lease.*;
import net.jini.id.Uuid;
import net.jini.id.UuidFactory;
import net.jini.lookup.entry.*;
import net.jini.core.transaction.*;
import net.jini.core.transaction.server.*;


import java.util.*;
import java.lang.reflect.*;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.io.Serializable;
import java.net.URL;
import java.rmi.*;
import java.rmi.activation.*;
import java.rmi.server.*;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;
import java.security.SecureRandom;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.security.auth.Subject;
import javax.security.auth.login.LoginContext;
import javax.security.auth.login.LoginException;

import net.jini.activation.ActivationExporter;
import net.jini.config.Configuration;
import net.jini.config.ConfigurationProvider;
import net.jini.config.ConfigurationException;
import net.jini.core.constraint.RemoteMethodControl;
import net.jini.core.lookup.ServiceID;
import net.jini.export.Exporter;
import net.jini.export.ProxyAccessor;
import net.jini.jeri.BasicILFactory;
import net.jini.jeri.BasicJeriExporter;
import net.jini.jeri.InvocationLayerFactory;
import net.jini.jeri.tcp.TcpServerEndpoint;
import net.jini.jrmp.JrmpExporter;
import net.jini.security.BasicProxyPreparer;
import net.jini.security.ProxyPreparer;
import net.jini.security.proxytrust.ServerProxyTrust;
import net.jini.security.TrustVerifier;

/**
 * An implementation of the Jini(TM) Transaction Specification.
 *
 * @author Sun Microsystems, Inc.
 *
 */
class TxnManagerImpl /*extends RemoteServer*/
    implements TxnManager, LeaseExpirationMgr.Expirer,
	       LogRecovery, TxnSettler, com.sun.jini.constants.TimeConstants,
               LocalLandlord, ServerProxyTrust, ProxyAccessor
{
    /** Logger for (successful) service startup message */
    static final Logger startupLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".startup");
    
    /** Logger for service re/initialization related messages */
    static final Logger initLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".init");
	
    /** Logger for service destruction related messages */
    static final Logger destroyLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".destroy");

    /** Logger for service operation messages */
    static final Logger operationsLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".operations");

    /** 
     * Logger for transaction related messages 
     * (creation, destruction, transition, etc.) 
     */
    static final Logger transactionsLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".transactions");

    /** Logger for transaction participant related messages */
    static final Logger participantLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".participant");

    /** Logger for transaction persistence related messages */
    static final Logger persistenceLogger = 
        Logger.getLogger(TxnManager.MAHALO + ".persistence");

    /**
     * @serial
     */
    private LogManager logmgr;

    /* Default tuning parameters for thread pool */
    /* Retrieve values from properties.          */

    private transient int settlerthreads = 150;
    private transient long settlertimeout = 1000 * 15;
    private transient float settlerload = 1.0f;


    private transient int taskthreads = 50;
    private transient long tasktimeout = 1000 * 15;
    private transient float taskload = 3.0f;


    /* Its important here to schedule SettlerTasks on a */
    /* different TaskManager from what is given to      */
    /* TxnManagerTransaction objects.  Tasks on a given */
    /* TaskManager which create Tasks cannot be on the  */
    /* same TaskManager as their child Tasks.		*/

    private transient TaskManager settlerpool;
    /** wakeup manager for <code>SettlerTask</code> */
    private WakeupManager settlerWakeupMgr;

    private transient TaskManager taskpool;
    /** wakeup manager for <code>ParticipantTask</code> */
    private WakeupManager taskWakeupMgr;

    /*
     * Map of transaction ids are their associated, internal 
     * transaction representations 
     */
    private transient Map       txns;

    private transient Vector		unsettledtxns;
    private transient InterruptedStatusThread settleThread;

    /**
     * @serial
     */
    private String persistenceDirectory = null;

    /**
     * @serial
     */
    private ActivationID activationID;
    
    /** Whether the activation ID has been prepared */
    private boolean activationPrepared;

    /** The activation system, prepared */
    private ActivationSystem activationSystem;

    /** Proxy preparer for listeners */
    private ProxyPreparer participantPreparer;

    /** The exporter for exporting and unexporting */
    protected Exporter exporter;

    /** The login context, for logging out */
    protected LoginContext loginContext;

    /** The generator for our IDs. */
    private static transient SecureRandom idGen = new SecureRandom();

    /** The buffer for generating IDs. */
    private static transient final byte[] idGenBuf = new byte[8];


    /** 
     * <code>LeaseExpirationMgr</code> used by our <code>LeasePolicy</code>.
     */
    private LeaseExpirationMgr expMgr;

    /**
     * @serial
     */
    private /*final*/ LeasePeriodPolicy txnLeasePeriodPolicy = null;
    
    /** <code>LandLordLeaseFactory</code> we use to create leases */
    private LeaseFactory leaseFactory = null;

    /**
     * @serial
     */
    private JoinStateManager joinStateManager;

    /**
     * The <code>Uuid</code> for this service. Used in the
     * <code>TxnMgrProxy</code> and <code>TxnMgrAdminProxy</code> to
     * implement reference equality. We also derive our
     * <code>ServiceID</code> from it.
     */
    private Uuid topUuid = null;

    /** The outter proxy of this server */
    private TxnMgrProxy txnMgrProxy;		

    /** The admin proxy of this server */
    private TxnMgrAdminProxy txnMgrAdminProxy;		

    /**
     * Cache of our inner proxy.
     */
    private TxnManager serverStub = null;

    /**
     * Cache of our <code>LifeCycle</code> object
     */
    private LifeCycle lifeCycle = null;
    
    /** 
     * Object used to prevent access to this service during the service's
     *  initialization or shutdown processing.
     */
    private final ReadyState readyState = new ReadyState();

    /**
     * <code>boolean</code> flag used to determine persistence support.
     * Defaulted to true, and overridden in the constructor overload that takes
     * a <code>boolean</code> argument.
     */
    private boolean persistent = true;

    /**
     * Constructs a non-activatable transaction manager.
     *
     * @param args Service configuration options
     *
     * @param lc <code>LifeCycle</code> reference used for callback
     */
    TxnManagerImpl(String[] args, LifeCycle lc, boolean persistent)
	throws Exception
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "TxnManagerImpl",
	        new Object[] {
		    Arrays.asList(args), lc, Boolean.valueOf(persistent)});
	}
	lifeCycle = lc; 
	this.persistent = persistent;
	try {
            init(args);
	} catch (Throwable e) {
            cleanup();
            initFailed(e); 
	}
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "TxnManagerImpl");
	}
    }
    /**
     * Constructs an activatable transaction manager.
     *
     * @param activationID activation ID passed in by the activation daemon.
     *
     * @param data state data needed to re-activate a transaction manager.
     */
    TxnManagerImpl(ActivationID activationID, MarshalledObject data)
	throws Exception
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "TxnManagerImpl",
	        new Object[] {activationID, data} );
	}
	this.activationID = activationID;
        try {
            // Initialize state
            init((String[])data.get());
        } catch (Throwable e) {
            cleanup();
	    initFailed(e); 
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "TxnManagerImpl");
	}
    }
    
    /** Initialization common to both activatable and transient instances. */
    private void init(String[] configArgs)
        throws Exception
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), "init",
	        (Object[])configArgs );
	}
        final Configuration config =
            ConfigurationProvider.getInstance(
		configArgs, getClass().getClassLoader());
        loginContext = (LoginContext) config.getEntry(
            TxnManager.MAHALO, "loginContext", LoginContext.class, null);
        if (loginContext != null) {
            doInitWithLogin(config, loginContext);
        } else {
            doInit(config);
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "init");
	}
    }
    
    private void doInitWithLogin(final Configuration config,
        LoginContext loginContext) throws Exception
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), 
	        "doInitWithLogin",
	        new Object[] { config, loginContext } );
	}
        loginContext.login();
        try {
            Subject.doAsPrivileged(
                loginContext.getSubject(),
                new PrivilegedExceptionAction() {
                    public Object run() throws Exception {
                        doInit(config);
                        return null;
                    }
                },
                null);
        } catch (PrivilegedActionException e) {
//TODO - move to end of initFailed() so that shutdown still occurs under login 	
            try {
                loginContext.logout();
            } catch (LoginException le) {
                if( initLogger.isLoggable(Levels.HANDLED) ) {
		    initLogger.log(Levels.HANDLED, "Trouble logging out", le);
		}
            }
            throw e.getException();
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), 
	        "doInitWithLogin");
	}

    }

    private void doInit(Configuration config) throws Exception {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "doInit", config);
	}
        // Get activatable settings, if activated
        if (activationID != null) {
            ProxyPreparer activationSystemPreparer =
                (ProxyPreparer) Config.getNonNullEntry(config,
                    TxnManager.MAHALO, "activationSystemPreparer",
                    ProxyPreparer.class, new BasicProxyPreparer());
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, "activationSystemPreparer: {0}", 
	            activationSystemPreparer);		
	    }
            activationSystem =
                (ActivationSystem) activationSystemPreparer.prepareProxy(
                    ActivationGroup.getSystem());
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, "Prepared activation system is: {0}",
                    activationSystem);
            }
            ProxyPreparer activationIdPreparer =
                (ProxyPreparer) Config.getNonNullEntry(config,
                    TxnManager.MAHALO, "activationIdPreparer",
                    ProxyPreparer.class, new BasicProxyPreparer());
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, "activationIdPreparer: {0}", 
	            activationIdPreparer);		
            }
	    activationID = (ActivationID) activationIdPreparer.prepareProxy(
                activationID);
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, "Prepared activationID is: {0}",
                    activationID);
            }
	    activationPrepared = true;
            exporter = (Exporter)Config.getNonNullEntry(config,
	        TxnManager.MAHALO, "serverExporter", Exporter.class,
                new ActivationExporter(
		    activationID,
                    new BasicJeriExporter(
			TcpServerEndpoint.getInstance(0), 
			new BasicILFactory(), false, true)),
		    activationID);
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, 
	            "Activatable service exporter is: {0}", exporter);
	    }
        } else {
            exporter = (Exporter) Config.getNonNullEntry(config,
                TxnManager.MAHALO, "serverExporter", Exporter.class,
                new BasicJeriExporter( 
		    TcpServerEndpoint.getInstance(0), 
		    new BasicILFactory(), false, true));
            if(initLogger.isLoggable(Level.CONFIG)) {
		initLogger.log(Level.CONFIG, 
	            "Non-activatable service exporter is: {0}", exporter);
            }
	}
	
	ProxyPreparer recoveredParticipantPreparer = 
	    (ProxyPreparer)Config.getNonNullEntry(config,
                TxnManager.MAHALO, "recoveredParticipantPreparer", 
		ProxyPreparer.class, new BasicProxyPreparer());
        if(initLogger.isLoggable(Level.CONFIG)) {
	    initLogger.log(Level.CONFIG, "Recovered participant preparer is: {0}",
                recoveredParticipantPreparer);
        }
	participantPreparer = (ProxyPreparer)Config.getNonNullEntry(config,
            TxnManager.MAHALO, "participantPreparer", ProxyPreparer.class,
            new BasicProxyPreparer());
        if(initLogger.isLoggable(Level.CONFIG)) {
	    initLogger.log(Level.CONFIG, "Participant preparer is: {0}",
                participantPreparer);
        }
	// Create lease policy -- used by recovery logic, below??
        txnLeasePeriodPolicy = (LeasePeriodPolicy)Config.getNonNullEntry(
            config, TxnManager.MAHALO, "leasePeriodPolicy", 
	    LeasePeriodPolicy.class,
            new FixedLeasePeriodPolicy(3 * HOURS, 1 * HOURS));
        if(initLogger.isLoggable(Level.CONFIG)) {
	    initLogger.log(Level.CONFIG, "leasePeriodPolicy is: {0}",
                txnLeasePeriodPolicy);
	}    
	
	if (persistent) {
            persistenceDirectory =
                (String)Config.getNonNullEntry(config,
                    TxnManager.MAHALO, "persistenceDirectory", String.class);
            if(initLogger.isLoggable(Level.CONFIG)) {
	        initLogger.log(Level.CONFIG, "Persistence directory is: {0}",
                    persistenceDirectory);
            }
	} else { // just for insurance
	    persistenceDirectory = null;
	}
	
	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Creating JoinStateManager");
        }
	// Note: null persistenceDirectory means no persistence
	joinStateManager = new JoinStateManager(persistenceDirectory);
	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Recovering join state ...");
	}
	joinStateManager.recover();
	
	// ServiceUuid will be null first time up.
	if (joinStateManager.getServiceUuid() == null) {
	    if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, "Generating service Uuid");
	    }
	    topUuid = UuidFactory.generate();
	    // Actual snapshot deferred until JSM is started, below
	    joinStateManager.setServiceUuid(topUuid);
	} else { // get recovered value for serviceUuid
	    if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, "Recovering service Uuid");
	    }
	    topUuid = joinStateManager.getServiceUuid();
	}
        if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Uuid is: {0}", topUuid);
	}
	
	if (persistent) {
            // Check persistence path for validity, and create if necessary
            com.sun.jini.system.FileSystem.ensureDir(persistenceDirectory);
	}
	
	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Exporting server");
	}
	serverStub = (TxnManager)exporter.export(this);
        if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Server stub: {0}", serverStub);
	}
	// Create the proxy that will be registered in the lookup service
        txnMgrProxy = 
	    TxnMgrProxy.create(serverStub, topUuid);
        if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Service proxy is: {0}", 
	        txnMgrProxy);		
        }
	// Create the admin proxy for this service
        txnMgrAdminProxy = 
	    TxnMgrAdminProxy.create(serverStub, topUuid);
        if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Service admin proxy is: {0}", 
	        txnMgrAdminProxy);		
	}
	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Setting up data structures");
        }
	txns = Collections.synchronizedMap(new HashMap());
        
        // Used by log recovery logic			
        settlerWakeupMgr =
            new WakeupManager(new WakeupManager.ThreadDesc(null, true));
	taskWakeupMgr =
            new WakeupManager(new WakeupManager.ThreadDesc(null, true));
	
        settlerpool =
            (TaskManager) Config.getNonNullEntry(
                config, TxnManager.MAHALO, "settlerPool", TaskManager.class,
                new TaskManager(settlerthreads, settlertimeout,
                                settlerload));
        taskpool =
            (TaskManager) Config.getNonNullEntry(
                config, TxnManager.MAHALO, "taskPool", TaskManager.class,
                new TaskManager(taskthreads, tasktimeout,
                                taskload));  
        
        unsettledtxns = new Vector();
	
        // Create leaseFactory
        leaseFactory = new LeaseFactory(serverStub, topUuid);
	
        // Create LeaseExpirationMgr
        expMgr = new LeaseExpirationMgr(this);

	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Setting up log manager");
        }
	if (persistent) {
 	    logmgr = new MultiLogManager(this, persistenceDirectory);
	} else {
 	    logmgr = new MultiLogManager();
	}

        try {
 	    if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, "Recovering state");
            }
	    logmgr.recover();
	    
	    // Restore transient state of recovered transactions
	    Iterator iter = txns.values().iterator();
            TxnManagerTransaction txn;
            while(iter.hasNext()) {
	        txn = (TxnManagerTransaction)iter.next();
                if(initLogger.isLoggable(Level.FINEST)) {
	            initLogger.log(Level.FINEST, 
		        "Restoring transient state for txn id: {0}", 
	                new Long(((ServerTransaction)txn.getTransaction()).id));		
		}
		try {
		    txn.restoreTransientState(recoveredParticipantPreparer);
		} catch (RemoteException re) {
                    if (persistenceLogger.isLoggable(Level.WARNING)) {
                        persistenceLogger.log(Level.WARNING,
		            "Cannot restore the TransactionParticipant", re);
		    }
//TODO - what should happen when participant preparation fails?		
		}
	    }
  
	    if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, "Settling incomplete transactions");
	    }
            settleThread = new InterruptedStatusThread("settleThread") {
                public void run() {
                    try {
		        settleTxns();
		    } catch (InterruptedException ie) {
                        if (transactionsLogger.isLoggable(Level.FINEST)) {
                            transactionsLogger.log(Level.FINEST,
                                "settleThread interrupted -- exiting");
                        }
		        return;
		    }
                };
            };
            settleThread.start();
        } catch (LogException le) {
            RemoteException re =  
	        new RemoteException("Problem recovering state");
	    initLogger.throwing(TxnManagerImpl.class.getName(), "doInit", re);
	    throw re;
        }

	/*
	 * With SecureRandom, the first ID requires generation of a
	 * secure seed, which can take several seconds.  We do it here
	 * so it doesn't affect the first call's time.  (I tried doing
	 * this in a separate thread so some of the startup would occur
	 * during the roundtrip back the client, but it didn't help
	 * much and this is simpler.)
	 */
	nextID();
	
	
	/*
	 * Create the object that manages and persists our join state
	 */
	if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST, "Starting JoinStateManager");
        }
	// Starting causes snapshot to occur
	joinStateManager.startManager(config, txnMgrProxy, 
	    new ServiceID(topUuid.getMostSignificantBits(),
                    topUuid.getLeastSignificantBits()),
            attributesFor());
	    
        if (startupLogger.isLoggable(Level.INFO)) {
            startupLogger.log
                   (Level.INFO, "Mahalo started: {0}", this);
        }
        readyState.ready();
	
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "doInit");
	}
    }


    //TransactionManager interface method

    public TransactionManager.Created create(long lease)
        throws LeaseDeniedException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "create", 
	        new Long(lease));
	}
        readyState.check();
    
	TxnManagerTransaction txntr = null;

	long tid = nextID();
        Uuid uuid = createLeaseUuid(tid);

        if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST, 
	        "Transaction ID is: {0}", new Long(tid));
	}

        txntr = new TxnManagerTransaction(
	    txnMgrProxy, logmgr, tid, taskpool, 
	    taskWakeupMgr, this, uuid);
	Lease txnmgrlease = null;
	try {
            Result r = txnLeasePeriodPolicy.grant(txntr, lease);
            txntr.setExpiration(r.expiration);
            txnmgrlease = 
	        leaseFactory.newLease(
		    uuid,
	            r.expiration);
            expMgr.register(txntr);		
	} catch (LeaseDeniedException lde) {
            // Should never happen in our implementation.
            throw new AssertionError("Transaction lease was denied" + lde);
        }

        if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST, 
	        "Created new TxnManagerTransaction ID is: {0}", new Long(tid));
	}

	Transaction tr = txntr.getTransaction();
	ServerTransaction str = null;

        try {
	    str = serverTransaction(tr);
	    txns.put(new Long(str.id), txntr);

            if (transactionsLogger.isLoggable(Level.FINEST)) {
                transactionsLogger.log(Level.FINEST,
		    "recorded new TxnManagerTransaction", txntr);
	    }


        } catch(Exception e) {
	    if (transactionsLogger.isLoggable(Level.FINEST)) {
                transactionsLogger.log(Level.FINEST,
		"Problem creating transaction", e);
	    }
	    RuntimeException wrap =
	        new RuntimeException("Unable to create transaction", e);
	    transactionsLogger.throwing(
	        TxnManagerImpl.class.getName(), "create", wrap);
	    throw wrap;
	}

        TransactionManager.Created tmp = 	
	    new TransactionManager.Created(str.id, txnmgrlease);		       
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "create", tmp);
	}

        return tmp;
    }

    public void
        join(long id, TransactionParticipant part, long crashCount)
        throws UnknownTransactionException, CannotJoinException,
	       CrashCountException, RemoteException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "join", 
	        new Object[] {new Long(id), part, new Long(crashCount) });
	}
        readyState.check();

								
        TransactionParticipant preparedTarget = null;
        preparedTarget =
            (TransactionParticipant)
                participantPreparer.prepareProxy(part);
        
	if (participantLogger.isLoggable(Level.FINEST)) {
            participantLogger.log(Level.FINEST, 
	        "prepared participant: {0}", preparedTarget);
	}

        TxnManagerTransaction txntr =
		(TxnManagerTransaction) txns.get(new Long(id));

	if (txntr == null)
	    throw new UnknownTransactionException("unknown transaction");

	// txntr.join does expiration check
	txntr.join(preparedTarget, crashCount);
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "join");
	}
    }


    public int getState(long id)
        throws UnknownTransactionException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "getState", 
	        new Object[] {new Long(id)});
	}
        readyState.check();

        TxnManagerTransaction txntr =
	        (TxnManagerTransaction) txns.get(new Long(id));

	if (txntr == null)
	    throw new UnknownTransactionException("unknown transaction");
        /* Expiration checks are only meaningful for active transactions. */
        /* NOTE: 
	 * 1) Cancellation sets expiration to 0 without changing state 
	 * from Active right away. Clients are supposed to treat 
	 * UnknownTransactionException just like Aborted, so it's OK to send
	 * in this case. 
	 * 2) Might be a small window where client is committing the transaction
	 * close to the expiration time. If the committed transition takes
	 * place between getState() and ensureCurrent then the client could get
	 * a false result.
	 */
//TODO - need better locking here. getState and expiration need to be checked atomically	
        int state = txntr.getState();
        if (state == ACTIVE && !ensureCurrent(txntr)) 
	    throw new UnknownTransactionException("unknown transaction");
	    
	if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "getState", 
	        new Integer(state));
	}
	return state;
    }


    public void commit(long id)
        throws UnknownTransactionException, CannotCommitException,
	RemoteException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "commit", 
	        new Long(id));
	}
        readyState.check();

        try {
            commit(id, 0);
        } catch(TimeoutExpiredException tee) {
	    //This exception is swallowed because the
	    //commit with no timeout only schedules a
	    //roll-forward to happen
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "commit");
	}
    }

    public void commit(long id, long waitFor)
        throws UnknownTransactionException, CannotCommitException,
	       TimeoutExpiredException, RemoteException
    {
        //!! No early return when not synchronous
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "commit", 
	        new Object[] {new Long(id), new Long(waitFor)});
	}
        readyState.check();

	TxnManagerTransaction txntr =
		(TxnManagerTransaction) txns.get(new Long(id));

	if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST,
	        "Retrieved TxnManagerTransaction: {0}", txntr);
	}

	if (txntr == null)
	    throw new UnknownTransactionException("Unknown transaction");

	// txntr.commit does expiration check
	txntr.commit(waitFor);
        txns.remove(new Long(id));

	if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST,
                "Committed transaction id {0}", new Long(id));
	}
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "commit");
	}
    }


    public void abort(long id)
	throws UnknownTransactionException, CannotAbortException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "abort", 
	        new Object[] {new Long(id)});
	}
        readyState.check();
        try {
            abort(id, 0);
        } catch(TimeoutExpiredException tee) {
	    //Swallow this exception because we only want to
	    //schedule a settler task
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "abort");
	}
    }

    public void abort(long id, long waitFor)
        throws UnknownTransactionException, CannotAbortException,
	       TimeoutExpiredException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "abort", 
	        new Object[] {new Long(id), new Long(waitFor)});
	}
        readyState.check();
        
        //!! Multi-participants not supported
        //!! No early return when not synchronous


        // At this point, ask the Participants associated
	// with the Transaction to prepare

	TxnManagerTransaction txntr = 
	        (TxnManagerTransaction) txns.get(new Long(id));

	if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST,
            "Retrieved TxnManagerTransaction: {0}", txntr);
	}
	/*
	 * Since lease cancellation process sets expiration to 0 
	 * and then calls abort, can't reliably check expiration 
	 * at this point.
	 */
//TODO - Change internal, lease logic to call overload w/o expiration check
//TODO - Add expiration check to abort for external clients     
	if (txntr == null)
	    throw new CannotAbortException();

	txntr.abort(waitFor);
	txns.remove(new Long(id));

	if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST,
                "aborted transaction id {0}", new Long(id));
	}
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "abort");
	}
    }

    //Satisfies the LogRecovery interface so that the
    //TransactionManager can recover it's non-transient
    //state in the face of process failure.

    /**
     *  This method recovers state changes resulting from
     *  committing a transaction.  This re-creates the
     *  internal representation of the transaction.
     * 
     * @param cookie the transaction's ID
     *
     * @param rec the <code>LogRecord</code>
     */
    public void recover(long cookie, LogRecord rec) throws LogException {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "recover", 
	        new Object[] {new Long(cookie), rec});
	}
	TxnManagerTransaction tmt = enterTMT(cookie);
	TxnLogRecord trec = (TxnLogRecord) rec;
	trec.recover(tmt);
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "recover");
	}
    }


    /**
     * Informs the transaction manager to attempt to
     * settle a given transaction.
     *
     * @param tid the transaction's ID
     */
    public synchronized void noteUnsettledTxn(long tid) {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), 
	        "noteUnsettledTxn", new Object[] {new Long(tid)});
	}
	unsettledtxns.add(new Long(tid));

	notifyAll();

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), 
	        "noteUnsettledTxn");
	}
    }

    private synchronized void settleTxns() throws InterruptedException {
	ClientLog log = null;

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), 
	        "settleTxns");
	}
	if (transactionsLogger.isLoggable(Level.FINEST)) {
            transactionsLogger.log(Level.FINEST,
                "Settling {0} transactions.", 
		new Integer(unsettledtxns.size()));
	}

	int numtxns = 0;
	Long first = null;
	long tid = 0;

	while (true) {
		numtxns = unsettledtxns.size();

	    if (numtxns == 0) {
	        if (transactionsLogger.isLoggable(Level.FINEST)) {
                    transactionsLogger.log(Level.FINEST,
                        "Settler waiting");
	        }
		wait();

	        if (transactionsLogger.isLoggable(Level.FINEST)) {
                    transactionsLogger.log(Level.FINEST,
                        "Settler notified");
	        }
		continue;
	    }

	    first = null;

	    first = (Long) unsettledtxns.firstElement();
	    tid = first.longValue();

	    SettlerTask task = 
	        new SettlerTask(
		    settlerpool, settlerWakeupMgr, this, tid);
	    settlerpool.add(task);
	    unsettledtxns.remove(first);

            if (settleThread.hasBeenInterrupted()) 
	        throw new InterruptedException("settleTxns interrupted");
		
	    if (transactionsLogger.isLoggable(Level.FINEST)) {
                transactionsLogger.log(Level.FINEST,
                    "Added SettlerTask for tid {0}", new Long(tid));
	    }
	}
	// Not reachable
        /*
	 * if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), 
	 *   "settleTxns");
	 */
    }


    //TransactionParticipant interface go here
    //when I implement nested transactions



    /**
     *  Method from <code>TxnManager</code> which produces
     *  a <code>Transaction</code> from its ID.
     *
     * @param id the ID
     *
     * @see net.jini.core.transaction.Transaction
     * @see com.sun.jini.mahalo.TxnManager
     */
    public Transaction getTransaction(long id)
        throws UnknownTransactionException {
            
        readyState.check();


        if (id == ((long)-1))
            return null;

        // First consult the hashtable for the Object
        // containing all actions performed under a
        // particular transaction

        TxnManagerTransaction txntr =
	        (TxnManagerTransaction) txns.get(new Long(id));

	if (txntr == null)
	    throw new UnknownTransactionException("unknown transaction");

        Transaction tn = (Transaction) txntr.getTransaction();
        ServerTransaction tr = serverTransaction(tn);

        if (tr == null)
           throw new UnknownTransactionException(
					     "TxnManagerImpl: getTransaction: "
                                             + "unable to find transaction(" +
					     id + ")");
//TODO - use IDs vs equals					     
        if (!tr.mgr.equals(this))
            throw new UnknownTransactionException("wrong manager (" + tr.mgr +
						  " instead of " + this + ")");

        return tr;
    }


    /**
     * Requests the renewal of  a lease on a <code>Transaction</code>.
     *
     * @param cookie identifies the leased resource
     *
     * @param extension requested lease extension
     *
     * @see net.jini.core.lease.Lease
     * @see com.sun.jini.landlord.LeasedResource
     * @see com.sun.jini.mahalo.LeaseManager
     */
    public long renew(Uuid uuid, long extension)
	        throws UnknownLeaseException, LeaseDeniedException
    {

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), "renew", 
	        new Object[] {uuid, new Long(extension)});
	}
        readyState.check();

        verifyLeaseUuid(uuid);
	Long tid = getLeaseTid(uuid);
	TxnManagerTransaction txntr =
	        (TxnManagerTransaction)txns.get(tid);

	if (txntr == null)
	    throw new UnknownLeaseException();

	// synchronize on the resource so there is not a race condition
	// between renew and expiration
	Result r;
	synchronized (txntr) {
//TODO - check for ACTIVE too?
//TODO - if post-ACTIVE, do anything?	
	    if (!ensureCurrent(txntr))
		throw new UnknownLeaseException("Lease already expired");
	    long oldExpiration = txntr.getExpiration();
            r = txnLeasePeriodPolicy.renew(txntr, extension);
	    txntr.setExpiration(r.expiration);
	    expMgr.renewed(txntr);
            if (operationsLogger.isLoggable(Level.FINER)) {
                operationsLogger.exiting(
		    TxnManagerImpl.class.getName(), "renew", 
	            new Object[] {new Long(r.duration)});
	    }
	    return r.duration;
	}
    }

    /**
     * Cancels the lease on a <code>Transaction</code>.
     *
     * @param cookie identifies the leased resource
     *
     * @see net.jini.core.lease.Lease
     * @see com.sun.jini.landlord.LeasedResource
     * @see com.sun.jini.mahalo.LeaseManager
     */
    public void cancel(Uuid uuid) throws UnknownLeaseException {

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "cancel", 
	        new Object[] {uuid});
	}
        readyState.check();
	
        verifyLeaseUuid(uuid);
	Long tid = getLeaseTid(uuid);
	TxnManagerTransaction txntr =
	        (TxnManagerTransaction)txns.get(tid);

	if (txntr == null)
	    throw new UnknownLeaseException();

	int state = txntr.getState();

/**
 * Add this back in once LeaseExpirationManager uses an overloaded version of cancel
 * that doesn't perform an expiration check.  LeaseExpirationManager calls cancel()
 * after the txn has expired, so can't reliably check expiration here.
 *
//TODO - need better locking here. getState and expiration need to be checked atomically	
        if ( (state == ACTIVE && !ensureCurrent(txntr)) ||
	     (state != ACTIVE))  
	    throw new UnknownLeaseException("unknown transaction");
**/	    

	if (state == ACTIVE) {

	    synchronized (txntr) {
	        txntr.setExpiration(0);	// Mark as done
	    }

	    try {
	        abort(((Long)tid).longValue());
	    } catch (TransactionException e) {
	        throw new
		    UnknownLeaseException("When canceling abort threw:" +
	        e.getClass().getName() + ":" + e.getLocalizedMessage());
	    }
	}
	
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "cancel");
	}
    }

    /**
     * Bulk renewal request of leases on <code>Transaction</code>s.
     *
     * @param cookies identifies the leased resources
     *
     * @param extensions requested lease extensions
     *
     * @see net.jini.core.lease.Lease
     * @see com.sun.jini.landlord.LeasedResource
     * @see com.sun.jini.mahalo.LeaseManager
     */
    public Landlord.RenewResults renewAll(Uuid[] cookies, long[] extensions) {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "renewAll");
	}
        readyState.check();

	Landlord.RenewResults results =
	    LandlordUtil.renewAll(this, cookies, extensions);
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "renewAll");
	}
	return results;
    }


    /**
     * Bulk cancel of leases on <code>Transaction</code>s.
     *
     * @param cookies identifies the leased resources
     *
     * @see net.jini.core.lease.Lease
     * @see com.sun.jini.landlord.LeasedResource
     * @see com.sun.jini.mahalo.LeaseManager
     */
    public Map cancelAll(Uuid[] cookies) {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "cancelAll");
	}
        readyState.check();

	Map results = LandlordUtil.cancelAll(this, cookies);
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "cancelAll");
	}
	return results;
    }

    // local methods

    /**
     * gets the next available transaction ID.
     */
    static long nextID() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "nextID");
	}
	long id;
	synchronized (idGen) {
	    do {
		id = 0;
		idGen.nextBytes(idGenBuf);
		for (int i = 0; i < 8; i++)
		    id = (id << 8) | (idGenBuf[i] & 0xFF);
	    } while (id == 0);				// skip flag value
	}
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), "nextID",
	        new Long(id));
	}
	return id;
    }



    private ServerTransaction serverTransaction(Transaction baseTr)
        throws UnknownTransactionException
    {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), 
	        "serverTransaction", baseTr);
	}
        try {
            if (operationsLogger.isLoggable(Level.FINER)) {
                operationsLogger.exiting(TxnManagerImpl.class.getName(), 
   	            "serverTransaction", baseTr);
	    }
            return (ServerTransaction) baseTr;
        } catch (ClassCastException e) {
            throw new UnknownTransactionException("unexpected transaction type");
        }
    }


    /**
     * Returns a reference to the <code>TransactionManager</code>
     * interface.
     *
     * @see net.jini.core.transaction.server.TransactionManager
     */
    public TransactionManager manager() {
        readyState.check();

        return txnMgrProxy;
    }


    private TxnManagerTransaction enterTMT(long cookie) {
	Long key = new Long(cookie);
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(TxnManagerImpl.class.getName(), 
	        "enterTMT", key);
	}
	TxnManagerTransaction tmt =
	        (TxnManagerTransaction) txns.get(key);

	if (tmt == null) {
            Uuid uuid = createLeaseUuid(cookie);
	    tmt = new TxnManagerTransaction(
	        txnMgrProxy, logmgr, cookie, taskpool, 
		taskWakeupMgr, this, uuid);
	    noteUnsettledTxn(cookie);
	    /* Since only aborted or committed txns are persisted,
	     * their expirations are irrelevant. Therefore, any recovered
	     * transactions are effectively lease.FOREVER. 
	     */
	}

	txns.put(key, tmt);

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), 
	        "enterTMT", tmt);
	}
	return tmt;
    }

    //***********************************************************
    // Admin

    // Methods required by DestroyAdmin

    /**
     * Cleans up and exits the transaction manager.
     */
    public void destroy() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "destroy");
	}
        readyState.check();

        (new DestroyThread()).start();
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "destroy");
	}
    }
 
    /** Maximum delay for unexport attempts */
    private final static long MAX_UNEXPORT_DELAY = 2 * MINUTES;
  
    /**
     * Termination thread code.  We do this in a separate thread to
     * avoid deadlock, because Activatable.inactive will block until
     * in-progress RMI calls are finished.
     */
    private class DestroyThread extends Thread {

        /** Create a non-daemon thread */
        public DestroyThread() {
            super("DestroyThread");
            /* override inheritance from RMI daemon thread */
            setDaemon(false);
        }

        public void run() {
            if (operationsLogger.isLoggable(Level.FINER)) {
               operationsLogger.entering(
		   DestroyThread.class.getName(), "run");
	    }

            Exception failed = null;

/**TODO 
  * - move this block into the destroy() method and let the 
  *   remote ex pass through
  */
            if (activationPrepared) {	    
	        try {
                    if(destroyLogger.isLoggable(Level.FINEST)) {
	                destroyLogger.log(Level.FINEST,
			    "Unregistering object.");
                    }
		    if (activationID != null)
                        activationSystem.unregisterObject(activationID);
   		} catch (RemoteException e) {
   		    /* give up until we can at least unregister */
                    if(destroyLogger.isLoggable(Level.WARNING)) {
	                destroyLogger.log(Level.WARNING,
			    "Trouble unregistering object -- aborting.", e);
                    }
   		    return;
   		} catch (ActivationException e) {
                    /*
                     * Activation system is shutting down or this
                     * object has already been unregistered --
                     * ignore in either case.
                     */
                    if(destroyLogger.isLoggable(Levels.HANDLED)) {
	                destroyLogger.log(Levels.HANDLED,
			    "Trouble unregistering object -- ignoring.", e);
                    }
   		}
	    }

            // Attempt to unexport this object -- nicely first
            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,
		    "Attempting unforced unexport.");
            }
            long endTime =
                System.currentTimeMillis() + MAX_UNEXPORT_DELAY;
            if (endTime < 0) { // Check for overflow
                endTime = Long.MAX_VALUE;
            }
            boolean unexported = false;
/**TODO 
  * - trap IllegalStateException from unexport
  */
            while ((!unexported) &&
                   (System.currentTimeMillis() < endTime)) {
                /* wait for any pending operations to complete */
                unexported = exporter.unexport(false);
                if (!unexported) {
                    if (destroyLogger.isLoggable(Level.FINEST)) {
                        destroyLogger.log(Level.FINEST, 
                            "Waiting for in-progress calls to complete");
                    }
                    try {
                        sleep(1000);
                    } catch (InterruptedException ie) {
                        if (destroyLogger.isLoggable(Levels.HANDLED)) {
                            destroyLogger.log(Levels.HANDLED, 
                                "problem unexporting nicely", ie);
                        }
                        break; //fall through to forced unexport
                   }
                } else {
                    if (destroyLogger.isLoggable(Level.FINEST)) {
                        destroyLogger.log(Level.FINEST, 
                            "Unexport completed");
                    }
                }      
            }

            // Attempt to forcefully unexport this object, if not already done
            if (!unexported) {
                if(destroyLogger.isLoggable(Level.FINEST)) {
	            destroyLogger.log(Level.FINEST,
		        "Attempting forced unexport.");
                }
		/* Attempt to forcefully export the service */
                unexported = exporter.unexport(true);
            }

            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Destroying JoinStateManager.");
	    }
	    try {
	        joinStateManager.destroy();
            } catch (Exception t) {
                if(destroyLogger.isLoggable(Levels.HANDLED)) {
	            destroyLogger.log(Levels.HANDLED, 
		        "Problem destroying JoinStateManager", t);
		}
            }

	    //
            // Attempt to stop all running threads
            //
            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Terminating lease expiration manager.");
	    }
	    expMgr.terminate();

            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Interrupting settleThread.");
            }
	    settleThread.interrupt();
            try {
                settleThread.join();
            } catch (InterruptedException ie) {
                if(destroyLogger.isLoggable(Levels.HANDLED)) {
	            destroyLogger.log(Levels.HANDLED, 
		        "Problem stopping settleThread", ie);
		}
            }

            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Terminating settlerpool.");
            }
	    settlerpool.terminate();
	    settlerWakeupMgr.stop();
	    settlerWakeupMgr.cancelAll();

            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Terminating taskpool.");
            }
	    taskpool.terminate();
	    taskWakeupMgr.stop();
            taskWakeupMgr.cancelAll();


	    // Remove persistent store- ask LogManager to clean
	    // itself up, then clean up the persistence path.
            if(destroyLogger.isLoggable(Level.FINEST)) {
	        destroyLogger.log(Level.FINEST,"Destroying transaction logs.");
	    }
	    MultiLogManagerAdmin logadmin =
	        (MultiLogManagerAdmin) logmgr.getAdmin();
	
	    logadmin.destroy();

	    if (persistent) {	    
                if(destroyLogger.isLoggable(Level.FINEST)) {
	            destroyLogger.log(Level.FINEST,"Destroying persistence directory.");
  	        }
	        try {
	            com.sun.jini.system.FileSystem.destroy(
		        new File(persistenceDirectory), true);
	        } catch (IOException e) {
                    if(destroyLogger.isLoggable(Levels.HANDLED)) {
	                destroyLogger.log(Levels.HANDLED, 
		            "Problem destroying persistence directory", e);
		    }
    	        }
	    }
	    
            if(activationID != null) {
	        if(destroyLogger.isLoggable(Level.FINEST)) {
	            destroyLogger.log(Level.FINEST,"Calling Activatable.inactive.");
                }
	        try {
                    Activatable.inactive(activationID);
                } catch (RemoteException e) { // ignore
                    if(destroyLogger.isLoggable(Levels.HANDLED)) {
	                destroyLogger.log(Levels.HANDLED, 
		            "Problem inactivating service", e);
		    }
                } catch (ActivationException e) { // ignore
                    if(destroyLogger.isLoggable(Levels.HANDLED)) {
	                destroyLogger.log(Levels.HANDLED, 
		            "Problem inactivating service", e);
		    }
                }
            }

	    if (lifeCycle != null) {
	        if(destroyLogger.isLoggable(Level.FINEST)) {
	            destroyLogger.log(Level.FINEST,
		        "Unregistering with LifeCycle.");
                }
		lifeCycle.unregister(TxnManagerImpl.this);
	    }
	    
	    if (loginContext != null) {
	        try {
		    if (destroyLogger.isLoggable(Level.FINEST)) {
		        destroyLogger.log(Level.FINEST,
			    "Logging out");
		    }
		    loginContext.logout();
	        } catch (Exception e) {
		    if (destroyLogger.isLoggable(Levels.HANDLED)) {
		        destroyLogger.log(Levels.HANDLED,
			    "Exception while logging out", 
			    e);
		    }
	        }
	    }
            readyState.shutdown();
			
            if (operationsLogger.isLoggable(Level.FINER)) {
                operationsLogger.exiting(
		    DestroyThread.class.getName(), "run");
	    }
	}
    }

    /**
     * Returns the administration object for the
     * transaction manager.
     */
    public Object getAdmin() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
	        TxnManagerImpl.class.getName(), "getAdmin");
	}
        readyState.check();

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
	        TxnManagerImpl.class.getName(), "getAdmin", txnMgrAdminProxy);
	}
	return txnMgrAdminProxy;
    }

    // Methods required by JoinAdmin
    // Inherit java doc from super type
    public Entry[] getLookupAttributes() {
        readyState.check();

	return joinStateManager.getLookupAttributes();
    }

    // Inherit java doc from super type
    public void addLookupAttributes(Entry[] attrSets) {
        readyState.check();
        
	joinStateManager.addLookupAttributes(attrSets);
    }

    // Inherit java doc from super type
    public void modifyLookupAttributes(Entry[] attrSetTemplates, 
				       Entry[] attrSets) 
    {
        readyState.check();

	joinStateManager.modifyLookupAttributes(attrSetTemplates, attrSets);
    }
  
    // Inherit java doc from super type
    public String[] getLookupGroups() {
        readyState.check();

	return joinStateManager.getLookupGroups();
    }

    // Inherit java doc from super type
    public void addLookupGroups(String[] groups) {
        readyState.check();

	joinStateManager.addLookupGroups(groups);
    }

    // Inherit java doc from super type
    public void removeLookupGroups(String[] groups) {
        readyState.check();

	joinStateManager.removeLookupGroups(groups);
    }

    // Inherit java doc from super type
    public void setLookupGroups(String[] groups) {
        readyState.check();

	joinStateManager.setLookupGroups(groups);
    }

    // Inherit java doc from super type
    public LookupLocator[] getLookupLocators() {
        readyState.check();

	return joinStateManager.getLookupLocators();
    }

    // Inherit java doc from super type
    public void addLookupLocators(LookupLocator[] locators) 
        throws RemoteException
    {
        readyState.check();

	joinStateManager.addLookupLocators(locators);
    }

    // Inherit java doc from super type
    public void removeLookupLocators(LookupLocator[] locators)  
        throws RemoteException
    {
        readyState.check();

        joinStateManager.removeLookupLocators(locators);
    }

    // Inherit java doc from super type
    public void setLookupLocators(LookupLocator[] locators)  
        throws RemoteException
    {
        readyState.check();

	joinStateManager.setLookupLocators(locators);
    }


    //***********************************************************
    // Startup

    /**
     * Create the service owned attributes for an Mahalo server
     */
    private static Entry[] attributesFor() {
	final Entry info = new ServiceInfo("Transaction Manager", 
	    "Sun Microsystems, Inc.",  "Sun Microsystems, Inc.", 
	    com.sun.jini.constants.VersionConstants.SERVER_VERSION,
	    "", "");
	
	final Entry type = 
	    new com.sun.jini.lookup.entry.BasicServiceType("Transaction Manager");

	return new Entry[]{info, type};
    }

    public Object getProxy() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
	        TxnManagerImpl.class.getName(), "getProxy");
	}
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
	        TxnManagerImpl.class.getName(), "getProxy", serverStub);
	}
	return serverStub;
    }
    
    /* inherit javadoc */
    public Object getServiceProxy() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
	        TxnManagerImpl.class.getName(), "getServiceProxy");
	}
        readyState.check();

        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
	        TxnManagerImpl.class.getName(), "getServiceProxy", 
	        txnMgrProxy);
	}
        return txnMgrProxy;
    }
 
    /**
     * Log information about failing to initialize the service and rethrow the
     * appropriate exception.
     *
     * @param e the exception produced by the failure
     */
    protected void initFailed(Throwable e) throws Exception {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
	        TxnManagerImpl.class.getName(), "initFailed");
	}
        if(initLogger.isLoggable(Level.SEVERE)) {
            initLogger.log(Level.SEVERE, "Mahalo failed to initialize", e);
        }
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
	        TxnManagerImpl.class.getName(), "initFailed");
	}
	if (e instanceof Exception) {
            throw (Exception) e;
        } else if (e instanceof Error) {
            throw (Error) e;
        } else {
            IllegalStateException ise =
                new IllegalStateException(e.getMessage());
            ise.initCause(e);
            throw ise;
        }
    }
    
    /*
     *
     */
    private void cleanup() {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.entering(
		TxnManagerImpl.class.getName(), "cleanup");
	}
//TODO - add custom logic	
        if (serverStub != null) { // implies that exporter != null
	    try {
                if(initLogger.isLoggable(Level.FINEST)) {
	            initLogger.log(Level.FINEST, "Unexporting service");		    
	        }
		exporter.unexport(true);
	    } catch (Throwable t) {
                if(initLogger.isLoggable(Levels.HANDLED)) {
	            initLogger.log(Levels.HANDLED, "Trouble unexporting service", t);
		}		    
	    }
	}
	
	if (settlerpool != null)  {
             if(initLogger.isLoggable(Level.FINEST)) {
	         initLogger.log(Level.FINEST, "Terminating settlerpool.");
             }
	     try {
                settlerpool.terminate();
	        if (settlerWakeupMgr != null)  {
                    if(initLogger.isLoggable(Level.FINEST)) {
	                 initLogger.log(Level.FINEST, 
			     "Terminating settlerWakeupMgr.");
	            }
		    settlerWakeupMgr.stop();
	            settlerWakeupMgr.cancelAll();
		}
            } catch (Throwable t) {
                if(initLogger.isLoggable(Levels.HANDLED)) {
	            initLogger.log(Levels.HANDLED, 
		        "Trouble terminating settlerpool", t);
		}		    
            }
        }
	
	if (taskpool != null)  {
             if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST,"Terminating taskpool.");
	     }
	     try {
                taskpool.terminate();
	        if (taskWakeupMgr != null)  {
                    if(initLogger.isLoggable(Level.FINEST)) {
	                 initLogger.log(Level.FINEST,
			     "Terminating taskWakeupMgr.");
		    }
		    taskWakeupMgr.stop();
		    taskWakeupMgr.cancelAll();
		}
            } catch (Throwable t) {
                if(initLogger.isLoggable(Levels.HANDLED)) {
	            initLogger.log(Levels.HANDLED, 
		        "Trouble terminating taskpool", t);
		}		    
            }
        }
	
	if (settleThread != null) {
            if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, "Interrupting settleThread.");
            }
	    try {
                settleThread.interrupt();
            } catch (Throwable t) {
                if(initLogger.isLoggable(Levels.HANDLED)) {
	            initLogger.log(Levels.HANDLED, 
		        "Trouble terminating settleThread", t);	
		}	    
            }
	}
	
        if (expMgr != null)  {
            if(initLogger.isLoggable(Level.FINEST)) {
	        initLogger.log(Level.FINEST, 
		    "Terminating lease expiration manager.");
            }
	    expMgr.terminate();
	}

        if(initLogger.isLoggable(Level.FINEST)) {
	    initLogger.log(Level.FINEST,"Destroying JoinStateManager.");
        }
	try {
	    if (joinStateManager != null) {
		joinStateManager.stop();
	    }
        } catch (Exception t) {
            if(initLogger.isLoggable(Levels.HANDLED)) {
	        initLogger.log(Levels.HANDLED, 
	            "Problem destroying JoinStateManager", t);
            }
	}
	
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(
		TxnManagerImpl.class.getName(), "cleanup");
	}
    }
    
    //////////////////////////////////////////
    // ProxyTrust Method
    //////////////////////////////////////////
    public TrustVerifier getProxyVerifier( ) {
        if (operationsLogger.isLoggable(Level.FINER)) {
            operationsLogger.exiting(TxnManagerImpl.class.getName(), 
	        "getProxyVerifier");
	}
        readyState.check();

        /* No verifier if the server isn't secure */
        if (!(txnMgrProxy instanceof RemoteMethodControl)) {
            throw new UnsupportedOperationException();
        } else {
            if (operationsLogger.isLoggable(Level.FINER)) {
                operationsLogger.exiting(TxnManagerImpl.class.getName(), 
   	            "getProxyVerifier");
	    }
            return new ProxyVerifier(serverStub, topUuid);
        }
    }


    /**
     * Utility method that check for valid resource
     */
    private static boolean ensureCurrent(LeasedResource resource) {
        return resource.getExpiration() > System.currentTimeMillis();
    }

    /*
     * Attempt to build "real" Uuid from 
     * topUuid.getLeastSignificantBits(), which contains
     * the variant field, and the transaction id, which
     * should be unique for this service. Between the two 
     * of these, the Uuid should be unique.
     */
    private Uuid createLeaseUuid(long txnId) {
        return UuidFactory.create(
	    topUuid.getLeastSignificantBits(),
	    txnId);
    }

    private void verifyLeaseUuid(Uuid uuid) throws UnknownLeaseException {
	/*
	 * Note: Lease Uuid contains
	 * - Most Sig => the least sig bits of topUuid
	 * - Least Sig => the txn id
	 */
	// Check to if this server granted the resource
	if (uuid.getMostSignificantBits() != 
	    topUuid.getLeastSignificantBits()) 
	{
	    throw new UnknownLeaseException();
	}

    }

    private Long getLeaseTid(Uuid uuid) {
	// Extract the txn id from the lower bits of the uuid
        return new Long(uuid.getLeastSignificantBits());
    }
}
