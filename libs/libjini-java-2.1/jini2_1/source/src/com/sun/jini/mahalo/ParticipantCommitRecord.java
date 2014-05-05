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

import com.sun.jini.mahalo.log.*;

import com.sun.jini.mahalo.ParticipantHandle;
import net.jini.core.transaction.server.TransactionManager;
import net.jini.core.transaction.server.TransactionParticipant;
import java.io.Serializable;


/**
 * A <code>LogRecord</code> which encapsulates a participant
 * being instructed to roll-forward.
 *
 * @author Sun Microsystems, Inc.
 *
 */

class ParticipantCommitRecord extends ParticipantModRecord {
    static final long serialVersionUID = -881052193077840308L;

    ParticipantCommitRecord(ParticipantHandle part) {
	super(part,COMMITTED);
    }
}
