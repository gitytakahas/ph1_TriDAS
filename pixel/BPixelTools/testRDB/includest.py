from SystemTests import *
#from Logger import Logger
#from SimpleSocket import SimpleSocket
#from pohgraph import suppress_stdout,graph,getFilename,showRMSPOH
#log=Logger()
######################################################## port
# port

fechost = 'localhost'      
pxfechost = 'localhost' 
caenhost ='localhost'
fedhost = 'localhost'   
fecport =  2001  
fedport= 2004  
dfedport= 2006            
pxfecport= 2000              
######################################################## port
######################################################## fec
caen=4
ccu=SimpleSocket( fechost, fecport)
pxfec=SimpleSocket(fechost, pxfecport)
fed=SimpleSocket(fedhost, fedport)
digfed=SimpleSocket( 'localhost', dfedport)
######################################################## fec
######################################################## SECTOR NAME
name= "+6P"
sector= SECTOR(name,fed,ccu,pxfec,caen,log)
######################################################## SECTOR NAME
######################################################## Pohs, modules and bundle
#sector.pohlist= range(7,10)
# sector.poh[1] =POH(fed, ccu, name+"L12","poh2", 1,[None, 1, 2, 3, 4],log)
# sector.poh[2] =POH(fed, ccu, name+"L12","poh4", 1,[None, 5, 6, 7, 8] ,log)
# sector.poh[3] =POH(fed, ccu, name+"L12","poh6", 1,[None, 9, 10, 11, 12],log)

sector.pohlist= range(1,4)
sector.poh[1] =POH(fed, ccu, name+"L12","poh1", 1,[None, 9, 10, 11, 12],log)
sector.poh[2] =POH(fed, ccu, name+"L12","poh2", 1,[None, 5, 6, 7, 8] ,log)
sector.poh[3] =POH(fed, ccu, name+"L34","poh1", 1,[None, 1, 2, 3, 4],log)

sector.poh[4] =POH(fed, ccu, name+"L12","poh3", 2,[None, 9, 10, 11, 12],log)
sector.poh[5] =POH(fed, ccu, name+"L34","poh2", 2,[None, 5, 6, 7, 8] ,log)
sector.poh[6] =POH(fed, ccu, name+"L12","poh4", 2,[None, 1, 2, 3, 4],log)

sector.poh[7] =POH(fed, ccu, name+"L12","poh5", 3,[None, 9, 10, 11, 12],log)
sector.poh[8] =POH(fed, ccu, name+"L34","poh3", 3,[None, 5, 6, 7, 8] ,log)
sector.poh[9] =POH(fed, ccu, name+"L12","poh6", 3,[None, 1, 2, 3, 4],log)

sector.poh[10] =POH(fed, ccu, name+"L34","poh4", 4,[None,9, 10, 11, 12],log)
sector.poh[11] =POH(fed, ccu, name+"L34","poh5", 4,[None, 5, 6, 7, 8] ,log)
sector.poh[12] =POH(fed, ccu, name+"L12","poh7", 4,[None, 1, 2, 3, 4],log)

######################################################## Pohs, modules and bundle
# sector.pohlist= range(1,10) 
# sector.poh[1] =POH(fed, ccu, name+"L12","poh1", 1,[None, 9, 10, 11, 12],log)
# sector.poh[2] =POH(fed, ccu, name+"L12","poh2", 1,[None, 5, 6, 7, 8] ,log)

# sector.poh[3] =POH(fed, ccu, name+"L34","poh1", 2,[None, 9, 10, 11, 12],log)
# sector.poh[4] =POH(fed, ccu, name+"L12","poh3", 2,[None, 5, 6, 7, 8] ,log)

# sector.poh[5] =POH(fed, ccu, name+"L34","poh2", 3,[None, 9, 10, 11, 12],log)
# sector.poh[6] =POH(fed, ccu, name+"L12","poh4", 3,[None, 5, 6, 7, 8] ,log)

# sector.poh[7] =POH(fed, ccu, name+"L12","poh5", 4,[None, 9, 10, 11, 12],log)
# sector.poh[8] =POH(fed, ccu, name+"L34","poh3", 4,[None, 5, 6, 7, 8] ,log)

# sector.poh[9] =POH(fed, ccu, name+"L12","poh6", 5,[None, 9, 10, 11, 12],log)

# sector.poh[10] =POH(fed, ccu, name+"L34","poh4", 6,[None, 9, 10, 11, 12],log)


########################################################
# sector.poh[13] =POH(fed, ccu, name+"L34","poh6", 4,[None, 5, 6, 7, 8] ,log)
# sector.poh[14] =POH(fed, ccu, name+"L34","poh7", 4,[None, 1, 2, 3, 4],log)
# sector.pohlist= range(2,3)+range(6,7)+range(9,10)
# sector.poh[2] =POH(fed, ccu, name+"L12","poh2", 1,[None, 1, 2, 3, 4] ,log)
# sector.poh[6] =POH(fed, ccu, name+"L12","poh4", 1,[None, 5, 6, 7, 8],log)
# sector.poh[9] =POH(fed, ccu, name+"L12","poh6", 1,[None, 9, 10, 11, 12],log)
########################################################
#modulelayer 1,2 3 4
module=[1,3,23,15,26,28,30,31]#,31,24]
d1=[]
d2=[]
moduleg=[]
mbundle=[]
fiber=[]
#moduleg.append(name+"L12")
#moduleg.append(name+"L34")
dummyfiber=[i for i in range(1,13)]
for i in module:
    moduleg.append(name+"L34")
    d1.append(40) # RDA
    d2.append(50) # SDA
    #moduleg.append(name+"L34")  ,name+"L34",name+"L34"] 
    mbundle.append(1)
    fiber.append(dummyfiber)
#fiber=[[1,2,3,4],[1,2],[3,4]]
#d1=[24]#,24,24] # RDA
#d2=[50]#,50,50] # SDA
#mbundle=[1]#,1,1] # module bundle distribution
#moduleg=[name+"L12"]#,name+"L34",name+"L34"]
######################################################## Pohs, modules and bundle
######################################################## Function
sendata=False
M15and23=False
only23= False 
only15= False
#############################
redundancytest=True 
resettest=True 
inittest= True
resetandverifynocrosstalk=True
dcdctest=False
pohplusfechannel=False
pohplusfechannel_repeat=False 
testonefiber=False
testonefibervisual=False
testsdarda= False #check range "rdasdascanrange" to check wich group is tested
testtrigger= False
testpiggyN= False #TBMPLL delay scan (roc disabled) reading from fifo1 number of trigger= ntrigger
testpiggyS= False #TBMPLL delay scan (roc disabled) reading from fifo1 number of trigger= ntrigger
testroc=False  #roc delay scan reading from fifo1 number of trigger= ntrigger fixed value for TBMPLL delay
datatrasfertest_repeat_N=False
datatrasfertest_repeat_S=False #DATA TRANSFER TEST (roc disabled) n of trigger= ntrigger1 fixed value for TBMPLL delay, read wathever is plugged in PiggyS
digital_bias_scan=False
#############################
# note datatrasfertest_repeat_S work fine, there are a series of internal step that should ensure a really high reliability
#############################
datatrasfertest_poh=False
datatrasfertest_piggy_S=False
#datatrasfertest_piggy_N=False
datatrasfertest=False
testvanaiana=False # Obsolete
fedchtest=  False # Obsolete
pohbiastest= False # Obsolete
testpiggyN_channel=False
tbmplldelaytest=  False 
tbmplldelayonetest=False
test_find_tbm_pll=False
test_poh_byhand=False
lazy_print=False
######################################################## Function
rdasdascanrange=range(1,2) ###0= L12 1=L34
ntrigger=100  # tbmpll delay
ntrigger1=100 # used for the data trasmission test
slopethr=10   # threshold for the pohbias scan slope
########################################################




