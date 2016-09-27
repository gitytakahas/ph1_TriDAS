import sys,time,os, re
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
if not sockdir in sys.path: sys.path.append(sockdir)
from SimpleSocket import SimpleSocket
import ROOT
from ROOT import *

########################################################

# connect
print "connecting to pxfec,",
pxfec=SimpleSocket( 'localhost', 2000)
print " done"

print "connecting to digfed,",
digfed=SimpleSocket( 'localhost', 2006)
print " done"

digfed.send("initFitelS").readlines()
digfed.send("piggyS").readlines()
digfed.send("fiber 3").readlines()

pxfec.send("module 28")
pxfec.send("tbm tbmadelay 93")
pxfec.send("tbm tbmbdelay 93")
pxfec.send("tbm tbmplldelay 208")
time.sleep(0.1)
pxfec.send("roc 0:15")
pxfec.send("mask")
#pxfec.send("roc 3")
#pxfec.send("arm 10 10")
#pxfec.send("roc 6")
#pxfec.send("arm 10 10")
#pxfec.send("roc 0:15")
#pxfec.send("roc 15")
#pxfec.send("arm 10 10")
#pxfec.send("roc 11")
pxfec.send("arm 10 10")
time.sleep(0.1)
pxfec.send("tbm reset tbm")
pxfec.send("tbm reset roc")
time.sleep(0.1)

ntriggers = 1
histo = TH1F("histo","histo",255,0,255)
print "\n"
for va in range(50,100):

 print "Set Vana = ",va
 pxfec.send("Vana %i"%va)
 time.sleep(0.1)
 pxfec.send("ReadBack 12")
 time.sleep(0.1)
 pxfec.send("tbm reset tbm")
 pxfec.send("tbm reset roc")
 time.sleep(0.1)

 start_roc1_A = -1
 RB_roc1_A = []
 start_roc1_B = -1
 RB_roc1_B = []

 for n in range(32):

  strayout = digfed.send("getFromPiggy").readlines()
  sleep(0.1)
  isA = True
  for s in strayout:

   #if "due to bit flip?" in s:
   # print s
   # continue

   roc  = ("ROC#" in s)
   roc1 = ("ROC#1 " in s)

   if roc and isA:

    try:
     #print s
     roch = int(s.split(" ")[0])
     readback = roch&0x7
    except ValueError:
     print "Missed a trigger?"
     continue

    if roc1:
     #print "trigger ",n+1," roc #1 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc1_A == -1 and start == 1:
      start_roc1_A = n+1
     if start_roc1_A != -1 and len(RB_roc1_A)<17:
      RB_roc1_A.append(readback&0x1)
     isA = False



 print "ROC#1 start ",start_roc1_A," readback ",
 for r in range(1,len(RB_roc1_A)): print RB_roc1_A[r],
 iana = 0
 #for r in range(9,len(RB_roc1_A)):
 # print RB_roc1_A[r],
 #print "\n"
 #for r in range(9,len(RB_roc1_A)):
 # print (RB_roc1_A[r]<<(16-r)),
 #print "\n"
 for r in range(9,len(RB_roc1_A)):
  iana+=(RB_roc1_A[r]<<(16-r))
 print "FOUND iana = ",iana,"\n"
 histo.Fill(va,iana*0.25)

canv = TCanvas()
canv.cd()
histo.Draw("HIST")
histo.SaveAs("current.root")
canv.SaveAs("current.pdf")
time.sleep(1000)
