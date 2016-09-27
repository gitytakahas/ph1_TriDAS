########################################################
# SUPPLY TUBE DCDC TEST PROGRAM
########################################################
import sys,time,os, re, ROOT
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
from SimpleSocket import SimpleSocket
from SystemTests import GROUP, SECTOR, TestRedundancy, TestTriggerStatusFED, tDOH, AOH, DELAY25, PLL, MODULE, TestDCDCEnable, DCDCDisable, POH
from Logger import Logger
from ROOT import *
from array import array
from contextlib import contextmanager
fechost = 'localhost'      
pxfechost = 'localhost' 
caenhost ='localhost'
fecport =  2001            
pxfecport= 2000              
caenport= 2005              
caenslot =4  
fed= "dummyfed"
ccu=SimpleSocket( fechost, fecport)
pxfec=SimpleSocket(fechost, pxfecport)
caen=SimpleSocket(caenhost,caenport)
########################################################
########################################################
c1=TCanvas('c1', 'c1', 200, 10, 800, 800)
########################################################
redundancytest=  False # True
resettest=  False
inittest=  False # True
dcdctest= True #True
vanatest= False
dcdcianatest=True
########################################################
########################################################
########################################################
# print "sector and groups names definition"
log=Logger()
name= "+6P"
ccu.send("sector" + name   ).readlines()
out=( ccu.send( "which fec").readlines())
fecslot =int(out[1][-2:])
out=(ccu.send("which ring").readlines())
fecring =int(out[1][-2:])
sector= SECTOR(name,fed,ccu,pxfec,caen,log)
########################################################
########################################################
ccu.send("sector " + str(sector.name)).readlines()
# ccu.send("reset").readlines()
# ccu.send("piareset all").readlines()
########################################################
# tag="FEC %d ring 0x%s :"%(fecslot,fecring)
# fecringfound=False
# fecfound=False
# status=""
# for l in ccu.send("mapccu").readlines():
#     if l.strip().startswith("FEC %d"%(fecslot)):
#         fecfound=True
#     if l.strip().startswith(tag):
#         fecringfound=True
#         status=l.strip().split(":")[1].strip()
# if not fecfound:
#     log.error("No such FEC " +str(fecslot))
#     sys.exit(1)
# if not fecringfound:
#     log.error("ring/mfec not found "+tag)
#     sys.exit(1)
    
# if status=="no CCU found":
#     log.error("no CCU found "+tag)
#     # print "verify ccu power"
#     while True:
#         yesno=raw_input("continue anyway (y/n) ")
#         if yesno=='n': sys.exit(1)
#         if yesno=='y': break

# else:
#     log.ok("CCUs found")    

########################################################
#print "start test"
########################################################
if redundancytest==True:
    TestRedundancy(ccu,log=log) 
########################################################
if resettest==True:
    sector.TestPIAResetDOH()
    sector.VerifySectorDevicesProgramming()
    for i in sector.group:
        ccu.send("group " + i.name).readlines()
        sector.ResetAllandInitAll() 
   
########################################################
if inittest==True:
    sector.init_tube()
########################################################
if dcdctest==True:
    print "DCDC power cycle test,MODULES MUST BE DISCONNECTED, "
    print "PLUG CAEN "
    rep = raw_input(' when ready hit return: ')
    print "test sector powerline"
    ccu.send("piareset all").readlines()
    sector.poweroff()
    message=ccu.send("get").readline()
    
    for s in message:
        c1=re.search("CCU_7c",s)
        if c1:
            print s
    sector.poweron()
    print "test dcdc pair enable/disable"
    TestDCDCEnable(ccu ,log)
    sector.poweron()
    
    # sector.TestCCUandI2CConnection()
########################################################
if vanatest is True:
    pxfec.send("exec data/layer3.ini")

    kappa=sector.GetSDALayerbyLayer("0:31",1)
    kappa.SaveAs("VANATEST.ps")
    #rep = raw_input(' wait for master when ready hit return: ')
      
    
########################################################
if dcdcianatest is True:
    name="c2"
    #c2.Divide(8,4)
    c2=TCanvas(name, name, 200, 10, 800, 800)
    c2.Divide(8,4)
    c2.cd()
    pxfec.send("exec data/layer3.ini")
    #def dcdcianavsiana(self,module,sda=50,which_layer,verbose=True):
    kappa=[]
    for module in range(1,32):
        c2.cd(module)
        kappa.append(sector.dcdcianavsiana(str(module),2,50))
        kappa[-1].SetLineColor((module+1)%6+1)
        kappa[-1].Draw("same")
        #c2.BuildLegend()
        c2.Update()
        #kappa.SaveAs("VANAIANAL2TEST.ps")
    rep = raw_input(' wait for master when ready hit return: ')
