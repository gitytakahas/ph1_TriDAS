#!/bin/sh
echo -n "Do you want to remove the previous test files (y/n): "
read y
if [ "$y" = "y" -o "$y" = "Y" ]; then
  rm -f /tmp/Error*
  rm -f /tmp/FecTest*
fi

loop=$1
if [ -d /exports/xdaq/development ]; then
  source /exports/xdaq/development/FecSoftwareV3_0/cvs_profile
  echo "Reseting the crate" 
  /exports/xdaq/development/FecSoftwareV3_0/bin/ProgramTest.exe -vmesbs -nogetchar -crateReset 1>/dev/null
  if [ -z "$loop" ]; then
    echo -n "Please enter the number of loop you want to do (-1 = forever): "
    read loop
  fi
  maDate=`date +%d%m%y_%H%M`
  echo "----------- Dump the errors in file /tmp/Error$maDate.txt"
  /exports/xdaq/development/FecSoftwareV3_0/bin/ProgramTest.exe -vmesbs -nogetchar -loop $loop-100000 -fectest 2> /tmp/Error$maDate.txt
  echo "Errors has been dump in /tmp/Error$maDate.txt"
  if [ -f /tmp/FecTest$maDate.txt ]; then 
    fileName=/tmp/FecTest$maDate.txt
  else 
    b=`date +%M`
    b=`expr $b + 1`
    maDate=`date +%d%m%y_%H`
    maDate=$maDate$b
    fileName=/tmp/FecTest$maDate.txt
  fi
  if [ -f $fileName ]; then
    echo "Result of tests has beem dump in $fileName"
    b=`wc -l $fileName | awk '{ print $1; }'`
    if [ $b -gt 1 ]; then
      grep "ERROR" $fileName
    else
      echo "FATAL: no FEC has been found in the crate: driver problem ?"
    fi
  else
    echo "FATAL: Did not find the result of the basic tests, please check in /tmp/FecList*.txt"
  fi
  if [ -f /tmp/Error$maDate.txt ]; then
    g=`grep "No FEC in the crates" /tmp/Error$maDate.txt`
    if [ -n "$g" ]; then
      echo "FATAL: no FEC has been found in the crate: driver problem ?"
    else
      b=`wc -l /tmp/Error$maDate.txt | awk '{ print $1; }'`
      if [ $b -gt 1 ]; then  
        b=`expr $b - 1`
        echo "ERROR: $b errors occurs on FIFO/loop back tests"
      else
        echo "No errors on FIFO/loop back tests"
      fi
    fi
  else
    echo "FATAL: Did not find the error file: driver problem ?"
  fi
else
  echo "Please mount the directory from NFS: mount -t nfs cmstkint09:/exports/xdaq /exports/xdaq"
fi
