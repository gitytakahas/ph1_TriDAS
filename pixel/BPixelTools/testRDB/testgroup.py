import sys,time,os, re, ROOT
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
if not sockdir in sys.path: sys.path.append(sockdir)
from SimpleSocket import SimpleSocket
from SystemTests import GROUP, SECTOR, TestRedundancy, TestTriggerStatusFED, tDOH, AOH, DELAY25, PLL, MODULE, TestDCDCEnable, DCDCDisable
from Logger import Logger
from ROOT import *
from array import array
from contextlib import contextmanager
from pohgraph import suppress_stdout,graph,getFilename,showRMSPOH
hList=[None]
gCanvases=[]
########################################################
# with suppress_stdout(): quiet mode for every function, just use this

########################################################

# configuration
fechost = 'localhost'      
pxfechost = 'localhost' 
caenhost ='localhost'
fedhost = 'localhost'   
fecport =  2001  
fedport= 2004              
pxfecport= 2000              
caenport= 2005              
caenslot =4  
########################################################
# connect start
########################################################
print "connecting to ccu,",fechost, fecport
ccu=SimpleSocket( fechost, fecport)
print " done"
print "connecting to pxfec,",pxfechost, pxfecport
pxfec=0
print " done"
print "connecting to caen,",caenhost, caenport
caen=0
print " done"
print "connecting to fed,",fedhost, fedport
fed=0
print " done"
########################################################
# connect end
########################################################
log=Logger()
name= "+6P"
fecslot=21
fecring=8
sector= SECTOR(name,fed,ccu,pxfec,caen,log)
group=[]
group1=GROUP(fed, ccu, pxfec, caen,name + "L12" ,0x11,log)
group2=GROUP(fed, ccu, pxfec, caen, name + 'L34', 0x13,log)
group.append(group1)
group.append(group2)
########################################################
# in real life we have this layout:
# sector.pohlist= range(1,9)+ range(14,15)
sector.pohlist= range(2,3)+range(5,7)
sector.poh[2].fedchannels=[None,5,6,7,8]
sector.poh[5].fedchannels=[None,1,2,3,4]
sector.poh[6].fedchannels=[None,9,10,11,12]
########################################################
# fed channel to bundle to poh map is stored in:
# pohfile=open('pohfiber.txt', 'r')
 

ccu.send("sector " + str(sector.name)).readlines()
ccu.send("reset").readlines()
ccu.send("piareset all").readlines()

###############################################################################
ccu.send("power all").readlines()
ccu.send("reset").readlines()

print "setting poh bias and gain"

   

print ccu.send("group " + sector.poh[2].group).readlines()
    #ccu.send("reset").readlines()
    
    #print sector.poh[k].name +" setall 3 3 3 3 40 40 40 40"
    #print ccu.send("poh2 setall 3 3 3 3 40 40 40 40").readlines()
print ccu.send("pll read").readlines()
    #print ccu.send("poh2 read").readlines()
  
######################################################################
ccu.close()
log.printLog()
log.saveLog("log/testCCU")
