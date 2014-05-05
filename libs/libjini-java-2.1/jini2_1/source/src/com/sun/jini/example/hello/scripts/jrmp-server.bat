@rem /*
@rem 
@rem Copyright 2005 Sun Microsystems, Inc.
@rem 
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem 
@rem 	http://www.apache.org/licenses/LICENSE-2.0
@rem 
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.
@rem 
@rem */
@rem Batch file to run JRMP server

java -Djava.security.manager= ^
     -Djava.security.policy=config\server.policy ^
     -Djava.rmi.server.codebase="http://%computername%:8080/server-dl.jar http://%computername%:8080/jsk-dl.jar" ^
     -jar lib\server.jar ^
     config\jrmp-server.config
