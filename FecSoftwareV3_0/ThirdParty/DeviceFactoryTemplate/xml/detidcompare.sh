#!/bin/sh
if [ -z "$1" -o -z "$2" ]; then
  echo "Error in usage: $0 <FILE1> <FILE2>: compare the DET ID for the same DCU ID and the number of APVS"
  exit
fi

fiberLength=0
if [ "$3" = "-fiberLength" ]; then
  fiberLength=1
fi

numberofdcu1=`grep DCUINFO $1 | wc -l`
numberofdcu2=`grep DCUINFO $2 | wc -l`
echo "Found $numberofdcu1 DCUs in file $1"
echo "Found $numberofdcu2 DCUs in file $2"
if [ $numberofdcu1 -lt $numberofdcu2 ]; then
  file1=$2
  file2=$1
else
  file1=$1
  file2=$2
fi
echo "Compare the file $file1 and $file2 in this order"
echo -n "Press enter to continue"
read y

for x in `grep DCUINFO $file1 | awk '{ print $2 }'`; do
  a=`grep $x $file2`
  if [ -z "$a" ]; then
    detid1=`grep $x $file1 | awk '{ print $3 }'`
    echo "The $x $detid1 is missing in file $file2"
  else
    detid1=`grep $x $file1 | awk '{ print $3 }'`
    detid2=`grep $x $file2 | awk '{ print $3 }'`
    if [ -z "$detid1" -o -z "$detid2" -o "$detid1" != "$detid2" ]; then
      echo "Bad det id in file: $x: $detid1 / $detid2"
    else
      numberapv1=`grep $x $file1 | awk '{ print $5 }'`
      numberapv2=`grep $x $file2 | awk '{ print $5 }'`
      if [ -z "$numberapv1" -o -z "$numberapv2" -o "$numberapv1" != "$numberapv2" ]; then
        echo "Bad APV mumber for $x, $detid, $numberapv1 / $numberapv2"
#      else
#        echo "$x ok"
      fi
      if [ "$fiberLength" = "1" ]; then
        fiberLength1=`grep $x $file1 | awk '{ print $4 }'`
	fiberLength2=`grep $x $file2 | awk '{ print $4 }'`
	if [ "$fiberLength1" != "$fiberLength2" ]; then
          echo "Fiber length different for $x, $detid, $fiberLength1, $fiberLength2"
        fi
      fi
    fi
  fi
done
