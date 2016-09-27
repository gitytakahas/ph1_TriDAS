import sys,time,os, re
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
if not sockdir in sys.path: sys.path.append(sockdir)
from SimpleSocket import SimpleSocket

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
digfed.send("fiber 1").readlines()

pxfec.send("module 28")
#pxfec.send("tbm tbmadelay 101")
#pxfec.send("tbm tbmbdelay 101")
#pxfec.send("tbm tbmplldelay 252")
time.sleep(0.1)
pxfec.send("roc 0:15")
pxfec.send("ReadBack 12")
pxfec.send("Vana 100")
#pxfec.send("mask")
#pxfec.send("roc 3")
#pxfec.send("arm 10 10")
#pxfec.send("roc 6")
#pxfec.send("arm 10 10")
#pxfec.send("roc 0:15")
#pxfec.send("roc 15")
#pxfec.send("arm 10 10")
#pxfec.send("roc 11")
#pxfec.send("arm 10 10")
time.sleep(0.1)
#pxfec.send("tbm reset tbm")
#pxfec.send("tbm reset roc")
time.sleep(0.1)

ntriggers = 5

for t in range(ntriggers):

 start_roc1_A = -1
 start_roc2_A = -1
 start_roc3_A = -1
 start_roc4_A = -1
 start_roc5_A = -1
 start_roc6_A = -1
 start_roc7_A = -1
 start_roc8_A = -1
 RB_roc1_A = []
 RB_roc2_A = []
 RB_roc3_A = []
 RB_roc4_A = []
 RB_roc5_A = []
 RB_roc6_A = []
 RB_roc7_A = []
 RB_roc8_A = []
 start_roc1_B = -1
 start_roc2_B = -1
 start_roc3_B = -1
 start_roc4_B = -1
 start_roc5_B = -1
 start_roc6_B = -1
 start_roc7_B = -1
 start_roc8_B = -1
 RB_roc1_B = []
 RB_roc2_B = []
 RB_roc3_B = []
 RB_roc4_B = []
 RB_roc5_B = []
 RB_roc6_B = []
 RB_roc7_B = []
 RB_roc8_B = []

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
   roc2 = ("ROC#2 " in s)
   roc3 = ("ROC#3 " in s)
   roc4 = ("ROC#4 " in s)
   roc5 = ("ROC#5 " in s)
   roc6 = ("ROC#6 " in s)
   roc7 = ("ROC#7 " in s)
   roc8 = ("ROC#8 " in s)

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

    if roc2:
     #print "trigger ",n+1," roc #2 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc2_A == -1 and start == 1:
      start_roc2_A = n+1
     if start_roc2_A != -1 and len(RB_roc2_A)<17:
      RB_roc2_A.append(readback&0x1)

    if roc3:
     #print "trigger ",n+1," roc #3 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc3_A == -1 and start == 1:
      start_roc3_A = n+1
     if start_roc3_A != -1 and len(RB_roc3_A)<17:
      RB_roc3_A.append(readback&0x1)

    if roc4:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc4_A == -1 and start == 1:
      start_roc4_A = n+1
     if start_roc4_A != -1 and len(RB_roc4_A)<17:
      RB_roc4_A.append(readback&0x1)
     isA = False

    '''
    if roc5:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc5_A == -1 and start == 1:
      start_roc5_A = n+1
     if start_roc5_A != -1 and len(RB_roc5_A)<17:
      RB_roc5_A.append(readback&0x1)

    if roc6:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc6_A == -1 and start == 1:
      start_roc6_A = n+1
     if start_roc6_A != -1 and len(RB_roc6_A)<17:
      RB_roc6_A.append(readback&0x1)

    if roc7:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc7_A == -1 and start == 1:
      start_roc7_A = n+1
     if start_roc7_A != -1 and len(RB_roc7_A)<17:
      RB_roc7_A.append(readback&0x1)

    if roc8:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc8_A == -1 and start == 1:
      start_roc8_A = n+1
     if start_roc8_A != -1 and len(RB_roc8_A)<17:
      RB_roc8_A.append(readback&0x1)
     isA = False
    '''

   if roc and not isA:

    roch = int(s.split(" ")[0])
    readback = roch&0x7

    if roc1:
     #print "TBM-B trigger ",n+1," roc #1 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc1_B == -1 and start == 1:
      start_roc1_B = n+1
     if start_roc1_B != -1 and len(RB_roc1_B)<17:
      RB_roc1_B.append(readback&0x1)

    if roc2:
     #print "TBM-B trigger ",n+1," roc #2 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc2_B == -1 and start == 1:
      start_roc2_B = n+1
     if start_roc2_B != -1 and len(RB_roc2_B)<17:
      RB_roc2_B.append(readback&0x1)

    if roc3:
     #print "TBM-B trigger ",n+1," roc #3 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc3_B == -1 and start == 1:
      start_roc3_B = n+1
     if start_roc3_B != -1 and len(RB_roc3_B)<17:
      RB_roc3_B.append(readback&0x1)

    if roc4:
     #print "TBM-B trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc4_B == -1 and start == 1:
      start_roc4_B = n+1
     if start_roc4_B != -1 and len(RB_roc4_B)<17:
      RB_roc4_B.append(readback&0x1)

    '''
    if roc5:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc5_B == -1 and start == 1:
      start_roc5_B = n+1
     if start_roc5_B != -1 and len(RB_roc5_B)<17:
      RB_roc5_B.append(readback&0x1)

    if roc6:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc6_B == -1 and start == 1:
      start_roc6_B = n+1
     if start_roc6_B != -1 and len(RB_roc6_B)<17:
      RB_roc6_B.append(readback&0x1)

    if roc7:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc7_B == -1 and start == 1:
      start_roc7_B = n+1
     if start_roc7_B != -1 and len(RB_roc7_B)<17:
      RB_roc7_B.append(readback&0x1)

    if roc8:
     #print "trigger ",n+1," roc #4 header ",roch," readback ",(readback&0x4)>>2,(readback&0x2)>>1,readback&0x1
     start = (readback&0x2)>>1
     if start_roc8_B == -1 and start == 1:
      start_roc8_B = n+1
     if start_roc8_B != -1 and len(RB_roc8_B)<17:
      RB_roc8_B.append(readback&0x1)
    '''

 print "************* TRIGGER NUMBER ", t+1,"****************"
 print "TBMA\n"    
 print "ROC#1 start ",start_roc1_A," readback ",
 for r in range(1,len(RB_roc1_A)): print RB_roc1_A[r],
 print "\n"
 print "ROC#2 start ",start_roc2_A," readback ",
 for r in range(1,len(RB_roc2_A)): print RB_roc2_A[r],
 print "\n"
 print "ROC#3 start ",start_roc3_A," readback ",
 for r in range(1,len(RB_roc3_A)): print RB_roc3_A[r],
 print "\n"
 print "ROC#4 start ",start_roc4_A," readback ",
 for r in range(1,len(RB_roc4_A)): print RB_roc4_A[r],
 print "\n"
 print "ROC#5 start ",start_roc5_A," readback ",
 for r in range(1,len(RB_roc5_A)): print RB_roc5_A[r],
 print "\n"
 print "ROC#6 start ",start_roc6_A," readback ",
 for r in range(1,len(RB_roc6_A)): print RB_roc6_A[r],
 print "\n"
 print "ROC#7 start ",start_roc7_A," readback ",
 for r in range(1,len(RB_roc7_A)): print RB_roc7_A[r],
 print "\n"
 print "ROC#8 start ",start_roc8_A," readback ",
 for r in range(1,len(RB_roc8_A)): print RB_roc8_A[r],
 print "\n"

 print "TBMB\n" 
 print "ROC#1 start ",start_roc1_B," readback ",
 for r in range(1,len(RB_roc1_B)): print RB_roc1_B[r],
 print "\n"
 print "ROC#2 start ",start_roc2_B," readback ",
 for r in range(1,len(RB_roc2_B)): print RB_roc2_B[r],
 print "\n"
 print "ROC#3 start ",start_roc3_B," readback ",
 for r in range(1,len(RB_roc3_B)): print RB_roc3_B[r],
 print "\n"
 print "ROC#4 start ",start_roc4_B," readback ",
 for r in range(1,len(RB_roc4_B)): print RB_roc4_B[r],
 print "\n"
 print "ROC#5 start ",start_roc5_B," readback ",
 for r in range(1,len(RB_roc5_B)): print RB_roc5_B[r],
 print "\n"
 print "ROC#6 start ",start_roc6_B," readback ",
 for r in range(1,len(RB_roc6_B)): print RB_roc6_B[r],
 print "\n"
 print "ROC#7 start ",start_roc7_B," readback ",
 for r in range(1,len(RB_roc7_B)): print RB_roc7_B[r],
 print "\n"
 print "ROC#8 start ",start_roc8_B," readback ",
 for r in range(1,len(RB_roc8_B)): print RB_roc8_B[r],
 print "\n"

