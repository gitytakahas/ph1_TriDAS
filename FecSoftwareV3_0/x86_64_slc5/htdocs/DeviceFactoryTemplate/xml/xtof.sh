#!/bin/sh
read z
while [ -n "$z" ]; do
  tagB=`echo $z | awk '{ print $1 }'`
  dcu=`echo $z | awk '{ print $2 }' | cut -d\" -f2`
  detid=`echo $z | awk '{ print $3 }' | cut -d\" -f2`
  fl=`echo $z | awk '{ print $4 }' | cut -d\" -f2`
  an=`echo $z | awk '{ print $5 }'`
  tage=`echo $z | awk '{ print $6 }'`

  if [ "$tagB" = "<DCUINFO" ]; then
    echo "$dcu $detid $fl"
  else
    echo $z
  fi
  read z
done