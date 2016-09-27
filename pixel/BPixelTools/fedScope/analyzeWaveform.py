import ROOT
from ROOT import *
from array import array
import time,sys

inFile = TFile.Open("outData.root","READ")
tree = ROOT.TTree()
inFile.GetObject("tree",tree)

data = array('f',[0])
delay = array('i',[0])
tree.SetBranchAddress("data",data)
tree.SetBranchAddress("delay",delay)

graphs = []
for d in range(0,16):
 x = array('d',[])
 y = array('d',[])
 for e in range(tree.GetEntries()):
  tree.GetEntry(e)
  if delay[0] == d:
   x.append(delay[0]) 
   y.append(data[0])
 print len(x)
 gr = ROOT.TGraph(len(x),x,y) 
 graphs.append(gr)

'''
graphs = []
for i in range(0,16):
 f = ROOT.TFile.Open("test_%i.root"%i)
 htmp = f.Get("hTest")
 x = array('d',[])
 y = array('d',[])
 for b in range(10000):
  x.append(i)
  y.append(htmp.GetBinContent(b+1))
 gr = ROOT.TGraph(len(x),x,y)
 graphs.append(gr)
'''

c = ROOT.TCanvas("c")
c.cd()
mg = ROOT.TMultiGraph("mg","mg")
for g in graphs:
 g.SetMarkerStyle(20)
 mg.Add(g)

mg.Draw("AP")
time.sleep(1000)

