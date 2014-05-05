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
package net.jini.discovery;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Encapsulate the details of marshaling a unicast request.
 *
 * @author Sun Microsystems, Inc.
 *
 * @see IncomingUnicastRequest
 */
public class OutgoingUnicastRequest {
    /**
     * The current version of the unicast discovery protocol.
     */
    protected static final int protoVersion = 1;

    /**
     * Marshal a unicast request to the given output stream.  The
     * stream is flushed afterwards.
     *
     * @param str the stream to marshal to
     * @exception IOException a problem occurred during marshaling
     */
    public static void marshal(OutputStream str) throws IOException {
	DataOutputStream dstr = new DataOutputStream(str);
	dstr.writeInt(protoVersion);
	dstr.flush();
    }
}
