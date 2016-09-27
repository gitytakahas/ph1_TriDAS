import sys,time,os, re
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
if not sockdir in sys.path: sys.path.append(sockdir)
from SimpleSocket import SimpleSocket
from SystemTests import SECTOR, GROUP, TestRedundancy, TestTriggerStatusFED, tDOH
from Logger import Logger


########################################################


# configuration
fechost = 'localhost'      
fecport =  2001  


fedslot =  99
fed=0                      # not used
pxfec=2000                 
caen=0                     # not used

########################################################


# connect

print "connecting to ccu,",fechost, fecport
ccu=SimpleSocket( fechost, fecport)
print " done"

#print "connecting to pxfec,",
#pxfec=SimpleSocket( 'localhost', 2000)
#print " done"

log=Logger()

# choose an arbitray sector in that shell and get ring and slot from the server
#s=SECTOR("-6P",fed,17,ccu,pxfec,caen)
s=GROUP(fed, 6, ccu, pxfec, caen, "+2PL12", 0x11, log)
print "FED slot ",s.fedslot

verbose = True

#ccu.send("cratereset").readlines()
#ccu.send("scanccu").readlines()
#ccu.send("piareset all").readlines()

#for l in ccu.send("scanccu").readlines():
#    if (verbose): print l

ccu.send("fec 12").readlines()
ccu.send("ring 0x8").readlines()
ccu.send("ccu 0x7b").readlines()


ccu.send("channel 0x11").readlines()
for i in range(1,10):
    print i
    for l in ccu.send("poh7 read").readlines():
        print l
    sleep(1)
    for l in ccu.send("poh7 setall 1 1 1 1 2 2 2 2").readlines():
        print l
    sleep(1)
#ccu.send("delay25 init").readlines()
#ccu.send("pll reset").readlines()
#ccu.send("pll init").readlines()
#ccu.send("doh init").readlines()
#ccu.send("poh1 init").readlines()
#ccu.send("poh2 init").readlines()
#ccu.send("poh3 init").readlines()
#ccu.send("poh4 init").readlines()
#ccu.send("poh5 init").readlines()
#ccu.send("poh6 init").readlines()
#ccu.send("poh7 init").readlines()
#ccu.send("doh setall 3 3 3 40 40 40").readlines()
#ccu.send("poh1 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh2 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh3 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh4 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh5 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh6 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh7 setall 3 3 3 3 40 40 40 40").readlines()

#ccu.send("channel 0x13").readlines()
#ccu.send("delay25 init").readlines()
#ccu.send("pll reset").readlines()
#ccu.send("pll init").readlines()
#ccu.send("doh init").readlines()
#ccu.send("poh1 init").readlines()
#ccu.send("poh2 init").readlines()
#ccu.send("poh3 init").readlines()
#ccu.send("poh4 init").readlines()
#ccu.send("poh5 init").readlines()
#ccu.send("poh6 init").readlines()
#ccu.send("poh7 init").readlines()
#ccu.send("doh setall 3 3 3 40 40 40").readlines()
#ccu.send("poh1 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh2 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh3 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh4 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh5 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh6 setall 3 3 3 3 40 40 40 40").readlines()
#ccu.send("poh7 setall 3 3 3 3 40 40 40 40").readlines()


ccu.close()
#pxfec.close()


