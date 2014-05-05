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
package com.sun.jini.thread;

import java.util.logging.Level;
import java.util.logging.Logger;
import com.sun.jini.constants.TimeConstants;

/**
 * An abstract class for building tasks that retry on failure after a
 * timeout period.  This builds upon <code>TaskManager</code> for task
 * execution and <code>WakeupManager</code> for retry scheduling.  You
 * extend <code>RetryTask</code>, implementing <code>tryOnce</code> to
 * represent a single attempt at the task.  If <code>tryOnce</code>
 * returns <code>true</code>, the attempt was successful and the task
 * is complete.  If <code>tryOnce</code> returns <code>false</code>,
 * the task is scheduled for a future retry using <code>WakeupManager</code>.
 * <p>
 * The default retry times are defined by this class's implementation
 * of <code>retryTime</code></code>.  You can override this method to
 * change the retry times.
 * <p>
 * It is legal to reuse the same task again and again, but only after
 * the task is complete and <code>reset</code> has been called.
 * Inserting the same task multiple times before it has either been
 * cancelled or completed successfully will generate unpredictable
 * behavior.<p>
 *
 * This class uses the {@link Logger} named
 * <code>com.sun.jini.thread.RetryTask</code> to log information at
 * the following logging levels: <p>
 *
 * <table border=1 cellpadding=5
 *       summary="Describes logging performed by RetryTask at different
 *	          logging levels">
 *
 * <tr> <th> Level <th> Description
 *
 * <tr> <td> FINEST <td> after a failed attempt, when should 
 *                       the task be scheduled for a re-try
 *
 * </table>
 *
 * @author Sun Microsystems, Inc.
 *
 * @see TaskManager
 * @see WakeupManager
 */
import com.sun.jini.thread.WakeupManager.Ticket;

public abstract class RetryTask implements TaskManager.Task, TimeConstants {
    private TaskManager	  manager;	// the TaskManager for this task
    private RetryTime	  retry;	// the retry object for this task
    private boolean	  cancelled;	// have we been cancelled?
    private boolean	  complete;	// have we completed successfully?
    private Ticket	  ticket;	// the WakeupManager ticket
    private long	  startTime;	// the time when we were created or 
                                        //   last reset
    private int		  attempt;	// the current attempt number
    private WakeupManager wakeup;       // WakeupManager for retry scheduling

    /**
     * Default delay backoff times.  These are converted from
     * intervals to "time since start" by the static block below.
     *
     * @see #retryTime 
     */
    private static final long[] delays = {
	 0, // First value is never read
	 1 * SECONDS,
	 5 * SECONDS,
	10 * SECONDS,
	 1 * MINUTES,
	 1 * MINUTES,
	 5 * MINUTES,
    };

    /** Logger for this class */
    private static final Logger logger = 
	Logger.getLogger("com.sun.jini.thread.RetryTask");

    /**
     * Create a new <code>RetryTask</code> that will be scheduled with
     * the given task manager, and which will perform retry scheduling 
     * using the given wakeup manager.
     */
    public RetryTask(TaskManager manager, WakeupManager wakeupManager) {
	this.manager = manager;
        this.wakeup = wakeupManager;
	reset();
    }

    /**
     * Make a single attempt.  Return <code>true</code> if the attempt
     * was successful.  If the attempt is not successful, the task
     * will be scheduled for a future retry.
     */
    public abstract boolean tryOnce();

    /**
     * The <code>run</code> method used as a
     * <code>TaskManager&#046;Task</code>.  This invokes
     * <code>tryOnce</code>.  If it is not successful, it schedules
     * the task for a future retry at the time it gets by invoking
     * <code>retryTime</code>.
     *
     * @see #tryOnce
     * @see #startTime 
     */
    public void run() {
	synchronized (this) {		// avoid retry if cancelled
	    if (cancelled)		// if they cancelled
		return;			// do nothing
	}

	boolean success = tryOnce();

	synchronized (this) {
	    if (!success) {		// if at first we don't succeed ...
		attempt++;
		long at = retryTime();	// ... try, try again

		if (logger.isLoggable(Level.FINEST)) {
		    logger.log(Level.FINEST, "retry of {0} in {1} ms", 
		        new Object[]{this, 
			    new Long(at - System.currentTimeMillis())});
		}

		if (retry == null)	// only create it if we need to
		    retry = new RetryTime();
		ticket = wakeup.schedule(at, retry);
	    } else {
		complete = true;
		notifyAll();		// see waitFor()
	    }
	}
    }

    /**
     * Return the next time at which we should make another attempt.
     * This is <em>not</em> an interval, but the actual time.
     * <p>
     * The implementation is free to do as it pleases with the policy
     * here.  The default implementation is to delay using intervals of
     * 1 second, 5 seconds, 10 seconds, 1 minute, and 1 minute between
     * attempts, and then retrying every five minutes forever.
     * <p>
     * The default implementation assumes it is being called from
     * the default <code>run</code> method and that the current thread
     * holds the lock on this object. If the caller does
     * not own the lock the result is undefined and could result in an
     * exception.
     */
    public long retryTime() {
	assert Thread.holdsLock(this): "thread does not hold lock";
	int index = (attempt < delays.length ? attempt : delays.length - 1); 
	return delays[index] + System.currentTimeMillis();
    }

    /**
     * Return the time this task was created, or the last
     * time {@link #reset reset} was called.
     */
    public synchronized long startTime() {
	return startTime;
    }

    /**
     * Return the attempt number, starting with zero.
     */
    public synchronized int attempt() {
	return attempt;
    }

    /**
     * Cancel the retrying of the task.  This ensures that there will be
     * no further attempts to invoke <code>tryOnce</code>.  It will not
     * interfere with any ongoing invocation of <code>tryOnce</code>
     * unless a subclass overrides this to do so.  Any override of this
     * method should invoke <code>super.cancel()</code>.
     */
    public synchronized void cancel() {
	cancelled = true;
	if (ticket != null)
	    wakeup.cancel(ticket);
	notifyAll();		// see waitFor()
    }

    /**
     * Return <code>true</code> if <code>cancel</code> has been invoked.
     */
    public synchronized boolean cancelled() {
	return cancelled;
    }

    /**
     * Return <code>true</code> if <code>tryOnce</code> has returned
     * successfully.
     */
    public synchronized boolean complete() {
	return complete;
    }

    public synchronized boolean waitFor() throws InterruptedException {
	while (!cancelled && !complete)
	    wait();
	return complete;
    }

    /**
     * Reset values for a new use of this task.
     */
    public synchronized void reset() {
	cancel();		// remove from the wakeup queue
	startTime = System.currentTimeMillis();
	cancelled = false;
	complete = false;
	ticket = null;
	attempt = 0;
    }

    /**
     * This is the runnable class for the <code>WakeupManager</code>,
     * since we need different implementations of
     * <code>WakeupManager&#046;run</code> and
     * <code>TaskManager&#046;run</code>.  
     */
    private class RetryTime implements Runnable {
	/**
	 * Time to retry the task.
	 */
	public void run() {
	    synchronized (RetryTask.this) {
		ticket = null;
	    }
	    manager.add(RetryTask.this);
	}
    };
}
