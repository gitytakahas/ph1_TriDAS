import sys,time,os, re
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
if not sockdir in sys.path: sys.path.append(sockdir)
from SimpleSocket import SimpleSocket


########################################################


# configuration
fechost = 'localhost'      
fecport =  2001  


fedslot =  99
fed=0                      # not used
pxfec=2000                 
caen=0                     # not used
fedport =  2006
fecport =  2001  
########################################################


# connect
print "connecting to pxfec,",
pxfec=SimpleSocket( 'localhost', 2000)
print " done"# port

print "connecting to ccu,",
########################################################
ccu=SimpleSocket( 'localhost', fecport)
print " done"# port


pxfec.send("cn +6PL12").readlines()
pxfec.send("module 28").readlines()
pxfec.send("roc 0:15").readlines()
#ccu.send("channel 0x11").readlines()

for i in range(30,600000):

  print "Setting Delay=%i"%(i)
  #ccu.send("delay25 set d2 %i"%i).readlines()
  pxfec.send("Vana 0").readlines()
  #time.sleep(3)
  pxfec.send("Vana 255").readlines()  
  #time.sleep(3)
  pxfec.send("Vana 0").readlines()
  
