#!/bin/bash

# Chinacache SquidDevTeam
# $Id: monitorReInstall.sh 5867 2008-12-03 04:16:38Z jyp $	

PWD=`pwd`
Now=${PWD}
cd monitor
make -s uninstall
make
make -s install
crontabInstall.sh
cd ${Now}/detectorig
make uninstall
make
make install
crontabInstall.sh
cd $Now
yes|cp -f ChinaCache-MIB.txt /usr/share/snmp/mibs/
