import ROOT
from ROOT import *
import tdrstyle
import sys
from optparse import OptionParser
import time

gROOT.Reset()




parser = OptionParser()

parser.add_option('--channel',action="store",type="int",dest="channel",default=-1)
parser.add_option('--gain',action="store",type="int",dest="gain",default=-1)
parser.add_option('--plot',action="store",type="string",dest="plot",default="bias")

(opts, args) = parser.parse_args()

channel = opts.channel
gain = opts.gain
plot = opts.plot

poh_files = [ROOT.TFile.Open("poh_bias_scan_gain"+str(i)+".root","READ") for i in range(4)]
for f in poh_files:
   if not(f.IsOpen()):
      print "Oooops : the input files poh_bias_scan_gain#.root do not exist!"
      sys.exit()

g_poh1 = []
g_poh2 = []
g_poh3 = []
leg_entry = []
outname = []

#compare bias/rms/slope curves of different channels with same gain
if gain != -1: 
   g_poh1 = [ROOT.TGraph(poh_files[gain].Get(plot+"_5:"+str(i+1)+";1")) for i in range(4)]
   g_poh2 = [ROOT.TGraph(poh_files[gain].Get(plot+"_6:"+str(i+1)+";1")) for i in range(4)]
   g_poh3 = [ROOT.TGraph(poh_files[gain].Get(plot+"_7:"+str(i+1)+";1")) for i in range(4)]
   leg_entry = ["channel "+str(i) for i in range(4)]
   outname = ["poh1_4ch_"+plot+"_g"+str(gain),"poh2_4ch_"+plot+"_g"+str(gain),"poh3_4ch_"+plot+"_g"+str(gain)]

#compare bias/rms/slope curves with different gains for same channel
if channel != -1:
   g_poh1 = [ROOT.TGraph(poh_files[i].Get(plot+"_5:"+str(channel+1)+";1")) for i in range(4)]
   g_poh2 = [ROOT.TGraph(poh_files[i].Get(plot+"_6:"+str(channel+1)+";1")) for i in range(4)]
   g_poh3 = [ROOT.TGraph(poh_files[i].Get(plot+"_7:"+str(channel+1)+";1")) for i in range(4)]
   leg_entry = ["gain = "+str(i) for i in range(4)]
   outname = ["poh1_ch"+str(channel)+"_"+plot,"poh2_ch"+str(channel)+"_"+plot,"poh3_ch"+str(channel)+"_"+plot]
   outname1 = ["poh1 channel "+str(channel)+" "+plot,"poh2 channel "+str(channel)+" "+plot,"poh3 channel "+str(channel)+" "+plot]
if channel == -1 and gain == -1:
   print "Ooops you forgot to set channel or gain"
   sys.exit()
print "FIN1" 
##################################################################################
tdrstyle.setTDRStyle()
marker_style = [20,21,22,33]
colors = [210,ROOT.TColor.kBlue,ROOT.TColor.kBlack,ROOT.TColor.kRed]
##################################################################################

for i in range(4):
   g_poh1[i].SetMarkerStyle(marker_style[i])
   g_poh1[i].SetMarkerColor(colors[i])
   g_poh1[i].SetLineColor(colors[i])
   g_poh2[i].SetMarkerStyle(marker_style[i])
   g_poh2[i].SetMarkerColor(colors[i])
   g_poh2[i].SetLineColor(colors[i])
   g_poh3[i].SetMarkerStyle(marker_style[i])
   g_poh3[i].SetMarkerColor(colors[i])
   g_poh3[i].SetLineColor(colors[i])
##################################################################################
hfile = TFile( 'hsimple1.root', 'RECREATE', 'Demo ROOT file with histograms' )
##################################################################################     

#################################################################################
mg_poh1 = ROOT.TMultiGraph("mg_poh1","mg_poh1")

l1 = ROOT.TLegend(0.68,0.23,0.92,0.48)
l1.SetBorderSize(0)
l1.SetFillStyle(0)
l1.SetTextSize(0.04)
l1.SetTextFont(42)

for i in range(4):
   mg_poh1.Add(g_poh1[i])
   l1.AddEntry(g_poh1[i],leg_entry[i],"LP")
c1 = ROOT.TCanvas("c1","c1",800,800)
c1.cd()
c1.SetTitle(outname1[0])
mg_poh1.Draw("ALP")
l1.Draw()
c1.Update()
mg_poh1.Write()
c1.Print(outname[0]+".pdf")
#################################################################################
#################################################################################
mg_poh2 = ROOT.TMultiGraph("mg_poh2","mg_poh2")

l2 = ROOT.TLegend(0.68,0.23,0.92,0.48)
l2.SetBorderSize(0)
l2.SetFillStyle(0)
l2.SetTextSize(0.04)
l2.SetTextFont(42)


for i in range(4):
   mg_poh2.Add(g_poh2[i])
   l2.AddEntry(g_poh2[i],leg_entry[i],"LP")
c2 = ROOT.TCanvas("c2","c2",800,800)
c2.cd()
c2.SetTitle(outname1[1])
mg_poh2.Draw("ALP")
l2.Draw()
c2.Update()
mg_poh2.Write()
c2.Print(outname[1]+".pdf")
#################################################################################
#################################################################################
mg_poh3 = ROOT.TMultiGraph("mg_poh3","mg_poh3")

l3 = ROOT.TLegend(0.68,0.23,0.92,0.48)
l3.SetBorderSize(0)
l3.SetFillStyle(0)
l3.SetTextSize(0.04)
l3.SetTextFont(42)

for i in range(4):
   mg_poh3.Add(g_poh3[i])
   l3.AddEntry(g_poh3[i],leg_entry[i],"LP")
c3 = ROOT.TCanvas("c3","c3",800,800)
c3.cd()
c3.SetTitle(outname1[2])
mg_poh3.Draw("ALP")
l3.Draw()
c3.Update()
mg_poh3.Write()
c3.Print(outname[2]+".pdf")
#################################################################################
