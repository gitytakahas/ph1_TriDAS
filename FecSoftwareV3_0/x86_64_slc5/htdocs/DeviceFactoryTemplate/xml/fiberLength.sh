#!/bin/sh
read z
while [ -n "$z" ]; do
  tagB=`echo $z | awk '{ print $1 }'`
  dcu=`echo $z | awk '{ print $2 }'`
  detid=`echo $z | awk '{ print $3 }'`
  fl=`echo $z | awk '{ print $4 }'`
  an=`echo $z | awk '{ print $5 }'`
  tage=`echo $z | awk '{ print $6 }'`

  if [ "$tagB" = "<DCUINFO" ]; then
    name=`echo $fl | cut -d\" -f 1` 
    value=`echo $fl | cut -d\" -f 2` 
    flr=`echo $value | awk '{ printf "%.2f", ($1*100); }'`
#  echo $flr
    echo "$tagB $dcu $detid $name=\"$flr\" $an $tage"
  else
    echo $z
  fi
  read z
done