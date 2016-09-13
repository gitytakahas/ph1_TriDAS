#!/bin/bash


partition=$1
#      /*
#       *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
#       *   - nextMajorVersion.0 if versionUpdate = 1
#       *   - currentMajorVersion.0 if versionUpdate = 2
#       *   - currentMajorVersion.currentMinorVersion if versionUpdate = 3
#       */

versionUpdate=3;

# loop on crate number
for i in `seq 1 7`; do 
    # loop on crate slot number
    ./uploadCrate.sh $i $partition $versionUpdate &
    versionUpdate=3;
done;


