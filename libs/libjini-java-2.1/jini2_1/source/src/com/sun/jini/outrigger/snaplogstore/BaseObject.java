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
package com.sun.jini.outrigger.snaplogstore;

import com.sun.jini.outrigger.StorableObject;
import com.sun.jini.outrigger.StoredObject;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import net.jini.space.InternalSpaceException;

/**
 * Top level wrapper class for persisting outrigger objects.
 * The target object is serialized and stored here as a byte
 * array.
 */
class BaseObject implements StoredObject, Serializable {
    static final long serialVersionUID = -400804064969360164L;

    private byte[]	blob;

    BaseObject(StorableObject object) {
	try {
	    ByteArrayOutputStream baos = new ByteArrayOutputStream();
	    ObjectOutputStream oos = new ObjectOutputStream(baos);
	    object.store(oos);
	    oos.flush();
	    blob = baos.toByteArray();
	    oos.close();
	} catch (IOException e) {
	    throw new InternalSpaceException("Exception serializing resource", e);
	}
    }

    public void restore(StorableObject object)
      throws IOException, ClassNotFoundException {
	ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(blob));
	object.restore(ois);
	ois.close();
    }
}
