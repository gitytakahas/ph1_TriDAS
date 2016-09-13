#!/bin/bash
#echo "We are working on $CONFDB account..."

TIMEFORMAT="%E" 
ROOTDIR="/exports/xdaq/dev35/FecSoftwareV3_0/ThirdParty/DatabaseDebugger/"  
XMLFILEPATH=$ROOTDIR"xml/"
EXECPROG=$ROOTDIR"Fed/test/bin/linux/x86/FEDTest.exe"


#make --silent --makefile Makefile _cleanall >/dev/null 2>&1;
#make --silent --makefile Makefile _all >/dev/null 2>&1;

# $1 get the number of uploads to be done...
if [ -n "$1" ]; then
    NB=$1;
else
    NB=7;
fi

partition=$2;

case "$2" in
    "TOB" ) coeffId=0;;
    "TIB" ) coeffId=1;;
    "TEC+"    ) coeffId=2;;
    "TEC-"    ) coeffId=3;;
esac

# loop on crate number
for j in `seq 1 $NB`; do 
    # loop on crate slot number
    for k in `seq 1 16`; do
	i=$(($(($k+$(($(($j-1))*16))))+$((120*$coeffId))));
	$EXECPROG -partition $partition -fedId $i -mode download &

    done;
done;


