#include "PixelCalibrations/include/PixelIanaAnalysis.h"

#include <cassert>
#include <fstream>

#include "TAxis.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLine.h"

//////////////////////////////////////////////////////////////////////////////////
void PixelIanaAnalysis::goIana(const std::string& roc,
			   const double oldVana_,
			   const int npoints,
			   const std::vector<double>& x,
			   std::vector<double> y,
			   std::vector<double> ey,
			   std::ostream& out,
			   const char* filename)
{
  oldVana = oldVana_;

  /*f2 = new TF1("f2", "[0] + [1]*x");
  f2->SetParName(0,"offset");
  f2->SetParName(1,"slope");
  f2->SetParameter(0,5);
  f2->SetParameter(1,0.5);*/

  f2 = new TF1("f2", "[0] + [1]*x + [2]*x*x");
  f2->SetParName(0,"offset");
  f2->SetParName(1,"slope");
  f2->SetParName(2,"par2");
  f2->SetParameter(0,5);
  f2->SetParameter(1,0.5);
  f2->SetParameter(2,0.05);
      
  canvas = new TCanvas(roc.c_str(), roc.c_str(), 800, 600);
  gr = new TGraphErrors(npoints, &x[0], &y[0], 0, 0);
  fitstatus = gr->Fit("f2", "", "", 10, 200);
  fitChisquare = f2->GetChisquare()/f2->GetNDF();

  //double currentIanaValue = gr->Eval(oldVana);
  double currentIanaValue = f2->Eval(oldVana); 
  
  if( fitChisquare > 10 ){

   int niter = 10;
   int i = 1;
   while( fitChisquare > 10 && i < niter ){
    fitstatus = gr->Fit("f2", "", "", 10+10*i, 200);
    fitChisquare = f2->GetChisquare()/f2->GetNDF();
    i++;
   }

  }

  if( fitChisquare <= 10 ){

   currentIanaValue = f2->Eval(oldVana); 
   pass = true;
   if( currentIanaValue > 24 && currentIanaValue < 26 ){ pass = true; newVana=oldVana; newIana=currentIanaValue;}
   else{
    for( int p = 0; p <= 255; ++p ){
      double iana = f2->Eval(p);
      if( iana > 24.5 && iana < 25.5 ){
       newVana = p;
       newIana = iana;
      }      
    }
   }

  }
  else{ pass = false; }

  gr->SetLineColor(2);
  gr->SetLineWidth(4);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(20);
  gr->SetTitle(roc.c_str());
  gr->SetMinimum(0);
  gr->SetMaximum(y[npoints-1]+10);
  gr->GetXaxis()->SetTitle("Vana (DAC)");
  gr->GetYaxis()->SetTitle("Iana (mA)");
  gr->GetXaxis()->SetRangeUser(0,255);
  gr->Draw("AP");

  f2->Draw("same");
  f2->SetLineColor(1);
  f2->SetLineStyle(2);
  f2->SetLineWidth(2);

  TLine l1(oldVana, 0, oldVana, currentIanaValue);
  l1.SetLineColor(1);
  l1.Draw();
  TLine l2(gr->GetXaxis()->GetXmin(), currentIanaValue, oldVana, currentIanaValue);
  l2.SetLineColor(1);
  l2.Draw(); 

  TLine l3(newVana, 0, newVana, newIana);
  l3.SetLineColor(kRed);
  l3.Draw();
  TLine l4(gr->GetXaxis()->GetXmin(), newIana, newVana, newIana);
  l4.SetLineColor(kRed);
  l4.Draw(); 

  if (filename)
    canvas->SaveAs(filename);
  else
    canvas->Write();
}

//////////////////////////////////////////////////////////////////////////////////
void PixelIanaAnalysis::goVdig(const std::string& roc,
	                       const std::vector<double>& x)
{

  std::cout << "************* JENDEBUG::goVdig for roc " << roc << " " << x.size() << std::endl;
  /*double min=100000;
  double max=0;
  for( unsigned int v = 0; v < x.size(); v++ ){
   if( x[v] <= min ) min = x[v];
  }
  for( unsigned int v = 0; v < x.size(); v++ ){
   if( x[v] >= max ) max = x[v];
  }

  std::cout << "********** JENDEBUG::goVdig for roc " << roc << " min " << min << " max " << max << std::endl;*/
  /*canvas = new TCanvas(roc.c_str(), roc.c_str(), 800, 600);
  hVdig = new TH1F(roc.c_str(), roc.c_str(), 1000, 0, 10); 

  for( unsigned int v = 0; v < x.size(); v++ ) hVdig->Fill(x[v]);

  //VdigMean = hVdig->GetMean();
  //VdigRMS = hVdig->GetRMS();

  hVdig->Draw("HIST");
  hVdig->Write();*/

}

//////////////////////////////////////////////////////////////////////////////////
PixelIanaAnalysis::~PixelIanaAnalysis() {
  //delete f2;
  delete gr;
  delete canvas;
}

//////////////////////////////////////////////////////////////////////////////////
void PixelIanaAnalysis::redoFromDat(const char* fn, const std::string& roc, std::ostream& out, const char* filename) {
  std::ifstream in(fn);

  std::string this_roc;

  if (roc != "") {
    while (this_roc != roc && getline(in, this_roc))
      ;
    assert(!in.eof());
  }
  else
    in >> this_roc;

  int npoints;
  in >> npoints;

  std::vector<double> x(npoints), y(npoints), ey(npoints);
  std::vector<double>* d[3] = {&x,&y,&ey};

  for (int j = 0; j < 3; ++j) {
    for (int i = 0; i < npoints; ++i) {
      assert(!in.eof());
      in >> (*d[j])[i];
    }
  }

  double yvalatzero;
  in >> yvalatzero;

  double dummy;
  for (int j = 0; j < 5; ++j)
    in >> dummy;

  int oldVana;
  int newVana;
  in >> oldVana;
  in >> newVana;

  in.close();

  goIana(this_roc, newVana, npoints, x, y, ey, out, filename);
}
