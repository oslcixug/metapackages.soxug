#! /bin/sh
#/*
# 
# Copyright 2005 Sun Microsystems, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# 	http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
#*/

# Shell script to run Jini ERI Phoenix

# The server host
host=`hostname`

set -x

java -Djava.security.manager= 						\
     -Djava.security.policy=config/phoenix.policy			\
     -Djava.rmi.server.codebase="http://$host:8080/phoenix-dl.jar http://$host:8080/jsk-dl.jar"	\
     -DserverHost=$host 						\
     -jar ../../../../../../../lib/phoenix.jar				\
     config/jeri-phoenix.config
