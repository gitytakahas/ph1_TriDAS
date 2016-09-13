#!/bin/bash
if [ -z "$1" ]; then
  if [ -z "$CONFDB" ]; then
    echo "Please set the environment variable CONFDB with the correct account"
  fi
else
  export CONFDB=$1
fi

if [ -n "$2" ]; then
  port=$2
else
  port=15000
fi

if [ -n "$3" ]; then
  level=$3
else
  level=WARN
fi

echo
echo
echo
echo '#######################################################################'
echo 
echo '    'http://`hostname`:$port/urn:xdaq-application:lid=15/
echo
echo '#######################################################################'
echo
echo
echo

filename=~/.tmp/tkdb.xml
mkdir -p ~/.tmp
rm -f $filename
cat > $filename <<EOF
<xc:Partition xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"
    xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

<xc:Context url="http://`hostname -i`:$port">

<!-- ************************************************************************************ -->
<!--                      TkConfigurationDb  application                                    -->
<!-- ************************************************************************************ -->
  <!-- Declare a TkConfigurationDb application -->
  <xc:Application class="TkConfigurationDb" id="15" instance="0" network="local">
  <properties xmlns="urn:xdaq-application:TkConfigurationDb" xsi:type="soapenc:Struct">
	<!-- DiagSystem Parameters-->
	<UseDiagSystem xsi:type="xsd:string">off</UseDiagSystem>  <!-- off / default / tuned / oldway -->
	<DiagSystemSettings xsi:type="xsd:string">/path/to/config/file/if/used</DiagSystemSettings>

  </properties>                     
  </xc:Application>

<!-- ************************************************************************************ -->
<!--                       Needed modules                                                 -->
<!-- ************************************************************************************ -->
  <xc:Module>/opt/trackerDAQ/lib/libTkConfigurationDb.so</xc:Module>
</xc:Context>
</xc:Partition>
EOF


if [ -n "$CONFDB" ]; then
  echo "Using the database account $CONFDB"
  echo "Using port $port and level \"$level\""
  if [ -z "$filename" ]; then
    $XDAQ_ROOT/bin/xdaq.exe  -p $port  -l $level
  else
    echo $XDAQ_ROOT/bin/xdaq.exe 
    $XDAQ_ROOT/bin/xdaq.exe  -p $port  -l $level -c $filename
  fi
fi


