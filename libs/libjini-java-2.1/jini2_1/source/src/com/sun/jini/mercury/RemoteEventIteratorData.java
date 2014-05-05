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
package com.sun.jini.mercury;

import java.io.Serializable;
import java.util.Collection;

import net.jini.id.Uuid;

/**
 * Simple struct to hold the <code>Uuid</code> for a new
 * <code>RemoteEventIterator</code> instance and the first batch of
 * data. 
 */
class RemoteEventIteratorData implements Serializable {
    private static final long serialVersionUID = 1L;

    /**
     * <code>Uuid</code> for iteration.
     */
    final Uuid uuid;

    /** Initial set of entries */
    final Collection events;

    /**
     * Creates a new RemoteEventIteratorData instance.
     * @param uuid value of <code>uuid</code> field.
     * @param reps value of <code>events</code> field.
     * @param intialLeaseDuration value of <code>intialLeaseDuration</code> 
     *        field.
     */
    RemoteEventIteratorData(Uuid uuid, Collection events) {
	this.uuid = uuid;
	this.events = events;
    }
}
