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

package net.jini.core.lease;

import java.rmi.RemoteException;

/**
 * An unsynchronized Map from Lease to Long (the duration to use when
 * renewing the lease).  For all methods of LeaseMap except canContainKey,
 * an IllegalArgumentException is thrown if a key is not a Lease or a value
 * is not a Long.  Null keys and values are not supported.
 *
 * @author Sun Microsystems, Inc.
 *
 * @since 1.0
 */
public interface LeaseMap extends java.util.Map {
    /**
     * Returns true if the given object is a Lease which can be renewed
     * and cancelled in a batch with other leases in the map.  Whether
     * or not two Lease objects can be batched is an implementation
     * detail determined by the objects.
     *
     * @param key  an object that should be a lease
     * @return true if the lease can be renewed and cancelled in a 
     *              a batch with other leases
     *
     * @see Lease#canBatch
     */
    boolean canContainKey(Object key);

    /**
     * Renews all leases in the LeaseMap, using their current values as
     * the renewal durations.  If all renewals are successful, returns
     * normally.  Otherwise, removes all leases that failed to renew
     * from the LeaseMap, and throws LeaseMapException.
     * 
     * @throws LeaseMapException
     * @throws java.rmi.RemoteException
     */
    void renewAll() throws LeaseMapException, RemoteException;

    /**
     * Cancels all leases in the LeaseMap.  If all cancels are successful,
     * returns normally (leaving all leases in the map).  Otherwise,
     * removes all leases that failed to cancel from the LeaseMap, and
     * throws LeaseMapException.
     * 
     * @throws LeaseMapException
     * @throws java.rmi.RemoteException
     */
    void cancelAll() throws LeaseMapException, RemoteException;
}
