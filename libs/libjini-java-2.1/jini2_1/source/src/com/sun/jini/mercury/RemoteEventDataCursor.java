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
/**
 * Trival class (struct) that simply holds the current read count
 * and the associated (next unread) read position. U?sed as the client-side
 * cookie for PersistentLog.
 * @since 2.1
 */
class RemoteEventDataCursor implements Serializable {
    private static final long serialVersionUID = 1L;

    private final long readCount;
    private final long readPosition;

    RemoteEventDataCursor(long count, long cursor) {
        readCount = count;
        readPosition = cursor;
    }

    long getReadCount() { return readCount; }
    long getReadPosition() { return readPosition; }
}
