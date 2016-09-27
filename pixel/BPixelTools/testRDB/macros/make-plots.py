import ROOT
from ROOT import *
import tdrstyle
import sys
from optparse import OptionParser
import time

gROOT.Reset()




def get_canvas(cname):

  H_ref = 600;
  W_ref = 700;
  W = W_ref
  H  = H_ref

  T = 0.08*H_ref
  B = 0.12*H_ref
  L = 0.12*W_ref
  R = 0.04*W_ref

  canvas = ROOT.TCanvas(cname,cname,W,H)
  canvas.SetFillColor(0)
  canvas.SetBorderMode(0)
  canvas.SetFrameFillStyle(0)
  canvas.SetFrameBorderMode(0)
  canvas.SetLeftMargin( L/W )
  canvas.SetRightMargin( R/W )
  canvas.SetTopMargin( T/H )
  canvas.SetBottomMargin( B/H )
  canvas.SetTickx()
  canvas.SetTicky()
  
  return canvas
hfile = TFile( 'hsimple.root', 'RECREATE', 'Demo ROOT file with histograms' )

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
   
if channel == -1 and gain == -1:
   print "Ooops you forgot to set channel or gain"
   sys.exit()
print "FIN1" 
##################################################################################
tdrstyle.setTDRStyle()

marker_style = [20,21,22,33]
colors = [210,ROOT.TColor.kBlue,ROOT.TColor.kBlack,ROOT.TColor.kRed]

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
##################################################################################      
mg_poh1 = ROOT.TMultiGraph("mg_poh1","mg_poh1")
print "FIN2"

l1 = ROOT.TLegend(0.68,0.23,0.92,0.48)
l1.SetBorderSize(0)
l1.SetFillStyle(0)
l1.SetTextSize(0.04)
l1.SetTextFont(42)
print "FIN3"
for i in range(4):
   mg_poh1.Add(g_poh1[i])
   l1.AddEntry(g_poh1[i],leg_entry[i],"LP")

c1 = get_canvas("c1")
c1.cd()
mg_poh1.Draw("ALP")
mg_poh1.GetXaxis().SetTitle("POH bias")
mg_poh1.GetXaxis().SetLabelSize(0.04)
mg_poh1.GetXaxis().SetTitleSize(0.045)
mg_poh1.GetXaxis().SetTitleOffset(1.2)
mg_poh1.GetYaxis().SetTitle("FED ADC")
mg_poh1.GetYaxis().SetLabelSize(0.04)
mg_poh1.GetYaxis().SetTitleOffset(1.2)
mg_poh1.GetYaxis().SetTitleSize(0.045)
mg_poh1.SetMinimum(0)
mg_poh1.SetMaximum(1024)
mg_poh1.GetXaxis().SetRangeUser(0.,80.)
l1.Draw()
hfile.Write()
# c1.Print(outname[0]+".pdf")
# c1.SaveAs(outname[0]+".pdf")  
# c1.Print("trial1.pdf")
# time.sleep(10)
print "FIN4"

# ##################################################################################      
# mg_poh2 = ROOT.TMultiGraph("mg_poh2","mg_poh2")

# l2 = ROOT.TLegend(0.68,0.23,0.92,0.48)
# l2.SetBorderSize(0)
# l2.SetFillStyle(0)
# l2.SetTextSize(0.04)
# l2.SetTextFont(42)
# print "FIN4"
# for i in range(4):
#    mg_poh2.Add(g_poh2[i])
#    # l2.AddEntry(g_poh2[i],leg_entry[i],"LP")

# print "FIN4"
# c2 = get_canvas("c2")
# c2.cd()
# mg_poh2.Draw("ALP")
# mg_poh2.GetXaxis().SetTitle("POH bias")
# mg_poh2.GetXaxis().SetLabelSize(0.04)
# mg_poh2.GetXaxis().SetTitleSize(0.045)
# mg_poh2.GetXaxis().SetTitleOffset(1.2)
# mg_poh2.GetYaxis().SetTitle("FED ADC")
# mg_poh2.GetYaxis().SetLabelSize(0.04)
# mg_poh2.GetYaxis().SetTitleOffset(1.2)
# mg_poh2.GetYaxis().SetTitleSize(0.045)
# mg_poh2.SetMinimum(0)
# mg_poh2.SetMaximum(1024)
# mg_poh2.GetXaxis().SetRangeUser(0.,80.)
# l2.Draw()
# # c2.Print(outname[1]+".pdf")
# # c2.SaveAs(outname[1]+".pdf")
# # c2.SaveAs(outname[1]+".pdf","pdf")

# #time.sleep(10)

# ##################################################################################      
# mg_poh3 = ROOT.TMultiGraph("mg_poh3","mg_poh3")
# print "FIN5"
# l3 = ROOT.TLegend(0.68,0.23,0.92,0.48)
# l3.SetBorderSize(0)
# l3.SetFillStyle(0)
# l3.SetTextSize(0.04)
# l3.SetTextFont(42)

# for i in range(4):
#    mg_poh3.Add(g_poh3[i])
#    l3.AddEntry(g_poh3[i],leg_entry[i],"LP")

# c3 = get_canvas("c3")
# c3.cd()
# mg_poh3.Draw("ALP")
# mg_poh3.GetXaxis().SetTitle("POH bias")
# mg_poh3.GetXaxis().SetLabelSize(0.04)
# mg_poh3.GetXaxis().SetTitleSize(0.045)
# mg_poh3.GetXaxis().SetTitleOffset(1.2)
# mg_poh3.GetYaxis().SetTitle("FED ADC")
# mg_poh3.GetYaxis().SetLabelSize(0.04)
# mg_poh3.GetYaxis().SetTitleOffset(1.2)
# mg_poh3.GetYaxis().SetTitleSize(0.045)
# mg_poh3.SetMinimum(0)
# mg_poh3.SetMaximum(1024)
# mg_poh3.GetXaxis().SetRangeUser(0.,80.)
# l3.Draw()
# # c3.Print(outname[3]+".pdf")
# # c3.SaveAs(outname[2]+".pdf")
# # c3.SaveAs(outname[2]+".pdf","pdf")
print "FIN6"
#time.sleep(10)

##################################################################################      

