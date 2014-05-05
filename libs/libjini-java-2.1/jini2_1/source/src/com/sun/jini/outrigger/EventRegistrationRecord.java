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
package com.sun.jini.outrigger;

import net.jini.id.Uuid;
import com.sun.jini.landlord.LeasedResource;

/**
 * Common functionally shared between new entry and (re)visibility
 * events. Allows <code>OutriggerServerImpl</code> to handle both with
 * the same code.
 */
interface EventRegistrationRecord extends LeasedResource {
    /**
     * Make sure this object will be removed from the system
     * independent of the expiration time.
     * @return <code>true</code> if this call was
     * the first removal attempt.
     */
    public boolean cancel();
}
