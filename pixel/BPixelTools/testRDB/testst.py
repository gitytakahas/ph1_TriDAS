########################################################
# SUPPLY TUBE TEST PROGRAM
########################################################
import sys,time,os, re, ROOT
from datetime import date
from time import sleep
sockdir="/home/cmspixel/TriDAS/pixel/BPixelTools/tools/python"
timer=[]
start_time=time.time()
timer.append("start")
timer.append(time.time()-start_time)
if not sockdir in sys.path: sys.path.append(sockdir)
###########################################################################################################################################################
from SimpleSocket import SimpleSocket
from SystemTests import *
from includest import *
from Logger import Logger
from ROOT import *
from array import array
from contextlib import contextmanager
from pohgraph import suppress_stdout,graph,getFilename,showRMSPOH
###########################################################################################################################################################
today=time.asctime(time.localtime(time.time()))
print today
def initmod():
    pxfec.send("exec data/layer3.ini").readlines()
def realmean(hist,axis):
    rangex=hist.GetNbinsX()
    rangey=hist.GetNbinsY()
    nx=0
    ny=0
    meanvaluex=0
    meanvaluey=0
    i=0
    j=0
    if axis== 1:
        for x in range(0,rangex):
            for y in range(0,rangey):
                value=0
                if y>0 and y<rangey and x>0 and x<rangex:
                    for k in range(-1,1):
                        for j in range(-1,1):
                            near=hist.GetBinContent(x+i,y+j)
                            if near>0:
                                value=hist.GetBinContent(x,y)
                if value==1:
                    ny=ny+1
                    meanvaluey=meanvaluey+hist.GetYaxis().GetBinCenter(y)
        return meanvaluey/ny
    if axis== 0:
        for x in range(0,rangex):
            for y in range(0,rangey):
                value=0
                if y>0 and y<rangey and x>0 and x<rangex:
                    for k in range(-1,1):
                        for j in range(-1,1):
                            near=hist.GetBinContent(x+i,y+j)
                            if near>0:
                                value=hist.GetBinContent(x,y)
                if value==1:
                    nx=nx+1
                    meanvaluex=meanvaluex+hist.GetXaxis().GetBinCenter(x)
        return meanvaluex/nx
    else:
        return "axis should be 0 for x and 1 for y"
########################################################
hList=[None]
gCanvases=[]
########################################################
timer.append("Connection to simplesocket")
timer.append(time.time()-start_time)
########################################################
#log=Logger()

ccu.send("sector" + name   ).readlines()
out=( ccu.send( "which fec").readlines())
fecslot =int(out[1][-2:])
out=(ccu.send("which ring").readlines())
fecring =int(out[1][-2:])
########################################################
filename=getFilename("data/may25/2016%s.pdf"%(sector.name))
rootfile=TFile(os.path.splitext(filename)[0]+".root","RECREATE")
copen=TCanvas('copen', 'copen', 200, 10, 800, 800)
copen.Print(filename+"(")
########################################################
########################################################
if only23==True:
    module=[23]
    mbundle=[1]
    d1=[50]
    d2=[51]
    inputo=[0]
    fiber=[[5,6]]
    sector.pohlist= range(2,3)
    sector.poh[2] =POH(fed, ccu, name+"L12","poh2", 1,[None, 5, 6, 7, 8] ,log)
########################################################
########################################################
if only15==True:
    module=[15]
    mbundle=[1]
    d1=[50]
    d2=[51]
    inputo=[0]
    fiber=[[11,12]]
    sector.pohlist= range(6,7)
    sector.poh[6] =POH(fed, ccu, name+"L12","poh4", 1,[None, 1, 2, 3, 4] ,log)
########################################################
########################################################
if M15and23==True:
    module=[15,23]
    mbundle=[1,1]
    d1=[50,50]
    d2=[51,51]
    imputo=[0,0]
    fiber=[[11,12],[5,6]]
    sector.pohlist= range(6,7)+range(2,3)
    sector.poh[6] =POH(fed, ccu, name+"L12","poh4", 1,[None, 1, 2, 3, 4] ,log)
    sector.poh[2] =POH(fed, ccu, name+"L12","poh2", 1,[None, 5, 6, 7, 8] ,log)
########################################################
########################################################
distinct_list = []
for each in mbundle:
    if each not in distinct_list:
            distinct_list.append(each)
print     distinct_list
distinct_list.sort()
print     distinct_list
sublist_m=[]
sublist_m_fiber=[]
for i in  distinct_list:
    sublist_m.append([])
    sublist_m_fiber.append([])
for i in range(0,len(module)):
    for k in range(0,len(distinct_list)):
               if mbundle[i]-1 ==k:
                   sublist_m[k].append(module[i])
                   sublist_m_fiber[k].append(fiber[i])
print  sublist_m
print  sublist_m_fiber
########################################################
sublist=  sector.get_bundle_list()   
print "sublist of poh divided by bundle!"
print sublist
########################################################
ccu.send("sector " + str(sector.name)).readlines()
ccu.send("reset").readlines()
ccu.send("piareset all").readlines()   
timer.append("CCU mapping")
timer.append(time.time()-start_time)
########################################################
#print "start test"
########################################################
########################################################
if redundancytest==True:
    testname="Redundancy test"
    log.info(testname)
    TestRedundancy(ccu,log=log) 
    timer.append("Redundancy test done")
    timer.append(time.time()-start_time)
########################################################
########################################################
if resettest==True:
    testname="Reset test"
    log.info(testname)
    sector.TestPIAResetDOH()
    sector.VerifySectorDevicesProgramming()
    for i in sector.group:
        ccu.send("group " + i.name).readlines()
        sector.ResetAllandInitAll() 
    sleep(1)
    #sector.VerifyQPLL()  
    timer.append("Reset Test done")
    timer.append(time.time()-start_time)
########################################################
########################################################
if inittest==True:
    sleep(1)
    sector.init_tube()
    
if resetandverifynocrosstalk==True:
    sleep(1)
    print "sector.verifynocrosstalk()"
    sector.verifynocrosstalk()
########################################################
########################################################
if sendata is True:
    sector.init_tube()
    
    for i in range(0,999999):
        pxfec.send("cn +6PL34")
        sleep(1)
        pxfec.send("module 0:30 tbm tbmplldelay 100")

        pxfec.send("cn +6PL12")
        sleep(1)
        pxfec.send("module 0:30 tbm tbmplldelay 100")
        print "sending",i
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
if pohplusfechannel==True:
    sector.init_tube()
    testname="Fed channels test"
    log.info(testname)
    counter =0
    errorc=0
    canvas=[]
    canvasnoise=[]
    slope=TH1F("slope","slope",60,0,60)
    print "1) START TRIGGER"
    print "2) Power on mb and ccu, modules off "
    kadc=[]
    krms=[]
    sector.init_tube()
    for n in sublist:
        name="bundle_"+str(sector.poh[n[0]].bundle)
        canvas.append(TCanvas(name,name, 200, 10, 800, 800))
        name="bundle_noise"+str(sector.poh[n[0]].bundle)
        canvasnoise.append(TCanvas(name,name, 200, 10, 800, 800))
        print  "  Fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
        rep = raw_input(' when ready hit return: ')
        # kadc=[]
        # krms=[]
        for i in n:
            number=sector.poh[n[0]].bundle-1
            print number
            number=sublist.index(n)
            print number
            kadc.append([])
            krms.append([])
            print "poh",i
            pos=0
            for ch in sector.poh[i].fedchannels[1:]:
                out ="group "+sector.poh[i].group
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean,rms,name=response.split()
                out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name +" set b"+str(3-pos)+" 60"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean1,rms,name=response.split()
                if  float(mean1) - float(mean) < 1000: 
                    errorc=errorc+1
                    errorstring="poh "+str(i)+" fiber " +str(13-ch) +" failure max-min ADC "+ str(float(mean1)) +" "+ str(float(mean))
                    # print "fiber",13-ch ,"failure", float(mean1) , float(mean)
                    log.error(errorstring)
                else:
                    okstring= "poh "+str(i)+" fiber "+ str(13-ch )+" ok"
                    # print "fiber",13-ch ,"ok"
                    log.ok(okstring)
    
                pos=pos+1
        if errorc==0:
            print "ok"
        counter =0
        
        testname="POH bias scan"
        log.info(testname)
       ###################################
        color=[kBlue,kOrange,kGreen,kOrange+3,kGray,kWhite,kRed,kBlack,kYellow,kViolet,kPink,kAzure]
        
        mgslope =  TMultiGraph()
        mgslope.SetMaximum(1100.)
        mgnoise =  TMultiGraph()
        mgnoise.SetMaximum(10.)
        memory=[]
        print sector.poh[n[0]].bundle
        canvas[number].cd()
        for i in n:
            print "poh",i
            pos=-1
            for ch in sector.poh[i].fedchannels[1:]:
                number=sector.poh[n[0]].bundle-1
                number=sublist.index(n)
                pos=pos+1
                out ="group "+sector.poh[i].group
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                sector.ccu.send(out).readlines()
                kadc[number].append(TGraph())
                krms[number].append(TGraph())
                out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                sector.ccu.send(out).readlines()
                # k[-1].SetMarkerColor(color[ch-1])
                name= " Fb "+str(13-ch)
                kadc[number][-1].SetName(name)
                kadc[number][-1].SetMaximum(1100)
                kadc[number][-1].SetFillStyle(0);
                kadc[number][-1].SetFillColor(0);
                name= " Fb "+str(13-ch)
                krms[number][-1].SetName(name)
                krms[number][-1].SetFillStyle(0);
                krms[number][-1].SetFillColor(0);
                tempgr=TGraph()
                ngr=0
                
                fed.send("reset fifo").readlines()
                pitch=2
                biasrange=range(0,60,pitch)
                for bias in biasrange:
                    pb(bias,biasrange[-1])
                    out = sector.poh[i].name + " setall 3 3 3 3 "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
                    sector.ccu.send(out).readlines()
                    fed.send("reset fifo").readlines()
                    sleep(0.1)
                    inin=fed.query("rms " +str(ch))
                    channel,mean,rms,name=inin.split()
                    kadc[number][-1].SetPoint(bias/pitch,bias,float(mean))
                    if float(mean)>50 and float(mean) <1000: 
                        krms[number][-1].SetPoint(ngr,bias,float(rms))
                        tempgr.SetPoint(ngr,bias,float(mean))
                        ngr=ngr+1
                    
                
                kadc[number][-1].SetLineWidth(3)
                krms[number][-1].SetLineWidth(3)
                grfit= TF1("pol1","pol1",0,70)
                if tempgr.GetN()>2:
                    maxx=krms[number][-1].GetHistogram().GetMean(2)
                    tempgr.Fit(grfit,"q")
                    if grfit.GetParameter(1)>35.0  and maxx <5.0:
                        okstring="poh "+ str(i)  + " fiber " + str(13-ch) + "  slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, OK, NOISE OK"
                        log.ok(okstring)
                    else:
                        if grfit.GetParameter(1)<=35.0:
                            errorstring ="poh "+ str(i)  + " fiber " + str(13-ch) + " slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, LOW SLOPE"
                            log.error(errorstring) 
                        if maxx >=5.0:
                            errorstring ="poh "+ str(i)  + " fiber " + str(13-ch)+ "  slope: "+str(round(grfit.GetParameter(1),2)) + " ADC/bias, noise: "+str(round(maxx,2))+ " ADC, HIGH NOISE"
                            log.error(errorstring)
                        

                else:
                    errorstring="poh "+ str(i)  + " fiber " + str(13-ch) + " CHANNEL DOES NOT WORK"
                    log.error(errorstring)
                kadc[number][-1].SetLineColor(color[12-ch ])
                kadc[number][-1].SetMarkerStyle(20+ch)
                krms[number][-1].SetLineColor(color[12-ch ])
                krms[number][-1].SetMarkerStyle(20+ch)
                if tempgr.GetN()>2:
                    title=kadc[number][-1].GetName()+" : " +str(round(grfit.GetParameter(1),2))+ " ADC/bias"
                    kadc[number][-1].SetTitle(title)
                    maxx=krms[number][-1].GetHistogram().GetMaximum(); 
                    title=krms[number][-1].GetName()+" : " +str(round(maxx,2))+ " ADC"
                    krms[number][-1].SetTitle(title)
                    slope.Fill(grfit.GetParameter(1)*0.56,1)
                else:
                    title=kadc[number][-1].GetName()+" LOW SLOPE" 
                    kadc[number][-1].SetTitle(title)
                    title=krms[number][-1].GetName()+" LOW SLOPE" 
                    krms[number][-1].SetTitle(title)
                    slope.Fill(0,1)
                canvas[number].cd()
                mgslope.Add(kadc[number][-1],"ALP")
                kadc[number][-1].Write()
                mgslope.Draw("ALP same" )
                canvasnoise[number].cd()
                mgnoise.Add(krms[number][-1],"ALP")
                krms[number][-1].Write()
                mgnoise.Draw("ALP same" )
                nameprint="SLOPE Bundle "+str(sector.poh[n[0]].bundle)
                canvas[number].BuildLegend(0.7,0.1,0.9,0.7,nameprint)
                canvas[number].Update()
                nameprint="NOISE Bundle "+str(sector.poh[n[0]].bundle)
                canvasnoise[number].BuildLegend(0.1,0.3,0.4,0.9,nameprint)
                canvasnoise[number].Update()
               
        
        canvas[number].Print(filename,nameprint)
        canvas[number].Write()
        
        canvasnoise[number].Print(filename,nameprint)
        canvasnoise[number].Write()
    
    name="slope_distribution"
    canvas.append(TCanvas(name,name, 200, 10, 800, 800))
    canvas[-1].cd()
    slope.Draw()
    canvas[-1].Write()     
    canvas[-1].Print(filename)
    
    #rep = raw_input(' when ready hit return: ')   
    timer.append("POH test done")
    timer.append(time.time()-start_time)
    print "POH bias scan finished"
    for i in canvasnoise:
        i.Close()
    for i in canvas:
        i.Close()
if testonefibervisual == True:
     sector.init_tube()
     i=1
     pitch=5
     biasrange=range(10,60,pitch)
     for bias in biasrange:
         out = "group "+ str(sector.poh[i].group)
         print out
         sector.ccu.send(out).readlines()
         print  sector.ccu.send(out).readlines()
         out = sector.poh[i].name + " setall 3 3 3 3 "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
         print out
         print sector.ccu.send(out).readlines()
         sleep(2)
    
if testonefiber == True:
     sector.init_tube()
     i=10
     fiber=1
     pitch=2
     trial=10
     biasrange=range(0,60,pitch)
     c2=TCanvas()
     name="h2d"
     th2d= TH2D(name,name,60,0,60,130,0,1300)
     thlist=[]
     for ch in sector.poh[i].fedchannels[1:]:
           if 12-fiber+1 == ch:
               for k in range(0,trial):
                   for bias in biasrange:
              
                       out = "group "+ str(sector.poh[i].group)
                       sector.ccu.send(out).readlines()
                       out = sector.poh[i].name + " setall 3 3 3 3 "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
                       print sector.ccu.send(out).readlines()
                       sys.stdout.write("\r working "+str(bias))
                       sys.stdout.flush()
                       sector.fed.send("reset fifo").readlines()
                       sleep(0.1)
                       inin=sector.fed.query("rms " +str(ch))
                       channel,mean,rms,name=inin.split()
                       th2d.Fill(float(bias),float(mean))
                       th2d.SetMarkerStyle(20)
                       c2.cd()
                       th2d.Draw("colz")
                       c2.Update()
      
     rep = raw_input(' when ready hit return: ')
        
        
########################################################   

##############


########################################################
if pohplusfechannel_repeat==True:
      
    testname="Fed channels test optimized"
    log.info(testname)
    counter =0
    errorc=0
    canvas=[]
    canvasnoise=[]
    slope=TH1F("slope","slope",60,0,60)
    print "1) START TRIGGER"
    print "2) Power on mb and ccu, modules off "
    kadc=[]
    krms=[]
    sector.init_tube()
    for n in sublist:
        name="bundle_"+str(sector.poh[n[0]].bundle)
        canvas.append(TCanvas(name,name, 200, 10, 800, 800))
        name="bundle_noise"+str(sector.poh[n[0]].bundle)
        canvasnoise.append(TCanvas(name,name, 200, 10, 800, 800))
        print  "  Fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
        rep = raw_input(' when ready hit return: ')
        # kadc=[]
        # krms=[]
        for i in n:
            number=sector.poh[n[0]].bundle-1
            print number
            number=sublist.index(n)
            print number
            kadc.append([])
            krms.append([])
            print "poh",i
            pos=0
            for ch in sector.poh[i].fedchannels[1:]:
                out ="group "+sector.poh[i].group
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean,rms,name=response.split()
                out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name +" set b"+str(3-pos)+" 60"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean1,rms,name=response.split()
                if  float(mean1) - float(mean) < 1000: 
                    errorc=errorc+1
                    errorstring="poh "+str(i)+" fiber " +str(13-ch) +" failure max-min ADC "+ str(float(mean1)) +" "+ str(float(mean))
                    # print "fiber",13-ch ,"failure", float(mean1) , float(mean)
                    log.error(errorstring)
                else:
                    okstring= "poh "+str(i)+" fiber "+ str(13-ch )+" ok"
                    # print "fiber",13-ch ,"ok"
                    log.ok(okstring)
    
                pos=pos+1
        if errorc==0:
            print "ok"
        counter =0
        
        testname="POH bias scan"
        log.info(testname)
       ###################################
        color=[kBlue,kOrange,kGreen,kOrange+3,kGray,kWhite,kRed,kBlack,kYellow,kViolet,kPink,kAzure]
        mgslope =  TMultiGraph()
        mgnoise =  TMultiGraph()
        mgnoise.SetMaximum(10.)
        memory=[]
        print sector.poh[n[0]].bundle
        canvas[number].cd()
        # here
        repeat=False
        iteration=-1
        for i in n:
            while (repeat is False and iteration==0) or (repeat == True and iteration>0 and iteration < 3):
                iteration=iteration+1
                if iteration>0.1:
                    rep = raw_input(' Turn off pohs ,clean and hit return: ')
                print "poh",i
                pos=-1
                if iteration>0.1:
                    for ch in sector.poh[i].fedchannels[1:]:
                        del kadc[number][-1]
                        del krms[number][-1]
                for ch in sector.poh[i].fedchannels[1:]:
                    number=sector.poh[n[0]].bundle-1
                    number=sublist.index(n)
                    pos=pos+1
                    out ="group "+sector.poh[i].group
                    sector.ccu.send(out).readlines()
                    out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                    sector.ccu.send(out).readlines()
                    kadc[number].append(TGraph())
                    krms[number].append(TGraph())
                    out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                    sector.ccu.send(out).readlines()
                    # k[-1].SetMarkerColor(color[ch-1])
                    name= " Fb "+str(13-ch)
                    kadc[number][-1].SetName(name)
                    kadc[number][-1].SetFillStyle(0);
                    kadc[number][-1].SetFillColor(0);
                    name= " Fb "+str(13-ch)
                    krms[number][-1].SetName(name)
                    krms[number][-1].SetFillStyle(0);
                    krms[number][-1].SetFillColor(0);
                    tempgr=TGraph()
                    ngr=0

                    fed.send("reset fifo").readlines()
                    pitch=2
                    biasrange=range(0,60,pitch)
                    for bias in biasrange:
                        pb(bias,biasrange[-1])
                        out = sector.poh[i].name + " setall 3 3 3 3 "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
                        sector.ccu.send(out).readlines()
                        fed.send("reset fifo").readlines()
                        sleep(0.1)
                        inin=fed.query("rms " +str(ch))
                        channel,mean,rms,name=inin.split()
                        kadc[number][-1].SetPoint(bias/pitch,bias,float(mean))
                        if float(mean)>50 and float(mean) <1000: 
                            krms[number][-1].SetPoint(ngr,bias,float(rms))
                            tempgr.SetPoint(ngr,bias,float(mean))
                            ngr=ngr+1


                    kadc[number][-1].SetLineWidth(3)
                    krms[number][-1].SetLineWidth(3)
                    grfit= TF1("pol1","pol1",0,70)
                    if tempgr.GetN()>2:
                        maxx=krms[number][-1].GetHistogram().GetMean(2)
                        tempgr.Fit(grfit,"q")
                        if grfit.GetParameter(1)>35.0  and maxx <5.0:
                            okstring="poh "+ str(i)  + " fiber " + str(13-ch) + "  slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, OK, NOISE OK"
                            log.ok(okstring)
                        else:
                            if grfit.GetParameter(1)<=35.0:
                                errorstring ="poh "+ str(i)  + " fiber " + str(13-ch) + " slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, LOW SLOPE"
                                log.error(errorstring) 
                                repeat=True
                            if maxx >=5.0:
                                errorstring ="poh "+ str(i)  + " fiber " + str(13-ch)+ "  slope: "+str(round(grfit.GetParameter(1),2)) + " ADC/bias, noise: "+str(round(maxx,2))+ " ADC, HIGH NOISE"
                                log.error(errorstring)


                    else:
                        errorstring="poh "+ str(i)  + " fiber " + str(13-ch) + " CHANNEL DOES NOT WORK"
                        log.error(errorstring)
                        repeat=True
                    kadc[number][-1].SetLineColor(color[12-ch ])
                    kadc[number][-1].SetMarkerStyle(20+ch)
                    krms[number][-1].SetLineColor(color[12-ch ])
                    krms[number][-1].SetMarkerStyle(20+ch)
                    if tempgr.GetN()>2:
                        title=kadc[number][-1].GetName()+" : " +str(round(grfit.GetParameter(1),2))+ " ADC/bias"
                        kadc[number][-1].SetTitle(title)
                        maxx=krms[number][-1].GetHistogram().GetMaximum(); 
                        title=krms[number][-1].GetName()+" : " +str(round(maxx,2))+ " ADC"
                        krms[number][-1].SetTitle(title)
                        slope.Fill(grfit.GetParameter(1)*0.56,1)
                    else:
                        title=kadc[number][-1].GetName()+" LOW SLOPE" 
                        kadc[number][-1].SetTitle(title)
                        title=krms[number][-1].GetName()+" LOW SLOPE" 
                        krms[number][-1].SetTitle(title)
                        slope.Fill(0,1)
                canvas[number].cd()
                mgslope.Add(kadc[number][-1],"ALP")
                kadc[number][-1].Write()
                mgslope.Draw("ALP same" )
                canvasnoise[number].cd()
                mgnoise.Add(krms[number][-1],"ALP")
                krms[number][-1].Write()
                mgnoise.Draw("ALP same" )
                nameprint="SLOPE Bundle "+str(sector.poh[n[0]].bundle)
                canvas[number].BuildLegend(0.7,0.1,0.9,0.7,nameprint)
                canvas[number].Update()
                nameprint="NOISE Bundle "+str(sector.poh[n[0]].bundle)
                canvasnoise[number].BuildLegend(0.1,0.3,0.4,0.9,nameprint)
                canvasnoise[number].Update()
                canvas[number].Print(filename,nameprint)
                canvas[number].Write()
                canvasnoise[number].Print(filename,nameprint)
                canvasnoise[number].Write()
    
    name="slope_distribution"
    canvas.append(TCanvas(name,name, 200, 10, 800, 800))
    canvas[-1].cd()
    slope.Draw()
    canvas[-1].Write()     
    canvas[-1].Print(filename)
    
    #rep = raw_input(' when ready hit return: ')   
    timer.append("POH test done")
    timer.append(time.time()-start_time)
    print "POH bias scan finished"
    for i in canvasnoise:
        i.Close()
    for i in canvas:
        i.Close()









############

########################################################
if digital_bias_scan==True:
    level=TLine(0,ntrigger1,0,ntrigger1)
    level.SetLineColor(2)
    testname="Starting digital bias scan "+str(ntrigger1)+" trigger"
    log.info(testname)
    timer.append(testname)
    timer.append(time.time()-start_time)
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c2.Divide(4,3)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("PiggyN").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0
    memox=0
    memoy=0
    histo=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    
    initmod()
    ######################################################
    delay=18
    ######################################################
    inputo= str(bin(delay))+str(11)
    inputo= int(inputo,2)
    for n in range(0,len(module)):
        ######################################################
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 "+str(d1[n])).readlines()
        ccu.send("delay25 set d2 "+str(d2[n])).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        ######################################################
        output="cn " + moduleg[n]
        pxfec.send(output).readlines()
        pxfec.send("module " + str(module[n])).readlines()
        output="tbm reset tbm"
        pxfec.send(output).readlines()
        initmod()
        pxfec.send("tbm").readlines()
        pxfec.send("tbmplldelay "+str(inputo)).readlines()
        pxfec.send("tbm disable triggers").readlines()
        sleep(0.1)
    ###############################################################################
    memory=[]
    ######################################################
    ######################################################
    digfed.send("initFitelN").readlines()
    histo=[]
    for i in range(0,12):
        name="fiber_"+str(i+1)
        histo.append(TH1F(name,name, 45,0,90))
        c2.cd(i+1)
        level.Draw()
        c2.Update() 
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    for bias in range(10,60,2):
        
        print"bias", bias
        for k in sector.pohlist:
            ccu.send("group " + sector.poh[k].group).readlines()
            out= sector.poh[k].name +" setall 3 3 3 3   "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
            print out
            a =ccu.send(out).readlines()
        sleep(1)
        for ntr in range(0,ntrigger1):
            #print ntr,
            nheader=[]
            ntrailer=[]
            for i in range(0,12):
                nheader.append(0)
                ntrailer.append(0)
                memory.append(0)
            strayout= digfed.send("shazamN").readlines()
            sleep(0.1)
            #print "######################################################"
            for s in strayout:
                #print s
                for nfiber in range(1,13):
                    fiber=re.search("FIBER "+ str(nfiber) +" ",s)
                    if fiber:
                        header=re.search("TBM_H",s)
                        trailer=re.search("TBM_T",s)
                        if header:
                            #print "fiber",nfiber, "H OK"
                            nheader[nfiber-1] = nheader[nfiber-1]+1
                        if trailer:
                            #print "fiber",nfiber, "T OK"
                            ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
            #print nheader, ntrailer
            for i in range(0,12):
                if  nheader[i]==2 and  ntrailer[i]==2:
                    histo[i].Fill(bias,1)
                    c2.cd(i+1)
                    level.Draw()
                    histo[i].Draw()
                    
                    c2.Update() 
    ###############################################################################
    rep = raw_input(' when ready hit return: ')
    c2.Print(filename)
    c2.Write()
    c2.Close()
########################################################

########################################################
if fedchtest==True:
    log.ok("Fed channels test")
    counter =0
    errorc=0
    print "START TRIGGER"
    print "power on mb and ccu, modules off "
    for n in sublist:
        print  " fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
        rep = raw_input(' when ready hit return: ')
        for i in n:
            print "poh",i
            pos=0
            for ch in sector.poh[i].fedchannels[1:]:
                out ="group "+sector.poh[i].group
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean,rms,name=response.split()
                out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name +" set b"+str(3-pos)+" 60"
                sector.ccu.send(out).readlines()
                sleep(0.5)
                fed.send("reset fifo").readlines()
                response=fed.query("rms " + str(ch))
                channel,mean1,rms,name=response.split()
                if  float(mean1) - float(mean) < 1000: 
                    errorc=errorc+1
                    errorstring="poh "+str(i)+" fiber " +str(13-ch) +" failure max-min ADC "+ str(float(mean1)) +" "+ str(float(mean))
                    # print "fiber",13-ch ,"failure", float(mean1) , float(mean)
                    log.error(errorstring)
                else:
                    okstring= "poh "+str(i)+" fiber "+ str(13-ch )+" ok"
                    # print "fiber",13-ch ,"ok"
                    log.ok(okstring)
    
                pos=pos+1
        if errorc==0:
            print "ok"

########################################################
########################################################
if pohbiastest==True:
    counter =0
    sector.init_tube()
   
    print "power on mb and ccu, modules off "
    print "START TRIGGER"
    print "POH bias scan"
    color=[kBlue,kOrange,kGreen,kOrange+3,kGray,kWhite,kRed,kBlack,kYellow,kViolet,kPink,kAzure ]
    for n in sublist:
        
        c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
        k=[]
        mg =  TMultiGraph()
        memory=[]
        c2.cd()
        print  " fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
        rep = raw_input(' when ready hit return: ')
        for i in n:
            print "poh",i
            pos=-1
            for ch in sector.poh[i].fedchannels[1:]:
                pos=pos+1
                out ="group "+sector.poh[i].group
                sector.ccu.send(out).readlines()
                out =sector.poh[i].name + " setall 0 0 0 0 0 0 0 0"
                sector.ccu.send(out).readlines()
                k.append(TGraph())
                out =sector.poh[i].name +" set g"+str(3-pos)+" 3"
                sector.ccu.send(out).readlines()
                # k[-1].SetMarkerColor(color[ch-1])
                name= "Channel " + str(ch)+ " Fiber "+str(13-ch)
                k[-1].SetName(name)
                tempgr=TGraph()
                ngr=0
                
                # print "poh ", i, " fiber fed ch ", ch
                fed.send("reset fifo").readlines()
                biasrange=range(0,60,2)
                for bias in biasrange:
                    pb(bias,biasrange[-1])
                    out =sector.poh[i].name +" set b"+str(3-pos)+" "+str(bias)
                    sector.ccu.send(out).readlines()
                    # print ccu.send(str(sector.poh[i].name)+ " read").readlines()
                    fed.send("clear fifo").readlines()
                    sleep(0.5)
                    inin=fed.query("rms " +str(ch))
                    channel,mean,rms,name=inin.split()
                    #print bias, mean
                    k[-1].SetPoint(bias/2,bias,float(mean))
                    if float(mean)>50 and float(mean) <1000: 
                        tempgr.SetPoint(ngr,bias,float(mean))
                        ngr=ngr+1
                    # print "poh ", i, " fiber fed ch ", ch," bias ",bias, " ADC ", mean
                
                k[-1].SetLineWidth(3)
                grfit= TF1("pol1","pol1",0,70)
                if tempgr.GetN()>2:
                    tempgr.Fit(grfit,"q")
                    if grfit.GetParameter(1)>40.0:
                        okstring="poh "+ str(i)  + " fiber " + str(13-ch) + "  slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, OK"
                        log.ok(okstring)
                    else:
                        errorstring ="poh "+ str(i)  + " fiber " + str(13-ch) + " slope: "+str(round(grfit.GetParameter(1),2))+ " ADC/bias, LOW SLOPE"
                        log.error(errorstring) 
                        
                else:
                    errorstring="poh "+ str(i)  + " fiber " + str(13-ch) + " CHANNEL DOES NOT WORK"
                    log.error(errorstring)
                k[-1].SetLineColor(color[12-ch ])
                mg.Add(k[-1],"ALP")
                k[-1].Write()
                mg.Draw("ALP same" )
                c2.BuildLegend()
                c2.Update()
               
        c2.Update()
        c2.Write()
        c2.Print(filename)
    rep = raw_input(' when ready hit return: ')   
    timer.append("POH test done")
    timer.append(time.time()-start_time)
    print "POH bias scan finished"
########################################################
########################################################
if testsdarda==True:
    testname="SDA RDA range"
    log.info(testname)
    cutoff=10
    sector.init_tube()
   
    print "1) Power on mb and ccu"
    print "2) Modules ON"
    rep = raw_input(' when ready hit return: ')
    initmod()
    k=[]
    histo=[]
    modulelist=[i for i in range(0,32)]
    totalhisto=[]
    for m in modulelist:
        name= "module" + str(m)
        histo.append(TH2D( name , name , 64,0,63,64,0,63 ))
       
    for i in range(0,2):
        name= "total_histo_"+sector.group[i].name
        totalhisto.append(TH2D( name, name, 64,0,63,64,0,63 )) 
    for i in rdasdascanrange: ###0= L12 1=L34
        print "group", sector.group[i].name
        sector.group[i].GetSDARDA3(modulelist,histo)
        for k in module:
                for l in histo:
                    if int(l.GetEntries())>cutoff and l.GetName() == "module"+str(k):
                        print "module ",k, "entries", l.GetEntries()
                        totalhisto[i].Add(l)
        
    totalhisto[1].Add(totalhisto[0],-1)              
    c4= TCanvas( 'c4','c4', 200, 10, 800, 800 )
    c5= TCanvas( 'c5','c5', 200, 10, 800, 800 )
    c6= TCanvas( 'c6','c6', 200, 10, 800, 800 )
    c6.Divide((len(module)+1)/2,2)
    c4.Divide(8,4)
    c4.cd()
    n=1
    nn=1

    print "module id  N RDA SDA  STATUS"
    for i in histo:
        looser=True
        c4.cd(n)
        i.Draw("COLZ")
        n=n+1
        i.Write()
        c4.Update()
        for k in module:
                 if i.GetName() == "module"+str(k):
                     c6.cd(nn)
                     i.Draw("COLZ")
                     nn=nn+1
        looser=True
        if int(i.GetEntries())>cutoff:
            for k in module:
                 if i.GetName() == "module"+str(k):
                     # print i.GetName(),int(i.GetEntries()),int(i.GetMean(1)),int(i.GetMean(2)), "  OK"
                     #okstring =  "module "+str(k) + " rda: " + str(int(i.GetMean(1)))+ " sda: "+ str(int(i.GetMean(2)))+ "  OK"
                     #print "this should work ",realmean(i,0),realmean(i,1)
                     #i.Fill(int(i.GetMean(1)),int(i.GetMean(2)),10)
                     i.GetXaxis().SetRangeUser(30,64)
                     d1[module.index(k)]=int(i.GetMean(1))
                     i.GetYaxis().SetRangeUser(30,64)
                     i.GetXaxis().SetRangeUser(0,64)
                     d2[module.index(k)]=int(i.GetMean(2))
                     i.GetXaxis().SetRangeUser(0,64)
                     i.GetYaxis().SetRangeUser(0,64)
                     i.Fill(d1[module.index(k)],d2[module.index(k)],10)
                     okstring =  "module "+str(k) + " rda: " + str(d1[module.index(k)])+ " sda: " + str(d2[module.index(k)])+ "  OK"
                     log.ok(okstring)
                     looser=False
            if looser==True:         
                errorstring=i.GetName() + " rda: " + str(d1[module.index(k)])+ " sda: "+ str(d2[module.index(k)])+ "  IS CONNECTED BUT NOT USED IN THE TEST!, CHECK  THE MODULE LIST"
                log.error(errorstring)
        if int(i.GetEntries())<cutoff:
             for k in module:
                 if i.GetName() == "module"+str(k):
                     errorstring="module "+str(k) +" rda: " + str(d1[module.index(k)])+ " sda: "+ str(d2[module.index(k)]) + " THE MODULE DONT ANSWER SO OFTEN AS EXPECTED, CHECK IF IT IS CONNECTED, module found "+str(int(i.GetEntries())) + " times"
                     log.error(errorstring)

    c5.Divide(2)
    c5.cd()
    for i in range(0,2):
        c5.cd(i+1)
        totalhisto[i].Draw("COLZ")
        name= sector.group[i].name
        totalhisto[i].GetXaxis().SetRangeUser(30,64)
        m1=int(totalhisto[i].GetMean(1))
        totalhisto[i].GetYaxis().SetRangeUser(30,64)
        totalhisto[i].GetXaxis().SetRangeUser(0,64)
        m2=int(totalhisto[i].GetMean(2))
        totalhisto[i].GetXaxis().SetRangeUser(0,64)
        totalhisto[i].GetYaxis().SetRangeUser(0,64)
        print "GROUP ",name,"mean RDA SDA", m1,m2
        pxfec.send("group " + sector.group[i].name).readlines()
        
        if m1 !=0 and m2 !=0 and (totalhisto[i].GetEntries !=0):
            print "GROUP ",name,"mean RDA SDA",m1,m2
            pxfec.send("delay25 set d1 "+str(m1)).readlines()
            pxfec.send("delay25 set d2 "+str(m2)).readlines()
        else:
            print "HISTO ARE EMPTY, SETTING SDA RDA TO 53 53 BY DEFAULT"
            print "GROUP ",name,"mean RDA SDA", int(53),int(53)
            pxfec.send("delay25 set d1 "+str(53)).readlines()
            pxfec.send("delay25 set d2 "+str(53)).readlines()
        totalhisto[i].Write()
    c5.Write()
    c4.Write()
    c6.Write()
    c4.Print(filename)
    c5.Print(filename)
    c6.Print(filename)
    timer.append("SDARDA range test done")
    print moduleg
    print module
    print d1
    print d2
    timer.append(time.time()-start_time)
    rep = raw_input(' when ready hit return: ')

########################################################
########################################################     
if testtrigger== True:
    testname="Trigger Distribution test"
    log.info(testname)
    print "1) Power on mb and ccu"
    print "2) Modules ON"
    print "3) Trigger configured, disable all trigger channels"
    rep = raw_input(' when ready hit return: ')
    digfed.send("piggyN").readlines()
    digfed.send("initFitelN").readlines()
    initmod()
   
   
    testrange=range(0,len(module))
    for m in testrange:
            ######################################################
            output="group " +moduleg[m]
            ccu.send(output).readlines()
            ccu.send("delay25 set d0 0").readlines()
            ccu.send("delay25 set d1 " +str(d1[m])).readlines()
            ccu.send("delay25 set d2 " +str(d2[m])).readlines()
            ccu.send("delay25 set d3 0").readlines()
            ccu.send("delay25 set d4 0").readlines()
            ######################################################
            print "\n"
            output="cn " + moduleg[m] 
            pxfec.send(output).readlines()
            sleep(0.1)
            output="cn scan hubs"
            message=pxfec.send(output).readlines()
            print "I should find module", module[m], "I find module(s)", message
            # sleep(0.1)
            output="module "+ str(module[m])
            pxfec.send(output).readlines()
            output="tbm reset tbm "
            # sleep(0.1)
            pxfec.send(output).readlines()
            # sleep(0.1)
            output="tbm clear counter "
            pxfec.send(output).readlines()
            output="tbm clear stack "
            pxfec.send(output).readlines()
            A0=-1
            B0=-1
            lasttrig=100
            kA=0
            kB=0
            ##WARMUP##
            if m is 0:
                for i in range(0,int(lasttrig/2)):
                    output="tbm readA"
                    pxfec.send(output).readlines()
                    output="cn inject trigger"
                    pxfec.send(output).readlines()
                    # digfed.send("shazamN").readlines()
                    output="tbm readB"
                    pxfec.send(output).readlines()
                    output="cn inject trigger"
                    pxfec.send(output).readlines()
                    # digfed.send("shazamN").readlines()
                    message= "\r############# WARMUP"
                    sys.stdout.write(message)
                    sys.stdout.flush()
            for i in range(0,lasttrig):
                output="tbm readA"
                readback = pxfec.send(output).readlines()
                for s in readback:
                    success= re.search("TBM A register 2",s)
                    if success: 
                        # print s
                        valueA=s.split()
                        A=int(valueA[-1])
                        #print A
                    
                output="tbm readB"
                readback = pxfec.send(output).readlines()
                for s in readback:
                    success= re.search("TBM B register 2",s)
                    if success: 
                        # print s
                        valueB=s.split()
                        B=int(valueB[-1])
                        # bprint B
                   
                
                if A0 is not -1 and A-A0 == 1: kA=kA+1
                if B0 is not -1 and B-B0 == 1: kB=kB+1
                A0=A
                B0=B
                if A0==255: A0=0
                if B0==255: B0=0

                message="module "+ str(module[m]) +"############# TBMA: "+str(kA)+"/"+str(lasttrig-1)+" ,  TBMB: "+str(kB)+"/"+str(lasttrig-1)
                sys.stdout.write("\r"+message)
                sys.stdout.flush()
                if i==(lasttrig-1): 
                    message="module "+ str(module[m]) +"############# TBMA: "+str(kA)+"/"+str(lasttrig-1)+" ,  TBMB: "+str(kB)+"/"+str(lasttrig-1)
                    if kA> 90 and kB>90:
                        log.ok(message)
                    else:log.error(message)
                output="cn inject trigger"
                pxfec.send(output).readlines()
                #digfed.send("shazamN").readlines()
                    
########################################################
########################################################
if tbmplldelaytest==True:
    testname="TBM pll delay test"
    log.info(testname)
    sector.init_tube()
    print "1) Power on mb and ccu"
    print "2) Modules ON"
    print "3) Trigger configured, disable all trigger channels"
    print 
    print "fiber bundle in the digital fed, start trigger"
    rep = raw_input(' when ready hit return: ')
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    n=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    for i in module:
        sendd1 = "delay25 set d1 " + str(d1[n])
        sendd2 = "delay25 set d2 " + str(d2[n])
        output=ccu.send(sendd1).readlines() #SDA d2 x
        print output
        output=ccu.send(sendd2).readlines()  #RDA d1 y
        print output
        output=pxfec.send("cn scan hubs").readlines()
        print"hope is ", str(i)
        print output
        pxfec.send("module " + str(i)).readlines()
        pxfec.send("tbm disable triggers").readlines()
        n=n+1

    graph=[]
    histo2=[]
    histo3=[]
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    digfed.send("setupTTC").readlines()
    for n in range(0,len(module)):
      for fb in fiber[n]:
        print "module ", module[n], " fiber ", fb
        name= "Fedch"+str(fb)
        histo2.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
        histo3.append(TH2D(name+"lazy",name+"lazy",8,-0.5,7.5,8,-0.5,7.5))
        graph.append(TGraph())
        
        out ="fiber " + str(fb)
        digfed.send(out).readlines()
        for delay in range(0,64):
            
          inputo= str(bin(delay))+str(11)
          inputo= int(inputo,2)
          
          print "inputo " +str(inputo)
          pxfec.send("tbmplldelay "+ str(inputo)).readlines()
          nheader = 0
          ntrailer = 0
          bdelay=bin(delay)
          if len(bdelay)>5:
            x=bdelay[-3:] # (last 3 elements)
            y=bdelay[:-3] # (first 3 elements)

          if len(bdelay) <=5:
            x=bdelay
            y=bin(0)
          n=False
          t=False
          r=False
          eandih=[]
          eandit=[]
          eandir=[]
          cut=[]
          for ntr in range(0,ntrigger): 
              
              n=False
              t=False
              r=False
              header=0
              trailer=0
              strayout=0
              strayout= digfed.send("getFromPiggy").readlines()
              sleep(0.1)
              for s in strayout:
                  header=re.match("011111111100 : TBM_Header",s)
                  trailer=re.match("011111111110 : TBM Trailer",s)
                  roc=re.search("due to bit flip?",s)
                  if header:
                      nheader = nheader+1
                      n=True
                      # print "ok ",
                  if trailer:
                      t=True
                      # print "ok",
                      ntrailer = ntrailer+1
                  if roc:
                      r= True
                  
              
              if n is False:
                  eandih.append(ntr)
              if t is False:
                  eandit.append(ntr)
              if r is True:
                  eandir.append(ntr)
              if (n is False) or (r is True) or (t is False):
                  strang="############### Trigger number "+ str(ntr) +" ###############  "
                  cut.append(strang)
                  for s in strayout:
                      cut1=re.search("01",s)
                      cut2=re.search("10",s)
                      if cut1 or cut2:
                          cut.append(s)
                      
          print "###################################################"
          print "  header: "+str(nheader/2)
          print "  trailer: "+str(ntrailer/2)


          if len(eandih)>0: print "missing header at trigger n. ",eandih
          if len(eandit)>0: print "missing trailer at trigger n.",eandit
          if len(eandir)>0: print "roc bit flip ??????????????? ",eandit
          if  len(eandih)>0 or  len(eandit)>0 or len(eandir)>0:
              for s in cut:
                  print s
          for hist in histo2:
            if hist.GetName()== "Fedch"+str(fb):
              hist.Fill(float(int(x,2)),float(int(y,2)),float(nheader+ntrailer)/4/ntrigger*100)
              copen.cd()
              hist.Draw("COLZ TEXT")
              copen.Update()
          for hist in histo3:
            if hist.GetName()== "Fedch"+str(fb)+"lazy":
              hist.Fill(float(int(x,2)),float(int(y,2)),inputo)
          
    timer.append("tbmplldelay scan done")
    timer.append(time.time()-start_time)  
    print "done"

    k=0
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    
    c2.cd()
    c2.Divide(int(len(histo2)/2),2)
    for i in histo2:
        c2.cd(k+1)
        i.Draw("COLZ TEXT")
        i.Write()
        k=k+1
    c2.Update()
    c2.Print(filename)
    c2.Write()
    c3=TCanvas('c3', 'c3', 200, 10, 800, 800)
    c3.cd()
    c3.Divide(int(len(histo2)/2),2)
    for i in histo3:
        c3.cd(k+1)
        i.Draw("TEXT")
        i.Write()
        k=k+1
    c3.Update()

    c3.Print(filename)
    c3.Write()
    digfed.close()
    digfed.open()
########################################################
########################################################
if testpiggyN==True:
    testname="TBM pll delay testfor Piggy N fiber by fiber"
    log.info(testname)
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c3=TCanvas('c3', 'c3', 200, 10, 800, 800)
    c4=TCanvas('c4', 'c4', 200, 10, 800, 800)
    # copen.Divide(4,3)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0
    memox=0
    memoy=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    

   
    histo=[]
    histo_HT=[]
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    
    for n in range(0,len(module)):
     sendd1 = "delay25 set d1 " + str(d1[n])
     sendd2 = "delay25 set d2 " + str(d2[n])
     output=ccu.send(sendd1).readlines() #SDA d2 x
     output=ccu.send(sendd2).readlines()  #RDA d1 y
     pxfec.send("cn " + str(moduleg[n])).readlines()
     pxfec.send("module " + str(module[n])).readlines()
     pxfec.send("tbm disable triggers").readlines()
    name= "AllFibers_mean"
    total=TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5)
    name= "AllFibers_stdev"
    total_stdev=TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5)
    sector.poh_set(20)
    for bundle in distinct_list:
        c2.Clear()
        c2.Divide(4,3)
        c3.Clear()
        c3.Divide(4,3)
        print "Plug bundle " + str(bundle) + " in Piggy North"
        rep = raw_input(' when ready hit return: ')
        for i in range(1,13):       
            name= "Fiber_"+str(i)+"_Bundle_"+str(bundle)
            histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
            name= "HT_Fiber_"+str(i)+"_Bundle_"+str(bundle)
            histo_HT.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
            
        for delay in range(0,64):
            sys.stdout.write("\r############# " + str(100*delay/63)+" %")
            sys.stdout.flush()
            inputo= str(bin(delay))+str(11)
            inputo= int(inputo,2)
            for n in range(0,len(module)):
                pxfec.send("cn " + str(moduleg[n])).readlines()
                pxfec.send("module " + str(module[n])).readlines()
                pxfec.send("tbm").readlines()
                pxfec.send("tbmplldelay "+ str(inputo)).readlines()
            nheader=[]
            ntrailer=[]
            nroc=[]
            bdelay=bin(delay)
            if len(bdelay)>5:
                x=bdelay[-3:] # (last 3 elements)
                y=bdelay[:-3] # (first 3 elements)

            if len(bdelay) <=5:
                x=bdelay
                y=bin(0)
                cut=[]
            sleep(0.1)
            for ntr in range(0,ntrigger): 
                n=False
                t=False
                r=False
                header=[]
                trailer=[]
                roc=[]
                strayout=[]
                dummyfiber=[]
                nheader=[]
                ntrailer=[]
                nroc=[]
                strayout= digfed.send("piggyN").readlines()
                strayout= digfed.send("shazamN").readlines()
                for i in range(0,12):
                    dummyfiber.append(i+1)
                    nheader.append(0)
                    ntrailer.append(0)
                    nroc.append(0)
         
                for s in strayout:
                    for nfiber in range(1,13):
                        fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                        if fiber:
                            header=re.search("TBM_H",s)
                            trailer=re.search("TBM_T",s)
                            for r_n in range(1,9):
                                roc_s="ROC_"+str(r_n) 
                                roc=re.search(roc_s,s) 
                                if roc: 
                                    nroc[nfiber-1]=nroc[nfiber-1]+1
                            if header:
                                nheader[nfiber-1] = nheader[nfiber-1]+1
                            if trailer:
                                ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                for hist in histo:        
                    for i in range(1,13):
                        if hist.GetName()== "Fiber_"+str(i)+"_Bundle_"+str(bundle):
                            if nheader[i-1]==2 and ntrailer[i-1]==2 and (nroc[i-1]==8 or nroc[i-1]==16):
                                hist.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                c2.cd(i)
                                hist.Draw("COLZ TEXT")
                                c2.Update()
                for hist in histo_HT:        
                    for i in range(1,13):
                        if hist.GetName()== "HT_Fiber_"+str(i)+"_Bundle_"+str(bundle):
                            if nheader[i-1]==2 and ntrailer[i-1]==2:
                                hist.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                total.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                c3.cd(i)
                                hist.Draw("COLZ TEXT")
                                c3.Update()
                                c4.cd()
                                total.Draw("COLZ TEXT")
                                c4.Update()

  
    for i in histo:
        i.Print(filename)
        i.Write()
    c2.Print(filename)
    c2.Write()
    c3.Print(filename)
    c3.Write()
    
########################################################
########################################################
if testpiggyS==True:
    testname="TBM pll delay testfor Piggy S fiber by fiber"
    log.info(testname)
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c3=TCanvas('c3', 'c3', 200, 10, 800, 800)
    c4=TCanvas('c4', 'c4', 200, 10, 800, 800)
    # copen.Divide(4,3)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelS").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0
    memox=0
    memoy=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    

   
    histo=[]
    histo_HT=[]
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    
    for n in range(0,len(module)):
     sendd1 = "delay25 set d1 " + str(d1[n])
     sendd2 = "delay25 set d2 " + str(d2[n])
     output=ccu.send(sendd1).readlines() #SDA d2 x
     output=ccu.send(sendd2).readlines()  #RDA d1 y
     pxfec.send("cn " + str(moduleg[n])).readlines()
     pxfec.send("module " + str(module[n])).readlines()
     # pxfec.send("tbm disable triggers").readlines()
    name= "AllFibers_mean"
    total=TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5)
    name= "AllFibers_stdev"
    total_stdev=TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5)
    sector.poh_set(20)
    for bundle in distinct_list:
        c2.Clear()
        c2.Divide(4,3)
        c3.Clear()
        c3.Divide(4,3)
        print "Plug bundle " + str(bundle) + " in Piggy South"
        rep = raw_input(' when ready hit return: ')
        rep=1
        tryhard=0
        # success=False
        psuccess=[]
        for nfiber in range(0,12):psuccess.append(False)
        success=all(psuccess)
        tryhard=0
        psuccess=[]
        for nfiber in range(0,12):psuccess.append(False)
        success=all(psuccess)
        #############################"right mood section"
        while tryhard<20  and success==False :#and rep==1
            tryhard=tryhard+1
            success=all(psuccess)
            #print tryhard, psuccess, success, all(psuccess)
            print "Initializing Digital Fed, step ", tryhard
            digfed.send("reset").readlines()
            digfed.send("regreset").readlines()
            digfed.send("Pllreset").readlines()
            digfed.send("PLLrestPiggy").readlines()
            digfed.send("PiggyS").readlines()
            digfed.send("initFitelS").readlines()
            digfed.send("trigger 100").readlines()
            sleep(0.1)
            strayout= digfed.send("shazamS").readlines()
            for s in strayout:
                for nfiber in range(1,13):
                    fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                    if fiber:
                        header=re.search("TBM_H",s)
                        if header:psuccess[nfiber-1]=True
        if success: print "Digital Fed initialization OK"
        for i in range(1,13):       
            name= "Fiber_"+str(i)+"_Bundle_"+str(bundle)
            histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
            name= "HT_Fiber_"+str(i)+"_Bundle_"+str(bundle)
            histo_HT.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
            
        for delay in range(0,64):
            sys.stdout.write("\r############# " + str(100*delay/63)+" %")
            sys.stdout.flush()
            inputo= str(bin(delay))+str(11)
            inputo= int(inputo,2)
            for n in range(0,len(module)):
                pxfec.send("cn " + str(moduleg[n])).readlines()
                pxfec.send("module " + str(module[n])).readlines()
                pxfec.send("tbm").readlines()
                pxfec.send("tbmplldelay "+ str(inputo)).readlines()
            nheader=[]
            ntrailer=[]
            nroc=[]
            bdelay=bin(delay)
            if len(bdelay)>5:
                x=bdelay[-3:] # (last 3 elements)
                y=bdelay[:-3] # (first 3 elements)

            if len(bdelay) <=5:
                x=bdelay
                y=bin(0)
                cut=[]
            sleep(0.1)
            for ntr in range(0,ntrigger): 
                n=False
                t=False
                r=False
                header=[]
                trailer=[]
                roc=[]
                strayout=[]
                dummyfiber=[]
                nheader=[]
                ntrailer=[]
                nroc=[]
                strayout= digfed.send("piggyS").readlines()
                strayout= digfed.send("shazamS").readlines()
                for i in range(0,12):
                    dummyfiber.append(i+1)
                    nheader.append(0)
                    ntrailer.append(0)
                    nroc.append(0)
         
                for s in strayout:
                    print s
                    for nfiber in range(1,13):
                        fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                        if fiber:
                            header=re.search("TBM_H",s)
                            trailer=re.search("TBM_T",s)
                            for r_n in range(1,9):
                                roc_s="ROC_"+str(r_n) 
                                roc=re.search(roc_s,s) 
                                if roc: 
                                    nroc[nfiber-1]=nroc[nfiber-1]+1
                            if header:
                                nheader[nfiber-1] = nheader[nfiber-1]+1
                            if trailer:
                                ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                for hist in histo:        
                    for i in range(1,13):
                        if hist.GetName()== "Fiber_"+str(i)+"_Bundle_"+str(bundle):
                            if nheader[i-1]==2 and ntrailer[i-1]==2 and (nroc[i-1]==8 or nroc[i-1]==16):
                                hist.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                c2.cd(i)
                                hist.Draw("COLZ TEXT")
                                c2.Update()
                for hist in histo_HT:        
                    for i in range(1,13):
                        if hist.GetName()== "HT_Fiber_"+str(i)+"_Bundle_"+str(bundle):
                            if nheader[i-1]==2 and ntrailer[i-1]==2:
                                hist.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                total.Fill(float(int(x,2)),float(int(y,2)),float(1))
                                c3.cd(i)
                                hist.Draw("COLZ TEXT")
                                c3.Update()
                                c4.cd()
                                total.Draw("COLZ TEXT")
                                c4.Update()

  
    for i in histo:
        i.Print(filename)
        i.Write()
    c2.Print(filename)
    c2.Write()
    c3.Print(filename)
    c3.Write()
    
########################################################




########################################################
if testpiggyN_channel==True:
    testname="TBM pll delay testfor Piggy S channel by channel"
    log.info(testname)
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c3=TCanvas('c3', 'c3', 200, 10, 800, 800)
    c2.Divide(6,4)
    sector.init_tube()
    print "header trailer"
    
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    digfed.send("setupTTC").readlines()
    digfed.send("piggyN").readlines()
    digfed.send("initFitelN").readlines()
    n=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
       

   
    histo=[]
    histo_memo=[]
    histo_HT=[]
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    
    print  " STOP TRIGGER"
    rep = raw_input(' when ready hit return: ')
    #digfed.send("setupTTC").readlines()
    initmod()
    
    for n in range(0,len(module)):
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 "+str(d1[n])).readlines()
        ccu.send("delay25 set d2 "+str(d2[n])).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        output="cn "+ str(moduleg[n])
        pxfec.send(output).readlines()
        output="module " + str(module[n])             
        pxfec.send(output).readlines()
        output="tbm disable triggers"
        # output="tbm enable triggers"
        pxfec.send(output).readlines()
        

    name= "total"
    total=TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5)
    for i in range(1,13):    
        name= "Fiber_TBMA"+str(i)
        histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
        histo_memo.append(0)
    for i in range(1,13):    
        name= "Fiber_TBMB"+str(i)
        histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
        histo_memo.append(0)
         
    for delay in range(0,64):
        # digfed.send("fitel").readlines()
        # digfed.send("reset").readlines()
        # digfed.send("regreset").readlines()
        # digfed.send("setupTTC").readlines() 
        # digfed.send("piggyS").readlines()
        # digfed.send("initFitelN").readlines()
        sys.stdout.write("\r############# " + str(100*delay/63)+" %")
        sys.stdout.flush()
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            output="module " + str(module[n])
            pxfec.send(output).readlines()
            output="tbm"
            pxfec.send(output).readlines()
            output="tbm tbmplldelay "+ str(inputo)
            pxfec.send(output).readlines()
            
        nheader=[]
        ntrailer=[]
        nroc=[]
        bdelay=bin(delay)
        if len(bdelay)>5:
            x=bdelay[-3:] # (last 3 elements)
            y=bdelay[:-3] # (first 3 elements)

        if len(bdelay) <=5:
            x=bdelay
            y=bin(0)
            cut=[]
        for ntr in range(0,ntrigger): 
            digfed.send("regreset").readlines()
            header=[]
            trailer=[]
            roc=[]
            strayout=[]
            dummyfiber=[]
            nheader=[]
            ntrailer=[]
            nroc=[]
           
            strayout= digfed.send("shazamN").readlines()
            for i in range(0,24):
                dummyfiber.append(i+1)
                nheader.append(0)
                ntrailer.append(0)
                nroc.append(0)
         
            for s in strayout:
                for nfiber in range(1,13):
                    fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                    if fiber:
                        tbma=re.search("TBMA",s)
                        tbmb=re.search("TBMB",s)
                        header=re.search("TBM_H",s)
                        trailer=re.search("TBM_T",s)
                        if tbma:
                            # for r_n in range(1,5):
                            #     roc_s="ROC_"+str(r_n) 
                            #     roc=re.search(roc_s,s) 
                            #     if roc: 
                            #         nroc[nfiber-1]=nroc[nfiber-1]+1
                            if header:
                                    nheader[nfiber-1] = nheader[nfiber-1]+1
                            if trailer:
                                    ntrailer[nfiber-1] = ntrailer[nfiber-1]+1

                        if tbmb:
                            # for r_n in range(1,5):
                            #     roc_s="ROC_"+str(r_n) 
                            #     roc=re.search(roc_s,s) 
                            #     if roc: 
                            #         nroc[12+nfiber-1]=nroc[12+nfiber-1]+1
                            if header:
                                nheader[12+nfiber-1] = nheader[12+nfiber-1]+1
                            if trailer:
                                ntrailer[12+nfiber-1] = ntrailer[12+nfiber-1]+1

                         
            for i in range(1,13):
                if nheader[i-1]==1 and ntrailer[i-1]==1 :# and nroc[i-1]==4:
                        histo_memo[i-1]=histo_memo[i-1]+1
                        
                if nheader[12+i-1]==1 and ntrailer[12+i-1]==1 :# and nroc[12+i-1]==4:
                        histo_memo[12+i-1]=histo_memo[12+i-1]+1
                        
        for i in range(1,13):   
            if histo[i-1].GetName()== "Fiber_TBMA"+str(i):
                        histo[i-1].Fill(float(int(x,2)),float(int(y,2)),histo_memo[i-1])
                        c2.cd(i)
                        histo[i-1].Draw("COLZ TEXT")
                        c2.Update()
                        total.Fill(float(int(x,2)),float(int(y,2)),histo_memo[i-1])
            if histo[12+i-1].GetName()== "Fiber_TBMB"+str(i):
                        histo[12+i-1].Fill(float(int(x,2)),float(int(y,2)),histo_memo[12+i-1])
                        c2.cd(12+i)
                        histo[12+i-1].Draw("COLZ TEXT")
                        c2.Update()
                        total.Fill(float(int(x,2)),float(int(y,2)),histo_memo[12+i-1])
                        c3.cd()
                        total.Draw("COLZ TEXT")
                        c3.Update()
            histo_memo[i-1]=0
            histo_memo[12+i-1]=0
              
    
  
    rep = raw_input(' when ready hit return: ')
    c3.Print(filename)
    c3.Write()
    c2.Print(filename)
    c2.Write()
    digfed.close()
########################################################
########################################################
if testvanaiana==True:
    testname="Test Vana-Iana, in development"
    log.info(testname)
    fiber =8
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c2.Divide(4,2)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
       
    histo=[]
    
    print  " STOP TRIGGER"
    rep = raw_input(' when ready hit return: ')
    digfed.send("setupTTC").readlines()
    initmod()
    
    for n in range(0,len(module)):
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 " +str(d1[n])).readlines()
        ccu.send("delay25 set d2 " +str(d2[n])).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        output="cn "+ str(moduleg[n])
        pxfec.send(output).readlines()
        output="module " + str(module[n])             
        pxfec.send(output).readlines()
        output="tbm enable triggers"
        pxfec.send(output).readlines()
        output="tbm tbmplldelay "+ str(75)
        pxfec.send(output).readlines()
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        
       

    for i in range(1,5):    
        name= "TBMA_ROC"+str(i)
        histo.append(TH1D(name,name,255,0,255))
    for i in range(1,5):    
        name= "TBMB_ROC"+str(i)
        histo.append(TH1D(name,name,255,0,255))
     
    for vana  in range(0,255):
        
        for n in range(0,len(module)):
            output="cn "+ str(moduleg[n])
            pxfec.send(output).readlines()
            output="module " + str(module[n])
            pxfec.send(output).readlines()
            output="tbm"
            pxfec.send(output).readlines()
            output="enable triggers"
            pxfec.send(output).readlines()
            output="tbm tbmbdelay 16"
            pxfec.send(output).readlines()
            output="tbm tbmadelay 16"
            pxfec.send(output).readlines()
            output="roc 0:15"
            pxfec.send(output).readlines()
            output="Vana "+ str(vana)
            print output
            pxfec.send(output).readlines()
            sleep(0.1)
            output="ReadBack 12"
            pxfec.send(output).readlines()
            sleep(0.1)
        nroc=[]
        for i in range(0,8):nroc.append(0)
        
        roc=[]
        strayout=[]
        digfed.send("fiber "+ str(fiber)).readlines()
        digfed.send("TBMA").readlines()
        strayout= digfed.send("getFromFifo1").readlines()
        memo=-1
        for s in strayout:
            for r_n in range(1,5):
                roc_s="ROC #"+str(r_n) +" "
                roc=re.search(roc_s,s) 
                if roc: 
                    answer=s.split()
                    iana=answer[-1]
                    print s
                    # print iana
                    #value=int(iana,16)
                    #histo[r_n-1].Fill(vana, value)
            roc31=re.search("ROC # 31",s)
                   
        digfed.send("fiber "+ str(fiber)).readlines()
        digfed.send("TBMB").readlines()
        strayout= digfed.send("getFromFifo1").readlines()
        for s in strayout:
            for r_n in range(1,5):
                roc_s="ROC #"+str(r_n)  +" "
                roc=re.search(roc_s,s) 
                if roc: 
                    answer=s.split()
                    iana=answer[-1]
                    #value=int(iana,16)
                    #histo[4+r_n-1].Fill(vana, value)

                        

                         
        for i in range(0,8):
               c2.cd(i+1)
               histo[i].Draw("COLZ TEXT")
               c2.Update()
            
    
  
    rep = raw_input(' when ready hit return: ')
    c2.Print(filename)
    c2.Write()
    digfed.close()
########################################################
########################################################
if testroc==True:
    testname="Scan Roc Delay"
    log.info(testname)
    fiber =12
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    c2.Divide(4,2)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
       
    histo=[]
    
    print  " STOP TRIGGER"
    rep = raw_input(' when ready hit return: ')
    digfed.send("setupTTC").readlines()
    initmod()
    
    for n in range(0,len(module)):
        output="cn "+ str(moduleg[n])
        pxfec.send(output).readlines()
        output="module " + str(module[n])             
        pxfec.send(output).readlines()
        output="tbm enable triggers"
        pxfec.send(output).readlines()
        output="tbm tbmplldelay "+ str(75)
        pxfec.send(output).readlines()
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 " +str(d1[n])).readlines()
        ccu.send("delay25 set d2 " +str(d2[n])).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
       

    for i in range(1,5):    
        name= "TBMA_ROC"+str(i)
        histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
    for i in range(1,5):    
        name= "TBMB_ROC"+str(i)
        histo.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
     
    for delay in range(0,64):
        inputo= delay
        print inputo
        nheader=[]
        ntrailer=[]
        nroc=[]

        bdelay=bin(delay)
        if len(bdelay)>5:
            x=int(bdelay[-3:],2) # (last 3 elements)
            y=int(bdelay[:-3],2) # (first 3 elements)
        if len(bdelay) <=5:
            x=int(bdelay,2)
            y=int(bin(0),2)
            cut=[]
        print bdelay, x, y
        nroc=[]
        for n in range(0,len(module)):
            output="cn "+ str(moduleg[n])
            pxfec.send(output).readlines()
            output="module " + str(module[n])
            pxfec.send(output).readlines()
            output="tbm"
            pxfec.send(output).readlines()
            inputo= inputo
            output="tbm tbmadelay "+ str(inputo)
            pxfec.send(output).readlines()
            output="tbm tbmbdelay "+ str(inputo)
            pxfec.send(output).readlines()
        for i in range(0,8):
            nroc.append(0)
        for ntr in range(0,ntrigger): 
            roc=[]
            strayout=[]
            digfed.send("fiber "+ str(fiber)).readlines()
            digfed.send("TBMA").readlines()
            strayout= digfed.send("getFromFifo1").readlines()
            for s in strayout:
                    for r_n in range(1,5):
                        roc_s="ROC #"+str(r_n) +" "
                        roc=re.search(roc_s,s) 
                        if roc: 
                            nroc[r_n-1]=nroc[r_n-1]+1
                            
            digfed.send("fiber "+ str(fiber)).readlines()
            digfed.send("TBMB").readlines()
            strayout= digfed.send("getFromFifo1").readlines()
            for s in strayout:
                    for r_n in range(1,5):
                        roc_s="ROC #"+str(r_n)  +" "
                        roc=re.search(roc_s,s) 
                        if roc: 
                            nroc[4+r_n-1]=nroc[4+r_n-1]+1
                           

                        

                         
        for i in range(0,8):
               histo[i].Fill(x,y,nroc[i])
               c2.cd(i+1)
               histo[i].Draw("COLZ TEXT")
               c2.Update()
            
    
  
    rep = raw_input(' when ready hit return: ')
    c2.Print(filename)
    c2.Write()
    digfed.close()
########################################################
########################################################
if datatrasfertest==True:
    testname="Starting Data trasfer test with "+str(ntrigger1)+" trigger"
    log.info(testname)
    timer.append("Starting Data trasfer test with "+str(ntrigger1)+" trigger")
    timer.append(time.time()-start_time)
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("PiggyS").readlines()
    digfed.send("initFitelS").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0
    memox=0
    memoy=0
    histo=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    for n in range(0,len(module)):
        ######################################################
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 50").readlines()
        ccu.send("delay25 set d2 50" ).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        ######################################################
        output="cn " + moduleg[n]
        pxfec.send(output).readlines()
        pxfec.send("module " + str(module[n])).readlines()
        pxfec.send("tbm disable triggers").readlines()
        sleep(0.1)
    ###############################################################################
    memory=[]
    for bundle in distinct_list:
        print "Plug bundle "+ str(bundle) +" in Piggy South "
        rep = raw_input(' when ready hit return: ')
        memory.append([])
        histo.append([])
        for i in range(1,13):       
            name= "Fiber_"+str(i)+"_bundle_"+str(bundle)
            histo[bundle-1].append(TH1D(name,name,32,-0.5,15.5))
        c2.Clear()
        ######################################################
        delay=18
        ######################################################
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            pxfec.send("cn "+moduleg[n]).readlines
            pxfec.send("module " + str(module[n])).readlines()
            pxfec.send("tbm").readlines()
            pxfec.send("tbmplldelay "+str(inputo)).readlines()
            #pxfec.send("tbmplldelay 187").readlines()
            pxfec.send("tbm disable triggers").readlines()
            print "inputo", inputo
        nheader=[]
        ntrailer=[]
        nroc=[]
        for i in range(0,13): memory[bundle-1].append(0)
        digfed.send("initFitelS").readlines()
        for ntr in range(0,ntrigger1): 
             sys.stdout.write("\r############# " + str(100*ntr/ntrigger1)+" %")
             sys.stdout.flush()
             n=False
             t=False
             r=False
             header=[]
             trailer=[]
             roc=[]
             strayout=[]
             dummyfiber=[]
             nheader=[]
             ntrailer=[]
             nroc=[]
             strayout= digfed.send("shazamS").readlines()
             for i in range(0,12):
                 dummyfiber.append(i+1)
                 nheader.append(0)
                 ntrailer.append(0)
                 nroc.append(0)
         
             for s in strayout:
                 for nfiber in range(1,13):
                     fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                     if fiber:
                         header=re.search("TBM_H",s)
                         trailer=re.search("TBM_T",s)
                         for r_n in range(1,5):
                             roc_s="ROC_"+str(r_n) 
                             roc=re.search(roc_s,s) 
                             if roc: 
                                 nroc[nfiber-1]=nroc[nfiber-1]+1
                         if header:
                             nheader[nfiber-1] = nheader[nfiber-1]+1
                         if trailer:
                             ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                         
             for i in range(0,12):
                 if nheader[i]==2 and ntrailer[i]==2 : #and nroc[i]==8:
                            memory[bundle-1][i]=memory[bundle-1][i]+1
    ###############################################################################
    ll=0
    for k in range(0,len(histo)):
        c2.Clear()
        for i in range(0,len(histo[k])):
           histo[k][i].SetFillColor(i+1)
           histo[k][i].Fill(i,memory[k][i])
           norm=100/ntrigger1
           histo[k][i].Scale(norm)
           histo[k][i].SetMaximum(110.0)
           histo[k][i].SetMinimum(0)
           histo[k][i].Draw("same")
    c2.Update()
                                         
    for k in range(0,len(histo)):
        log.ok("BUNDLE "+ str(k)) 
        for i in range(0,len(histo[k])):
            value=100*memory[k][i]/ntrigger1
            message= "fiber "+ str(i+1) +" efficiency "+str( 100*memory[k][i]/ntrigger1)+ " %, "+ str(memory[k][i])+ "/"+str(ntrigger1)
            if value> 90:
                log.ok(message)
            else:
                log.error(message)
                

    timer.append(" Data trasfer test with "+str(ntrigger1)+" trigger finished")
    timer.append(time.time()-start_time)  
    for k in range(0,len(distinct_list)):
        for i in histo[k]:
            i.Write()
            i.Print(filename)
    #rep = raw_input(' when ready hit return: ')
    c2.Print(filename)
    c2.Write()
    c2.Close()
########################################################

########################################################
if datatrasfertest_repeat_N==True:
    testname="Starting Data trasfer test with "+str(ntrigger1)+" trigger"
    log.info(testname)
    timer.append("Starting Data trasfer test with "+str(ntrigger1)+" trigger")
    timer.append(time.time()-start_time)

    
    
    sector.init_tube()
    digfed.send("setupTTC").readlines()
    digfed.send("PiggyN").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    for n in range(0,len(module)):
        ######################################################
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 50").readlines()
        ccu.send("delay25 set d2 50" ).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        ######################################################
        output="cn " + moduleg[n]
        pxfec.send(output).readlines()
        pxfec.send("module " + str(module[n])).readlines()
        pxfec.send("tbm disable triggers").readlines()
        sleep(0.1)
    ###############################################################################
    nht = 0
    memory=[]
    memorystatus=[]
    globalmemory=[]
    ################################
    for k in sublist:
        bundle=sublist.index(k)+1
        # name="bundle_"+str(sector.poh[n[0]].bundle)
    #     canvas.append(TCanvas(name,name, 200, 10, 800, 800))
    #     name="bundle_noise"+str(sector.poh[n[0]].bundle)
    #     canvasnoise.append(TCanvas(name,name, 200, 10, 800, 800))
    #     print  "  Fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
    # ################################
    #for bundle in distinct_list:
        #print distinct_list
        print "Plug bundle "+ str(bundle) +" in Piggy North "
        rep = raw_input(' when ready hit return: ')
        memory.append([])
        globalmemory.append([])
        memorystatus.append([])
        ######################################################
        delay=27
        ######################################################
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            pxfec.send("cn "+moduleg[n]).readlines
            pxfec.send("module " + str(module[n])).readlines()
            pxfec.send("tbm").readlines()
            pxfec.send("tbmplldelay "+str(inputo)).readlines()
            pxfec.send("tbm disable triggers").readlines()
            print "module ", module[n],"Tbm pll delay:", inputo
        for i in range(0,12): 
            memory[bundle-1].append(0)
            memorystatus[bundle-1].append(False)
            globalmemory[bundle-1].append(0)
        digfed.send("initFitelN").readlines()
        rep=1
        tryhard=0
        goon=all(globalmemory[bundle-1])
        # success=False
        psuccess=[]
        for nfiber in range(0,12):psuccess.append(False)
        success=all(psuccess)
        while rep <6 and goon is False:
            tryhard=0
            psuccess=[]
            for nfiber in range(0,12):psuccess.append(False)
            success=all(psuccess)
            #############################"right mood section"
            while tryhard<20  and success==False :#and rep==1
                tryhard=tryhard+1
                success=all(psuccess)
                # difed.close()
                # digfed=SimpleSocket( 'localhost', dfedport)
                #print tryhard, psuccess, success, all(psuccess)
                sleep(1)

                print "Initializing Digital Fed, step ", tryhard
                digfed.send("reset").readlines()
                digfed.send("regreset").readlines()
                digfed.send("Pllreset").readlines()
                digfed.send("PLLrestPiggy").readlines()
                digfed.send("PiggyN").readlines()
                digfed.send("initFitelN").readlines()
                digfed.send("trigger 100").readlines()
                sleep(0.1)
                strayout= digfed.send("shazamS").readlines()
                for s in strayout:
                    print s
                    for nfiber in range(1,13):
                        fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                        if fiber:
                            header=re.search("TBM_H",s)
                            if header:psuccess[nfiber-1]=True
            if success: print "Digital Fed initialization OK"
    
            #############################"right mood section"
            for i in range(0,12):memory[bundle-1][i]=0
            print "iteration number:", rep
            for ntr in range(0,ntrigger1): 
                sys.stdout.write("\r############# " + str(100*ntr/ntrigger1)+" %")
                sys.stdout.flush()
                header=[]
                trailer=[]
                strayout=[]
                nheader=[]
                ntrailer=[]
                strayout= digfed.send("shazamN").readlines()
                for i in range(0,12):
                    nheader.append(0)
                    ntrailer.append(0)
                for s in strayout:
                    for nfiber in range(1,13):
                        #if memorystatus[bundle-1][nfiber-1] is False:
                            fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                            if fiber:
                                header=re.search("TBM_H",s)
                                trailer=re.search("TBM_T",s)
                                if header:
                                    nheader[nfiber-1] = nheader[nfiber-1]+1
                                if trailer:
                                    ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                         
                for i in range(0,12):
                        if nheader[i]==2 and ntrailer[i]==2 :
                            memory[bundle-1][i]=memory[bundle-1][i]+1
    ###############################################################################                         
            for k in memory:
                log.ok("BUNDLE "+ str(memory.index(k)+1)) 
                for i in range(0,12):
                    value=100*memory[memory.index(k)][i]/ntrigger1
                    message= "fiber "+ str(i+1) +" efficiency "+str(value)+ " %, "+ str(memory[memory.index(k)][i])+ "/"+str(ntrigger1)
                    print message
                    if memory[memory.index(k)][i]>globalmemory[memory.index(k)][i]:
                        globalmemory[memory.index(k)][i]=memory[memory.index(k)][i]
                        value=100*globalmemory[memory.index(k)][i]/ntrigger1
                        if value>95:
                            memorystatus[memory.index(k)][i]=True

            rep=rep+1
            print globalmemory[bundle-1]
            print all(memorystatus[bundle-1])
            goon=all(memorystatus[bundle-1])
    print "final answer is:"
    for k in globalmemory:
        log.ok("BUNDLE "+ str(globalmemory.index(k)+1)) 
        for i in range(0,12):
            value=100*globalmemory[globalmemory.index(k)][i]/ntrigger1
            message= "fiber "+ str(i+1) +" efficiency "+str(value)+ " %, "+ str(globalmemory[globalmemory.index(k)][i])+ "/"+str(ntrigger1)
            if value>95:
                log.ok(message)
            if value<96:
                log.error(message)  
    print "final answer is: end"
    timer.append(" Data trasfer test with "+str(ntrigger1)+" trigger finished")
    timer.append(time.time()-start_time)  
    
########################################################
if datatrasfertest_repeat_S==True:
    testname="Starting Data trasfer test with "+str(ntrigger1)+" trigger"
    log.info(testname)
    timer.append("Starting Data trasfer test with "+str(ntrigger1)+" trigger")
    timer.append(time.time()-start_time)

    
    
    sector.init_tube()
    digfed.send("setupTTC").readlines()
    digfed.send("PiggyS").readlines()
    digfed.send("initFitelS").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    for n in range(0,len(module)):
        ######################################################
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 50").readlines()
        ccu.send("delay25 set d2 50" ).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        ######################################################
        output="cn " + moduleg[n]
        pxfec.send(output).readlines()
        pxfec.send("module " + str(module[n])).readlines()
        pxfec.send("tbm disable triggers").readlines()
        sleep(0.1)
    ###############################################################################
    nht = 0
    memory=[]
    memorystatus=[]
    globalmemory=[]
    ################################
    for k in sublist:
        bundle=sublist.index(k)+1
        # name="bundle_"+str(sector.poh[n[0]].bundle)
    #     canvas.append(TCanvas(name,name, 200, 10, 800, 800))
    #     name="bundle_noise"+str(sector.poh[n[0]].bundle)
    #     canvasnoise.append(TCanvas(name,name, 200, 10, 800, 800))
    #     print  "  Fiber bundle n.",sector.poh[n[0]].bundle, " in the analog fed"
    # ################################
    #for bundle in distinct_list:
        #print distinct_list
        print "Plug bundle "+ str(bundle) +" in Piggy South "
        rep = raw_input(' when ready hit return: ')
        memory.append([])
        globalmemory.append([])
        memorystatus.append([])
        ######################################################
        delay=27
        ######################################################
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            pxfec.send("cn "+moduleg[n]).readlines
            pxfec.send("module " + str(module[n])).readlines()
            pxfec.send("tbm").readlines()
            pxfec.send("tbmplldelay "+str(inputo)).readlines()
            pxfec.send("tbm disable triggers").readlines()
            print "module ", module[n],"Tbm pll delay:", inputo
        for i in range(0,12): 
            memory[bundle-1].append(0)
            memorystatus[bundle-1].append(False)
            globalmemory[bundle-1].append(0)
        digfed.send("initFitelS").readlines()
        rep=1
        tryhard=0
        goon=all(globalmemory[bundle-1])
        # success=False
        psuccess=[]
        for nfiber in range(0,12):psuccess.append(False)
        success=all(psuccess)
        while rep <3 and goon is False:
            tryhard=0
            psuccess=[]
            for nfiber in range(0,12):psuccess.append(False)
            success=all(psuccess)
            #############################"right mood section"
            while tryhard<3  and success==False :#and rep==1
                tryhard=tryhard+1
                success=all(psuccess)
                # difed.close()
                # digfed=SimpleSocket( 'localhost', dfedport)
                #print tryhard, psuccess, success, all(psuccess)
                sleep(1)

                print "Initializing Digital Fed, step ", tryhard
                digfed.send("reset").readlines()
                digfed.send("regreset").readlines()
                digfed.send("Pllreset").readlines()
                digfed.send("PLLrestPiggy").readlines()
                digfed.send("PiggyS").readlines()
                digfed.send("initFitelS").readlines()
                digfed.send("trigger 100").readlines()
                sleep(0.1)
                strayout= digfed.send("shazamS").readlines()
                for s in strayout:
                    for nfiber in range(1,13):
                        fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                        if fiber:
                            header=re.search("TBM_H",s)
                            if header:psuccess[nfiber-1]=True
            if success: print "Digital Fed initialization OK"
    
            #############################"right mood section"
            for i in range(0,12):memory[bundle-1][i]=0
            print "iteration number:", rep
            for ntr in range(0,ntrigger1): 
                sys.stdout.write("\r############# " + str(100*ntr/ntrigger1)+" %")
                sys.stdout.flush()
                header=[]
                trailer=[]
                strayout=[]
                nheader=[]
                ntrailer=[]
                strayout= digfed.send("shazamS").readlines()
                for i in range(0,12):
                    nheader.append(0)
                    ntrailer.append(0)
                for s in strayout:
                    for nfiber in range(1,13):
                        #if memorystatus[bundle-1][nfiber-1] is False:
                            fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                            if fiber:
                                header=re.search("TBM_H",s)
                                trailer=re.search("TBM_T",s)
                                if header:
                                    nheader[nfiber-1] = nheader[nfiber-1]+1
                                if trailer:
                                    ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                         
                for i in range(0,12):
                        if nheader[i]==2 and ntrailer[i]==2 :
                            memory[bundle-1][i]=memory[bundle-1][i]+1
    ###############################################################################                         
            for k in memory:
                log.ok("BUNDLE "+ str(memory.index(k)+1)) 
                for i in range(0,12):
                    value=100*memory[memory.index(k)][i]/ntrigger1
                    message= "fiber "+ str(i+1) +" efficiency "+str(value)+ " %, "+ str(memory[memory.index(k)][i])+ "/"+str(ntrigger1)
                    print message
                    if memory[memory.index(k)][i]>globalmemory[memory.index(k)][i]:
                        globalmemory[memory.index(k)][i]=memory[memory.index(k)][i]
                        value=100*globalmemory[memory.index(k)][i]/ntrigger1
                        if value>95:
                            memorystatus[memory.index(k)][i]=True

            rep=rep+1
            print globalmemory[bundle-1]
            print all(memorystatus[bundle-1])
            goon=all(memorystatus[bundle-1])
    print "final answer is:"
    for k in globalmemory:
        log.ok("BUNDLE "+ str(globalmemory.index(k)+1)) 
        for i in range(0,12):
            value=100*globalmemory[globalmemory.index(k)][i]/ntrigger1
            message= "fiber "+ str(i+1) +" efficiency "+str(value)+ " %, "+ str(globalmemory[globalmemory.index(k)][i])+ "/"+str(ntrigger1)
            if value>95:
                log.ok(message)
            if value<96:
                log.error(message)  
    print "final answer is: end"
    timer.append(" Data trasfer test with "+str(ntrigger1)+" trigger finished")
    timer.append(time.time()-start_time)  
    
########################################################


########################################################
if datatrasfertest_poh==True:
    sector.init_tube()
    whichbundle=[]
    memory=[]
    histo=[]
    canvas=[]
    initmod()
    for n in sublist:
        name="bundle_"+str(sector.poh[n[0]].bundle)
        canvas.append(TCanvas(name,name, 200, 10, 800, 800))
        print  "  Fiber bundle n.",sector.poh[n[0]].bundle, " in the digital fed"
        rep = raw_input(' when ready hit return: ')
        for i in n:
            number=sublist.index(n)
            
        whichbundle.append(sector.poh[n[0]].bundle)
        memory.append([])
        histo.append([])
        for i in range(1,13):       
            name= "Fiber_"+str(i)+"_bundle_"+str(sector.poh[n[0]].bundle)
            histo[number].append(TH1D(name,name,32,-0.5,15.5))
        ######################################################
        delay=18
        ######################################################
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            ccu.send("group "+ moduleg[n]).readlines()
            ccu.send("delay25 set d1 50").readlines()
            ccu.send("delay25 set d2 50").readlines()
            pxfec.send("cn "+moduleg[n]).readlines()
            pxfec.send("module " + str(module[n])).readlines()
            pxfec.send("tbm").readlines()
            pxfec.send("tbm disable triggers").readlines()
            pxfec.send("tbmplldelay "+str(inputo)).readlines()
            print "inputo", inputo
        nheader=[]
        ntrailer=[]
        nroc=[]
        for i in range(0,13): memory[number].append(0)
        digfed.send("initFitelN").readlines()
        for ntr in range(0,ntrigger1): 
            sys.stdout.write("\r############# " + str(100*ntr/ntrigger1)+" %")
            sys.stdout.flush()
            n=False
            t=False
            r=False
            header=[]
            trailer=[]
            roc=[]
            strayout=[]
            dummyfiber=[]
            nheader=[]
            ntrailer=[]
            nroc=[]
            strayout= digfed.send("shazamN").readlines()
            for i in range(0,12):
                dummyfiber.append(i+1)
                nheader.append(0)
                ntrailer.append(0)
                nroc.append(0)
         
            for s in strayout:
                for nfiber in range(1,13):
                    fiber=re.search("FIBER "+ str(nfiber)+" ",s)
                    if fiber:
                        header=re.search("TBM_H",s)
                        trailer=re.search("TBM_T",s)
                        if header:
                            nheader[nfiber-1] = nheader[nfiber-1]+1
                        if trailer:
                            ntrailer[nfiber-1] = ntrailer[nfiber-1]+1
                         
            for i in range(0,12):
                if nheader[i]==2 and ntrailer[i]==2 : 
                    memory[number][i]=memory[number][i]+1
            for i in range(0,12):
                print "Fiber ", i+1,"eff.",memory[number][i], "/99"
    ###############################################################################
    ll=0
    for k in range(0,len(histo)):
        canvas[k].cd()
        for i in range(0,len(histo[k])):
           histo[k][i].SetFillColor(i+1)
           histo[k][i].Fill(i,memory[k][i])
           norm=100/ntrigger1
           histo[k][i].Scale(norm)
           histo[k][i].SetMaximum(110.0)
           histo[k][i].SetMinimum(0)
           histo[k][i].Draw("same")
        canvas[k].Print(filename)
    #c2.Update()
                                         
    for k in range(0,len(histo)):
        #log.info(canvas[k].GetName()) 
        log.info(whichbundle[k])
        for i in range(0,len(histo[k])):
            value=100*memory[k][i]/ntrigger1
            message= "fiber "+ str(i+1) +" efficiency "+str( 100*memory[k][i]/ntrigger1)+ " %, "+ str(memory[k][i])+ "/"+str(ntrigger1)
            if value> 90:
                log.ok(message)
            else:
                log.error(message)
                

    timer.append(" Data trasfer test with "+str(ntrigger1)+" trigger finished")
    timer.append(time.time()-start_time)  
    # for k in range(0,len(histo)):
    #     for i in histo[k]:
    #         i.Write()
    #         i.Print(filename)
    rep = raw_input(' when ready hit return: ')
    # c2.Print(filename)
    # c2.Write()
    # c2.Close()
########################################################



########################################################
if datatrasfertest_piggy_S==True:
    testname="Starting Data trasfer test with "+str(ntrigger1)+" trigger"
    log.info(testname)
    timer.append("Starting Data trasfer test with "+str(ntrigger1)+" trigger")
    timer.append(time.time()-start_time)
    c2= TCanvas()
    sector.init_tube()
    print "header trailer"
    digfed.send("setupTTC").readlines()
    digfed.send("piggyS").readlines()
    digfed.send("initFitelS").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0
    memox=0
    memoy=0
    histo=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    print  " STOP TRIGGER"
    digfed.send("setupTTC").readlines()
    initmod()
    for n in range(0,len(module)):
        ######################################################
        output="group " + moduleg[n]
        ccu.send(output).readlines()
        ccu.send("delay25 set d0 0").readlines()
        ccu.send("delay25 set d1 "+str(d1[n])).readlines()
        ccu.send("delay25 set d2 "+str(d2[n])).readlines()
        ccu.send("delay25 set d3 0").readlines()
        ccu.send("delay25 set d4 0").readlines()
        ######################################################
        output="cn " + moduleg[n]
        pxfec.send(output).readlines()
        pxfec.send("module " + str(module[n])).readlines()
        pxfec.send("tbm disable triggers").readlines()
    ###############################################################################
    memory=[]
    for bundle in distinct_list:
        print "Plug bundle "+ str(bundle) +" in Piggy South "
        #rep = raw_input(' when ready hit return: ')
        memory.append(0)
        ######################################################
        delay=18
        ######################################################
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        for n in range(0,len(module)):
            pxfec.send("cn "+moduleg[n]).readlines
            pxfec.send("module " + str(module[n])).readlines()
            pxfec.send("tbm disable triggers " ).readlines()
            pxfec.send("tbm").readlines()
            pxfec.send("tbmplldelay "+str(inputo)).readlines()
            print "inputo", inputo
        nheader=[]
        ntrailer=[]
        nroc=[]
        digfed.send("initFitelS").readlines()

        for fiber in range(1,13):
            digfed.send("shazamS").readlines()
            memory[bundle-1]=0
            digfed.send("fiber "+ str(fiber)).readlines()
            for ntr in range(0,ntrigger1): 
                n=False
                t=False
                r=False
                header=False
                trailer=False
                roc=False
                strayout=[]
                nheader=0
                ntrailer=0
                nroc=0
                digfed.send("reset").readlines()
                digfed.send("regreset").readlines()
                sleep(0.1)
                strayout= digfed.send("getFromPiggy").readlines()
                sleep(0.1)
                for s in strayout:
                    header=re.search("011111111100 : TBM_Header",s)
                    trailer=re.search("011111111110 : TBM Trailer",s)
                    for r_n in range(1,5):
                        roc_s="ROC#"+str(r_n) 
                        roc=re.search(roc_s,s) 
                        if roc: 
                            nroc=nroc+1
                    if header:
                        nheader = nheader+1
                    if trailer:
                        ntrailer = ntrailer+1
                         
                if nheader==2 and ntrailer==2 : #and nroc[i]==8:
                    memory[bundle-1]=memory[bundle-1]+1
                sys.stdout.write("\r############# " + str(100*ntr/ntrigger1)+" %")
                sys.stdout.flush()
    ###############################################################################
            print "fiber", fiber, "HT",    memory[bundle-1], "/", ntrigger1
    # for k in range(0,len(histo)):
    #     log.ok("BUNDLE "+ str(k)) 
    #     for i in range(0,len(histo[k])):
    #         value=100*memory[k][i]/ntrigger1
    #         message= "fiber "+ str(i+1) +" efficiency "+str( 100*memory[k][i]/ntrigger1)+ " %, "+ str(memory[k][i])+ "/"+str(ntrigger1)
    #         if value> 90:
    #             log.ok(message)
    #         else:
    #             log.error(message)
                

    timer.append(" Data trasfer test with "+str(ntrigger1)+" trigger finished")
    timer.append(time.time()-start_time)  
    
    rep = raw_input(' when ready hit return: ')
    c2.Print(filename)
    c2.Write()
    c2.Close()
########################################################
########################################################
if tbmplldelayonetest==True:
    copen.Divide(2)
    sector.init_tube()
    print "header trailer"
    print  " fiber bundle in the digital fed, start trigger"
    rep = raw_input(' when ready hit return: ')
    digfed.send("setupTTC").readlines()
    digfed.send("initFitelN").readlines()
    digfed.send("fitel").readlines()
    digfed.send("reset").readlines()
    digfed.send("regreset").readlines()
    n=0

    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    for i in module:
        sendd1 = "delay25 set d1 " + str(d1[n])
        sendd2 = "delay25 set d2 " + str(d2[n])
        output=ccu.send(sendd1).readlines() #SDA d2 x
        print output
        output=ccu.send(sendd2).readlines()  #RDA d1 y
        print output
        output=pxfec.send("cn scan hubs").readlines()
        print"hope is ", str(i)
        print output
        pxfec.send("module " + str(i)).readlines()
        pxfec.send("tbm enable triggers").readlines()
        n=n+1

    
    histoch=[]
    
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    
    print  " STOP TRIGGER"
    rep = raw_input(' when ready hit return: ')
    digfed.send("setupTTC").readlines()


    
    for n in range(0,len(module)):
     sendd1 = "delay25 set d1 " + str(d1[n])
     sendd2 = "delay25 set d2 " + str(d2[n])
     output=ccu.send(sendd1).readlines() #SDA d2 x
     output=ccu.send(sendd2).readlines()  #RDA d1 y
     initmod()
     output=pxfec.send("cn scan hubs").readlines()
     pxfec.send("module " + str(module[n])).readlines()
     pxfec.send("tbm enable triggers").readlines()
       
###################sublist_m_fiber
    blunde=0
    counter=-1
    for fiber_on_bundle in sublist_m_fiber:
        counter=counter+1
        print "fiber bundle "+ str(blunde +1) + " in piggy nord Digital Fed"
        blunde=blunde+1
        rep = raw_input(' when ready hit return: ')
        for i in fiber_on_bundle: 
         for fb in i:
             
             print "module ",sublist_m[blunde-1][counter], " fiber ", fb, "bundle",str(blunde)
             name= "Fedch"+str(fb) +"bundle_"+str(blunde+1)
             histoch.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
             out ="fiber " + str(fb)
             digfed.send(out).readlines()
             ## send delay
             print "inputo " +str(inputo[n])
             pxfec.send("tbmplldelay "+ str(inputo[n])).readlines()
             nheader=0
             ntrailer=0
             nroc=0
             finalvalue=0
             for ntr in range(0,ntrigger): 
                     n=False
                     t=False
                     r=False
                     nheader=0
                     ntrailer=0
                     nroc=0
                     header=0
                     trailer=0
                     roc=0
                     strayout=0
                     strayout= digfed.send("TBMA").readlines()
                     strayout= digfed.send("getFromFifo1").readlines()
                     for s in strayout:
                         header=re.match("TBM_H",s)
                         if header:
                             nheader = nheader+1
                         trailer=re.match("TBM_T",s)
                         for r_n in range(1,5):
                             roc_s="  ROC #"+str(r_n) +" "
                             roc=re.match(roc_s,s) 
                             if roc: break
                         if trailer:
                             ntrailer = ntrailer+1
                         if roc:
                             nroc = nroc+1
                     value=nheader+ntrailer+nroc
                     value1=nheader+ntrailer+nroc
                     header=0
                     trailer=0
                     roc=0
                     nheader=0
                     ntrailer=0
                     nroc=0
                     strayout=0
                     
                     strayout= digfed.send("TBMB").readlines()
                     strayout= digfed.send("getFromFifo1").readlines()
                     for s in strayout:
                         header=re.match("TBM_H",s)
                         if header:
                             nheader = nheader+1
                             
                         trailer=re.match("TBM_T",s)
                         for r_n in range(1,5):
                             roc_s="  ROC #"+str(r_n) +" "
                             roc=re.match(roc_s,s)
                             if roc: break
                         if trailer:
                             ntrailer = ntrailer+1
                         if roc:
                             nroc = nroc+1
                     value=value+nheader+ntrailer+nroc
                     value2=nheader+ntrailer+nroc
                     #print value
                     #print "TBMA", value1, "TBMB", value2
                     value1=0
                     value2=0
                     if value==12:
                         finalvalue=finalvalue+1
             print "module ",module[n], "eff. on ",ntrigger, "trigger", 100*finalvalue/ntrigger, "%"
             
                     
             
                 
                 
         
                 
               
          
    timer.append("tbmplldelay scan done")
    timer.append(time.time()-start_time)  
    print "done"
   ########################################################
########################################################
if test_find_tbm_pll is True:
    print sublist_m_fiber
    copen.Divide(2)
    sector.init_tube()
    print "header trailer"
    print  " fiber bundle in the digital fed, start trigger"
    rep = raw_input(' when ready hit return: ')
    n=0
    for k in sector.pohlist:
        ccu.send("group " + sector.poh[k].group).readlines()
        out= sector.poh[k].name +" init"
        a =ccu.send(out).readlines()
    for i in module:
        sendd1 = "delay25 set d1 " + str(d1[n])
        sendd2 = "delay25 set d2 " + str(d2[n])
        output=ccu.send(sendd1).readlines() #SDA d2 x
        print output
        output=ccu.send(sendd2).readlines()  #RDA d1 y
        print output
        output=pxfec.send("cn scan hubs").readlines()
        print"hope is ", str(i)
        print output
        pxfec.send("module " + str(i)).readlines()
        pxfec.send("tbm enable triggers").readlines()
        n=n+1

    
    histoch=[] 
    name=[]
    nheader = 0
    ntrailer = 0
    nht = 0
    g=3
    b=40
    ss=[]
    print  " STOP TRIGGER"
    rep = raw_input(' when ready hit return: ')
    for n in range(0,len(module)):
     sendd1 = "delay25 set d1 " + str(d1[n])
     sendd2 = "delay25 set d2 " + str(d2[n])
     output=ccu.send(sendd1).readlines() #SDA d2 x
     output=ccu.send(sendd2).readlines()  #RDA d1 y
     initmod() 
     output=pxfec.send("cn scan hubs").readlines()
     pxfec.send("module " + str(module[n])).readlines()
     pxfec.send("tbm enable triggers").readlines()
       
###################sublist_m_fiber
    enb_ttc=0
    blunde=0
    counter=-1
    for fiber_on_bundle in sublist_m_fiber:

        counter=counter+1
        blunde =blunde+1 # +1
        for i in  fiber_on_bundle:# fiber_on_bundle[1:]:
         for fb in i:
             print "module ",sublist_m[blunde-1][counter], " fiber ", fb, "bundle",str(blunde)
            
             name= "fiber"+str(fb) +"bundle_"+str(blunde)
             histoch.append(TH2D(name,name,8,-0.5,7.5,8,-0.5,7.5))
             out ="fiber " + str(fb)
             if blunde==1:
                 
                 digfed.send("initFitelN").readlines()
                 digfed.send("fitel").readlines()
                 digfed.send("reset").readlines()
                 digfed.send("regreset").readlines()
                 digfed.send("piggyN").readlines()
                 digfed.send(out).readlines()
                 print "Piggy North Bundle",blunde
             if blunde==2:

                 digfed.send("initFitelN").readlines()
                 digfed.send("fitel").readlines()
                 digfed.send("reset").readlines()
                 digfed.send("regreset").readlines()
                 digfed.send("piggyN").readlines()
                 digfed.send(out).readlines()
                 print "Piggy South Bundle",blunde
             if enb_ttc==0:
                 digfed.send("setupTTC").readlines() 
                 enb_ttc=1
             for delay in range(0,64):
                 inputo= str(bin(delay))+str(11)
                 inputo= int(inputo,2)
          
                 print "inputo " +str(inputo)
                 pxfec.send("tbmplldelay "+ str(inputo)).readlines()
                 nheader=0
                 ntrailer=0
                 nroc=0
                 bdelay=bin(delay)
                 if len(bdelay)>5:
                     x=bdelay[-3:] # (last 3 elements)
                     y=bdelay[:-3] # (first 3 elements)

                 if len(bdelay) <=5:
                     x=bdelay
                     y=bin(0)
                     cut=[]
                 for ntr in range(0,ntrigger): 
                     n=False
                     t=False
                     r=False
                     nheader=0
                     ntrailer=0
                     nroc=0
                     header=0
                     trailer=0
                     roc=0
                     strayout=0
                     strayout= digfed.send("TBMA").readlines()
                     strayout= digfed.send("getFromFifo1").readlines()
                     for s in strayout:
                         header=re.match("TBM_H",s)
                         if header:
                             nheader = nheader+1
                         trailer=re.match("TBM_T",s)
                         for r_n in range(1,5):
                             roc_s="  ROC #"+str(r_n) +" "
                             roc=re.match(roc_s,s) 
                             if roc: break
                         if trailer:
                             ntrailer = ntrailer+1
                         if roc:
                             nroc = nroc+1
                     value=nheader+ntrailer+nroc
                     value1=nheader+ntrailer+nroc
                     header=0
                     trailer=0
                     roc=0
                     nheader=0
                     ntrailer=0
                     nroc=0
                     strayout=0
                     
                     strayout= digfed.send("TBMB").readlines()
                     strayout= digfed.send("getFromFifo1").readlines()
                     for s in strayout:
                         header=re.match("TBM_H",s)
                         if header:
                             nheader = nheader+1
                             
                         trailer=re.match("TBM_T",s)
                         for r_n in range(1,5):
                             roc_s="  ROC #"+str(r_n) +" "
                             roc=re.match(roc_s,s)
                             if roc: break
                         if trailer:
                             ntrailer = ntrailer+1
                         if roc:
                             nroc = nroc+1
                     value=value+nheader+ntrailer+nroc
                     value2=nheader+ntrailer+nroc
                     print value
                     print "TBMA", value1, "TBMB", value2
                     value1=0
                     value2=0
                     if value==12:
                         for hist in histoch:
                             if hist.GetName()== "fiber"+str(fb) +"bundle_"+str(blunde):
                                 hist.Fill(float(int(x,2)),float(int(y,2)),float(value/12))
                                 if fb==5:copen.cd(1)
                                 if fb==6:copen.cd(2)
                                 hist.Draw("COLZ TEXT")
                                 copen.Update()
             
                 
                 
         
                 
               
          
    timer.append("tbmplldelay scan done")
    timer.append(time.time()-start_time)  
    print "done"

    k=0
    c2=TCanvas('c2', 'c2', 200, 10, 800, 800)
   
    c2.cd()
    c2.Divide(int(len(histoch)/2),2)
    for i in histoch:
        c2.cd(k+1)
        i.Draw("COLZ TEXT")
        i.Write()
        k=k+1
    c2.Update()
    c2.Print(filename)
    c2.Write()
   

    #digfed.close()
    rep = raw_input(' when ready hit return: ')
########################################################
if test_poh_byhand== True:
    goon=True
    while goon is True:
        for bias in range(0,70,10):
            for i in sector.pohlist:
                out ="group "+sector.poh[i].group
                ccu.send(out).readlines()
                out = sector.poh[i].name + " setall 3 3 3 3 "+str(bias)+" "+str(bias)+" "+str(bias)+" "+str(bias)
                
                ccu.send(out).readlines() 
            print "##################bias value:",bias
            rep = raw_input(' when ready hit return: ')
            if rep =="exit":goon=False





if lazy_print == True:
 histo=(TH2D("Lazy","Lazy",8,-0.5,7.5,8,-0.5,7.5))
 for delay in range(0,64):
        inputo= str(bin(delay))+str(11)
        inputo= int(inputo,2)
        # output="tbm tbmplldelay "+ str(inputo)
        bdelay=bin(delay)
        if len(bdelay)>5:
            x=bdelay[-3:] # (last 3 elements)
            y=bdelay[:-3] # (first 3 elements)

        if len(bdelay) <=5:
            x=bdelay
            y=bin(0)
            cut=[]
        histo.Fill(float(int(x,2)),float(int(y,2)),inputo)
 copen.cd()
 histo.Draw("text")
 rep = raw_input(' when ready hit return: ')

########################################################
########################################################
for i in range(0,len(timer)):
    
    print timer[i], " ",
    if i%2==1:
        print "\n"
ccu.close()
pxfec.close()
cclose= TCanvas( 'cclose','cclose', 200, 10, 800, 800 )
cclose.Print(filename+")")
log.printLog()
log.saveLog("log/testCCU")
print "this is the end"
