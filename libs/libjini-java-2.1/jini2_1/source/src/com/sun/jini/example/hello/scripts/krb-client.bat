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
@rem Batch file to run Kerberos client

@rem Get the environment variables setup
call scripts/krb-setenv.bat

java -Djava.security.manager= ^
     -Djava.security.policy=config\krb-client.policy ^
     -Djava.security.properties=config\dynamic-policy.security-properties ^
     -Djava.security.auth.login.config=config\krb-client.login ^
     -Djava.protocol.handler.pkgs=net.jini.url ^
     -Djava.rmi.server.RMIClassLoaderSpi=com.sun.jini.example.hello.MdClassAnnotationProvider ^
     -Dexport.codebase.source.jsk=..\..\..\..\..\..\..\lib-dl ^
     -Dexport.codebase.jsk=httpmd://%computername%:8080/jsk-dl.jar;sha=0 ^
     -DclientPrincipal=%CLIENT%@%REALM% ^
     -DserverPrincipal=%SERVER%@%REALM% ^
     -DphoenixPrincipal=%PHOENIX%@%REALM% ^
     -DreggiePrincipal=%REGGIE%@%REALM% ^
     -Djava.security.krb5.realm=%REALM% ^
     -Djava.security.krb5.kdc=%KDC_HOST% ^
     -jar lib\client.jar ^
     config\krb-client.config
