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
package com.sun.jini.reggie;

import com.sun.jini.start.LifeCycle;

/**
 * Class for starting transient lookup services.
 *
 * @author Sun Microsystems, Inc.
 */
class TransientRegistrarImpl extends RegistrarImpl {

    /**
     * Constructs a TransientRegistrarImpl based on a configuration obtained
     * using the provided arguments.  If lifeCycle is non-null, then its
     * unregister method is invoked during service shutdown.
     */
    TransientRegistrarImpl(String[] configArgs, LifeCycle lifeCycle)
	throws Exception
    {
	super(configArgs, null, false, lifeCycle);
    }
}
