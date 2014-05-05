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

package com.sun.jini.example.hello;

import java.rmi.Remote;
import java.rmi.RemoteException;

/**
 * Defines a simple remote interface.
 *
 * @author Sun Microsystems, Inc.
 * 
 */
public interface Hello extends Remote {

    /**
     * Returns a message string.
     *
     * @return message string
     * @throws RemoteException if a remote communication problem occurs
     */
    String sayHello() throws RemoteException;
}
