
#  "LOG", the circuit editing and simulation system, 5.64
#  "DigLOG", the digital simulator for LOG.
#  Copyright (C) 1985, 1990, 1993 David Gillespie.
#  Author's address: daveg@synaptics

#  "AnaLOG", the analog simulator for LOG, 5.64 .
#  Copyright (C) 1985, 1990, 1993 John Lazzaro.
#  Author's address: lazzaro@cs.berkeley.edu;

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation (any version).

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.




LOCINC = ../../../psys/include
XINCLUDEDIR = .

##
##  (1) Choosing your compiler
##  ---------------------------
##
## For most Unix architectures, a stable gcc 2.X will work well.
##
## For IBM RS/6000, xlc is recommended.
##
## Note than for HP series 700/800, the default cc won't work; the
## extra-cost ANSI cc is needed. Gcc works well on HP series 700/800.
##
## For AmigaOS, ADE (Amiga Development Enviroment) with gcc is recommended.
##
##
  

CHIPMUNKCC = gcc -O3


CC = $(CHIPMUNKCC) 

##
## (2) Adding appropriate flags
## -----------------------------
##
## Add to CHIPMUNKFLAGS below:
##
## Linux/86                         -DBSD -Dlinux -DF_OK=0 -m486
## Solaris 2.X                      -Daux 
## DEC Alpha (Linux or OSF)         -DBSD -Dlinux -DF_OK=0 -D__GCC__
## HP series 800 (gcc)
## HP series 800 (cc)               -w -Aa -D_HPUX_SOURCE -DF_OK=0
## FreeBSD                          -DBSD -Dfreebsd -Dlinux -DF_OK=0 -m486
## SGI Irix 5.2/5,3                 -ansi -Wf -XNd10000 
##                                   (if   -Wf -XNd10000 isn't supported by
##                                    your compiler, delete them)
## Solaris 1.X (SunOS)              
## IBM RS/6000 (xlc)                -DBSD=0 -Drs6000 
## Apple OS X                       -DBSD -Dlinux -Dfreebsd
## Apple AU/X                       -Daux
## Apple MachTen                    -DBSD -DF_OK=0  
## AmigaOS (gcc)                    -ansi -DBSD -DF_OK=0
##
##
## AmigaOS: if you are using X shared library with ADE, see notes below
## in log and loged rules, also
## to increase speed, try to add -O3 and, based on your CPU, one of these
##
##   -m68020              : for 68020
##   -m68020 -m68881      : for 68020+6888x
##   -m68030              : for 68030
##   -m68030 -m68881      : for 68030+6888x
##   -m68040              : for 68040 and 68060
## 
  

CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0

CFLAGS =  $(CHIPMUNKFLAGS) -I$(LOCINC) -I$(XINCLUDEDIR)

ANAOBJ = globals.o nsolver.o main.o inter.o $(GATESOBJ)
GATESOBJ = numbers.o mmeter.o iscope.o \
	   vdiff.o vswitch.o idiff.o iswitch1.o iswitch2.o stairs.o \
	   resfloat.o capfloat.o nfet4.o pfet4.o \
	   nfet5.o pfet5.o pfet6.o pspc1.o nspc1.o devtechp.o\
	   nfet7t.o nfet7f.o pfet7t.o pfet7f.o thermal.o physical.o\
	   tc.o moscap.o ganglion.o hres.o runspec.o\
	   npn1.o pnp1.o pnp2.o npn2.o diode1.o pwl.o rtd.o devtechn.o 

# Note:  The above list is also present in ../Makefile.



all: grab $(ANAOBJ)


grab: logdef.h

logdef.h: ../logdef.h
	cp ../logdef.h .











