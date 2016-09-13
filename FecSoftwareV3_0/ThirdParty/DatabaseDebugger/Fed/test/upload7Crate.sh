#!/bin/bash
#echo "We are working on $CONFDB account..."

TIMEFORMAT="%E" 
ROOTDIR="/exports/xdaq/dev35/FecSoftwareV3_0/ThirdParty/DatabaseDebugger/"  
XMLFILEPATH=$ROOTDIR"xml/"
EXECPROG=$ROOTDIR"Fed/test/bin/linux/x86/FEDTest.exe"


#make --silent --makefile Makefile _cleanall >/dev/null 2>&1;
#make --silent --makefile Makefile _all >/dev/null 2>&1;


partition=$1;
version=$2;

case "$partition" in
    "TOB" ) coeffId=0;;
    "TIB" ) coeffId=1;;
    "TEC+"    ) coeffId=2;;
    "TEC-"    ) coeffId=3;;
esac

rm -f /tmp/testFed/newTestFed.xml$partition;
rm -f /tmp/testFed/newTestTtcrx.xml$partition;
rm -f /tmp/testFed/newTestVoltageMonitor.xml$partition;
rm -f /tmp/testFed/newTestFeFpga.xml$partition;
rm -f /tmp/testFed/newTestChannelPair.xml$partition;
rm -f /tmp/testFed/newTestChannel.xml$partition;
rm -f /tmp/testFed/newTestApvFed.xml$partition;
rm -f /tmp/testFed/newTestStrip.xml$partition;

echo '<ROWSET>' > /tmp/testFed/newTestFed.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestTtcrx.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestVoltageMonitor.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestFeFpga.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestChannelPair.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestChannel.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestApvFed.xml$partition;
echo '<ROWSET>' > /tmp/testFed/newTestStrip.xml$partition;

# loop on crate slot number
for j in `seq 1 7`; do
    for k in `seq 1 16`; do
	i=$(($(($k+$(($(($j-1))*16))))+$((120*$coeffId))));

	sed s/\<ID\>0/\<ID\>$i/g $XMLFILEPATH/newTestFed.xml >> /tmp/testFed/newTestFed.xml$partition;
	sed -i s/HARDWAREID\>0/HARDWAREID\>$i/g /tmp/testFed/newTestFed.xml$partition;
	sed -i s/CRATE\>0/CRATE\>$j/g /tmp/testFed/newTestFed.xml$partition;
	sed -i s/CRATESLOT\>0/CRATESLOT\>$k/g /tmp/testFed/newTestFed.xml$partition;
	    
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestTtcrx.xml >> /tmp/testFed/newTestTtcrx.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestVoltageMonitor.xml >> /tmp/testFed/newTestVoltageMonitor.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestFeFpga.xml >> /tmp/testFed/newTestFeFpga.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestChannelPair.xml >> /tmp/testFed/newTestChannelPair.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestChannel.xml >> /tmp/testFed/newTestChannel.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestApvFed.xml >> /tmp/testFed/newTestApvFed.xml$partition;
	sed s/\<IDFED\>0/\<IDFED\>$i/g $XMLFILEPATH/newTestStrip.xml >> /tmp/testFed/newTestStrip.xml$partition;
    done;
done;

echo '</ROWSET>' >> /tmp/testFed/newTestFed.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestTtcrx.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestVoltageMonitor.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestFeFpga.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestChannelPair.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestChannel.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestApvFed.xml$partition;
echo '</ROWSET>' >> /tmp/testFed/newTestStrip.xml$partition;

$EXECPROG -f /tmp/testFed -partition $partition -mode upload -versionUpdate $version
echo $?

