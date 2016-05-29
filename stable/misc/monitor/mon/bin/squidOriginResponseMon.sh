#!/bin/bash

# Chinacache SquidDevTeam
# $Id: squidOriginResponseMon.sh 5585 2008-11-19 05:53:41Z jyp $
#@version  : 0.0.2
#@function  : monitor squid's original server response status.

DEBUG=0
DATAFILE='/var/named/chroot/var/named/anyhost'
ResTime=0
INTERVAL=6
#www.sohu.com IN A 192.168.1.240 ; 0.123456 ip ip_work 200 Y
keyAnalysis(){
local KEYWORD=${10}
local RT=$6

case "$KEYWORD" in
    "Y")
        #0 in 0.123456

        Result=`echo "($ResTime - $RT) > 0"|bc`

        [ $Result -eq 0 ]&&ResTime=$RT
        return 0
    ;;

    *)
        return 1
    ;;
esac
}
#------------------------------------------------------------------------
while read  Line; do
set -- $Line
#$2 is $HOST
keyAnalysis $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10}
done < $DATAFILE

[ $DEBUG -gt 0 ]&&echo "ResTime  $ResTime"

ResMin=$(echo "$ResTime * 1000"|bc )

[ $DEBUG -gt 0 ]&& echo "ResMin=$ResMin"

[ $DEBUG -gt 0 ]&&printf "printf ResMin %%.f: %.f\n" $ResMin

printf "squidOriginResponseTime: %.f\n" $ResMin

echo "squidOriginResponseSpeedInKBps: 1"
echo "squidOriginResponseUpdateTime: $(date +%s)"
exit 0
