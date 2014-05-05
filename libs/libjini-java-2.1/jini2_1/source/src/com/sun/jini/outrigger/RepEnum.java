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

import java.util.Enumeration;

/**
 * An internal interface for enumerating through <code>EntryRep</code>
 * objects inside a JavaSpace server.
 *
 * @author Sun Microsystems, Inc.
 *
 * @see EntryHolder#contents
 * @see OutriggerServerImpl#AllReps
 */
interface RepEnum {
    /**
     * Return the next <code>EntryRep</code> object, or <code>null</code>
     * if there is none.  Repeated calls at the end simply continue to return
     * <code>null</code>.
     */
    EntryRep nextRep();
}
