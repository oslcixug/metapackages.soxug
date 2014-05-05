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
@rem Batch file to run SSL Reggie

java -Djava.security.manager= ^
     -Djava.security.policy=config\ssl-reggie.policy ^
     -Djava.security.auth.login.config=config\ssl-reggie.login ^
     -Djava.security.properties=config\dynamic-policy.security-properties ^
     -Djavax.net.ssl.trustStore=prebuiltkeys\truststore ^
     -Djava.protocol.handler.pkgs=net.jini.url ^
     -jar ..\..\..\..\..\..\..\lib\start.jar ^
     config\start-ssl-reggie.config
