/**
 * A collection of simple ROOT macros.
 *
 * N. Amapane, G. Cerminara, M. Dalchenko
 */

#include <sstream>
#include <iomanip>

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TProfile.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TVirtualPad.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TObject.h"
#include "TH1.h"
#include "TH1F.h"
#include <TFile.h>
#include "TPaveStats.h"
#include <math.h>

#include <iostream>

using namespace std;

#endif



TStyle * getStyle(TString name) 
{
    TStyle *theStyle;
    
    if ( name == "ZZ" ) {
        // Style from http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/pandolf/CommonTools/DrawBase.C?revision=1.25&view=markup
        theStyle = new TStyle("ZZStyle", "ZZ Style");
        theStyle->SetCanvasColor(0);
        theStyle->SetPadColor(0);
        theStyle->SetFrameFillColor(0);
        theStyle->SetStatColor(0);
        theStyle->SetOptStat(1111);
        theStyle->SetTitleFillColor(0);
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetPadBorderMode(0);
        theStyle->SetFrameBorderMode(0);
        theStyle->SetPadBottomMargin(0.12);
        theStyle->SetPadLeftMargin(0.12);
        theStyle->cd();
        
        // For the canvas:
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetCanvasColor(kWhite);
        theStyle->SetCanvasDefH(600); //Height of canvas
        theStyle->SetCanvasDefW(600); //Width of canvas
        theStyle->SetCanvasDefX(0);   //POsition on screen
        theStyle->SetCanvasDefY(0);
        
        // For the Pad:
        theStyle->SetPadBorderMode(0);
        theStyle->SetPadColor(kWhite);
        theStyle->SetPadGridX(false);
        theStyle->SetPadGridY(false);
        theStyle->SetGridColor(0);
        theStyle->SetGridStyle(3);
        theStyle->SetGridWidth(1);
        
        // For the frame:
        theStyle->SetFrameBorderMode(0);
        theStyle->SetFrameBorderSize(1);
        theStyle->SetFrameFillColor(0);
        theStyle->SetFrameFillStyle(0);
        theStyle->SetFrameLineColor(1);
        theStyle->SetFrameLineStyle(1);
        theStyle->SetFrameLineWidth(1);
        
        // Margins:
        theStyle->SetPadTopMargin(0.05);
        theStyle->SetPadBottomMargin(0.15);//0.13);
        theStyle->SetPadLeftMargin(0.2);//0.16);
        theStyle->SetPadRightMargin(0.15);//0.02);
        
        // For the Global title:
        theStyle->SetOptTitle(0);
        theStyle->SetTitleFont(42);
        theStyle->SetTitleColor(1);
        theStyle->SetTitleTextColor(1);
        theStyle->SetTitleFillColor(10);
        theStyle->SetTitleFontSize(0.05);
        
        // For the axis titles:
        theStyle->SetTitleColor(1, "XYZ");
        theStyle->SetTitleFont(42, "XYZ");
        theStyle->SetTitleSize(0.05, "XYZ");
        theStyle->SetTitleXOffset(1.25);//0.9);
        theStyle->SetTitleYOffset(1.65); // => 1.15 if exponents
        
        // For the axis labels:
        theStyle->SetLabelColor(1, "XYZ");
        theStyle->SetLabelFont(42, "XYZ");
        theStyle->SetLabelOffset(0.007, "XYZ");
        theStyle->SetLabelSize(0.045, "XYZ");
        
        // For the axis:
        theStyle->SetAxisColor(1, "XYZ");
        theStyle->SetStripDecimals(kTRUE);
        theStyle->SetTickLength(0.03, "XYZ");
        theStyle->SetNdivisions(10, "XYZ");
        theStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
        theStyle->SetPadTickY(1);
        TGaxis::SetMaxDigits(3);
    }else if ( name == "myStyle" ) {
        theStyle = new TStyle("myStyle", "myStyle");
        //    theStyle->SetOptStat(0);
        theStyle->SetPadBorderMode(0);
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetPadColor(0);
        theStyle->SetCanvasColor(0);
        theStyle->SetMarkerStyle(8);
        theStyle->SetMarkerSize(0.7);
        theStyle->SetStatH(0.3);
        theStyle->SetStatW(0.15);
        //   theStyle->SetTextFont(132);
        //   theStyle->SetTitleFont(132);
        theStyle->SetTitleBorderSize(1);
        theStyle->SetPalette(1);
        
    }else if( name == "tdr" ) {
        theStyle = new TStyle("tdrStyle","Style for P-TDR");
        
        // For the canvas:
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetCanvasColor(kWhite);
        theStyle->SetCanvasDefH(600); //Height of canvas
        theStyle->SetCanvasDefW(600); //Width of canvas
        theStyle->SetCanvasDefX(0);   //POsition on screen
        theStyle->SetCanvasDefY(0);
        
        // For the Pad:
        theStyle->SetPadBorderMode(0);
        // theStyle->SetPadBorderSize(Width_t size = 1);
        theStyle->SetPadColor(kWhite);
        theStyle->SetPadGridX(true);
        theStyle->SetPadGridY(true);
        theStyle->SetGridColor(0);
        theStyle->SetGridStyle(3);
        theStyle->SetGridWidth(1);
        
        // For the frame:
        theStyle->SetFrameBorderMode(0);
        theStyle->SetFrameBorderSize(1);
        theStyle->SetFrameFillColor(0);
        theStyle->SetFrameFillStyle(0);
        theStyle->SetFrameLineColor(1);
        theStyle->SetFrameLineStyle(1);
        theStyle->SetFrameLineWidth(1);
        
        // For the histo:
        theStyle->SetHistLineColor(1);
        theStyle->SetHistLineStyle(0);
        theStyle->SetHistLineWidth(1);
        
        
        theStyle->SetEndErrorSize(2);
        
        theStyle->SetMarkerStyle(20);
        theStyle->SetMarkerSize(0.5);
        
        
        //For the fit/function:
        theStyle->SetOptFit(1);
        theStyle->SetFitFormat("5.4g");
        theStyle->SetFuncColor(2);
        theStyle->SetFuncStyle(1);
        theStyle->SetFuncWidth(1);
        
        //For the date:
        theStyle->SetOptDate(0);
        
        // For the statistics box:
        theStyle->SetOptFile(0);
        
        theStyle->SetOptStat("e");
        theStyle->SetStatColor(kWhite);
        theStyle->SetStatTextColor(1);
        theStyle->SetStatFormat("6.4g");
        theStyle->SetStatBorderSize(1);
        
        // Margins:
         theStyle->SetPadTopMargin(0.1);
         theStyle->SetPadBottomMargin(0.1);
         theStyle->SetPadLeftMargin(0.1);
         theStyle->SetPadRightMargin(0.05);
        
        // For the Global title:
        
        // Uncomment to remove title
        //    theStyle->SetOptTitle(0); 
        theStyle->SetTitleFont(42);
        theStyle->SetTitleColor(1);
        theStyle->SetTitleTextColor(1);
        theStyle->SetTitleFillColor(10);
        theStyle->SetTitleFontSize(0.05);
        // theStyle->SetTitleH(0); // Set the height of the title box
        // theStyle->SetTitleW(0); // Set the width of the title box
        // theStyle->SetTitleX(0); // Set the position of the title box
        theStyle->SetTitleY(1); // Set the position of the title box
        theStyle->SetTitleStyle(1001);
        // theStyle->SetTitleBorderSize(2);
        
        // For the axis titles:
        
        theStyle->SetTitleColor(1, "XYZ");
        theStyle->SetTitleFont(42, "XYZ");
        theStyle->SetTitleSize(0.05, "XYZ");
        theStyle->SetTitleXOffset(0.9);
        theStyle->SetTitleYOffset(1.25);
        
        // For the axis labels:
        
        theStyle->SetLabelColor(1, "XYZ");
        
        theStyle->SetLabelFont(42, "XYZ");
        
        theStyle->SetLabelOffset(0.007, "XYZ");
        
        theStyle->SetLabelSize(0.045, "XYZ");
        
        // For the axis:
        
        theStyle->SetAxisColor(1, "XYZ");
        theStyle->SetStripDecimals(kTRUE);
        theStyle->SetTickLength(0.03, "XYZ");
        theStyle->SetNdivisions(510, "XYZ");
        theStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
        theStyle->SetPadTickY(1);
        
        // Change for log plots:
        theStyle->SetOptLogx(0);
        theStyle->SetOptLogy(0);
        theStyle->SetOptLogz(0);
        
        // Postscript options:
        theStyle->SetPaperSize(20.,20.);
        // theStyle->SetLineScalePS(Float_t scale = 3);
        // theStyle->SetLineStyleString(Int_t i, const char* text);
        // theStyle->SetHeaderPS(const char* header);
        // theStyle->SetTitlePS(const char* pstitle);
        
        // theStyle->SetBarOffset(Float_t baroff = 0.5);
        // theStyle->SetBarWidth(Float_t barwidth = 0.5);
        // theStyle->SetPaintTextFormat(const char* format = "g");
        // theStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
        // theStyle->SetTimeOffset(Double_t toffset);
        // theStyle->SetHistMinimumZero(kTRUE);
        theStyle->SetTextSize(0.045);
        theStyle->SetTextFont(42);
        
        //   style->SetOptFit(101);
        //   style->SetOptStat(1111111); 
        
    }else {
        // Avoid modifying the default style!
        theStyle = gStyle;
    }
    return theStyle;
}

// Make a new canvas setting name, title, x/y divisions, size.
TCanvas * newCanvas(TString title="", Int_t xdiv=1, Int_t ydiv =1,
            Int_t w=600, Int_t h=600)
{
    TCanvas* c= new TCanvas;
    c->SetWindowSize(w,h);
    if (title != "") c->SetTitle(title);
    if (xdiv*ydiv>1) {
        c->Divide(xdiv,ydiv);
        c->cd(1);
    }
    return c;
}

// Print all open canvases in separate files
// Type = "png", "eps", etc.
void printCanvases(TString type)
{
    TIter iter(gROOT->GetListOfCanvases());
    TCanvas *c;
    while( (c = (TCanvas *)iter()) ) {
        TString name =  c->GetTitle();
        c->Print(name+"."+type,type);
    }
}


void printCanvases(TString type, TString prefix="")
{
    
    TIter next(gROOT->GetListOfCanvases());
    TCanvas *c;
    while ((c = (TCanvas *)next()) )
    {
	TString name =  c->GetTitle();
        if (prefix!="") gROOT->ProcessLine(".!mkdir -p ./"+prefix);
        c->Print(prefix+name+"."+type,type);
    }
}




// Set histogram titles
void setTitles(TH1 *h, TString xtitle, TString ytitle)
{
    h->GetXaxis()->SetTitle(xtitle);
    h->GetYaxis()->SetTitle(ytitle);
}

void setTitles(TH1 *h, TString xtitle, TString ytitle, TString ztitle)
{
    h->GetXaxis()->SetTitle(xtitle);
    h->GetYaxis()->SetTitle(ytitle);
    h->GetZaxis()->SetTitle(ztitle);
}

// Print all histograms in separate files
// Type = "png", "eps", etc.
void printHistograms(TString type, TString prefix="")
{
    
    TIter next(gDirectory->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) 
    {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH1 *h = (TH1*)key->ReadObj();
        TCanvas *c = newCanvas();
        h->Draw();
        TString name =  h->GetTitle();
        if (prefix!="") gROOT->ProcessLine(".!mkdir -p ./"+prefix);
        c->Print(prefix+name+"."+type,type);
        delete c;
    }
}

//Prints each individual histogram to picture formats
void printPictures(TH1 *h, TString iname, TString opath, TCanvas *c, int pad=1)
{							
    c->cd(pad);
    int max=h->GetBinContent(h->GetMaximumBin());
    h->SetMaximum(max*1.1);
    h->SetMinimum(0);

    //Focus on Valid ChipIDs
    if (strcmp(h->GetName(),"ChipID")==0)
	h->GetXaxis()->SetRangeUser(3700,3800);

    //Set color/style
    h->SetMarkerColor(kBlue+3);
    h->SetMarkerStyle(1);
    h->SetFillStyle(1001);
    h->SetFillColor(kBlue+3);
    h->SetLineColor(kBlue+3);

    gStyle->SetOptStat(111111);

    h->Draw();

    //Add Legend
    TLegend* leg = new TLegend(0.78,0.4,0.98,0.45);
    leg->AddEntry(h,iname,"l");
    leg->Draw();

    gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_pdfs/");
    c->Print(opath+"individual_pdfs/"+iname+"--"+h->GetName()+".pdf","pdf");

    gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_pngs/");
    c->Print(opath+"individual_pngs/"+iname+"--"+h->GetName()+".png","png");

    gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_jpgs/");
    c->Print(opath+"individual_jpgs/"+iname+"--"+h->GetName()+".jpg","jpg");
    
    gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_roots/");
    c->Print(opath+"individual_roots/"+iname+"--"+h->GetName()+".root","root");
}

//Layers all histograms in h on single canvas/pad in different colors/styles
void layerHistogram(vector<TH1*> h, vector<TString> inames, TCanvas *c, int pad=1)
{
    int numH = h.size();
    cout << "Number of histograms: " << numH << endl;
    vector<int> maxs;
    int max=h[0]->GetBinContent(h[0]->GetMaximumBin());
    // vector<int> mins;
    // int min=h[0]->GetBinContent(h[0]->GetMinimumBin());
    h[0]->SetMaximum(max*1.1);
    h[0]->SetMinimum(0);

    //Focus on Valid ChipIDs
    if (strcmp(h[0]->GetName(),"ChipID")==0)
    {
	for(int f=0;f<numH;f++)
	{
	    h[f]->GetXaxis()->SetRangeUser(3700,3800);
	}
    }
	
    c->cd(pad);
    int n = 0;
    for (int i=0;i<numH;i++)
    { 
	//Set color/style
	
	if (i==4) //avoid yellow
	    n++; 
	
	if (numH==1)
	{
	    h[i]->SetMarkerColor(kBlue+3);
	    h[i]->SetMarkerStyle(1);
	    h[i]->SetFillStyle(1001);
	    h[i]->SetFillColor(kBlue+3);
	    h[i]->SetLineColor(kBlue+3);
	}
	else if (numH>9 and i>9) //Should never have more than 9 histograms, but it works
	{
	    h[i]->SetMarkerColor(i+30+n);
	    h[i]->SetMarkerStyle(1);
	    h[i]->SetFillStyle(i+3004);
	    h[i]->SetFillColor(i+30+n);
	    h[i]->SetLineColor(i+30+n);
	}
	else if (numH<5)
	{

	    h[i]->SetMarkerColor(pow(2,i));
	    h[i]->SetMarkerStyle(1);
	    h[i]->SetFillStyle(i+3004);
	    h[i]->SetFillColor(pow(2,i));
	    h[i]->SetLineColor(pow(2,i));
	}
	else
	{
	    h[i]->SetMarkerColor(i+1+n);
	    h[i]->SetMarkerStyle(1);
	    h[i]->SetFillStyle(i+3004);
	    h[i]->SetFillColor(i+1+n);
	    h[i]->SetLineColor(i+1+n);
	}

	//Resize as necessary
	maxs.push_back(h[i]->GetBinContent(h[i]->GetMaximumBin()));
	// mins.push_back(h[i]->GetBinContent(h[i]->GetMinimumBin()));

	if (maxs[i]>max)
	    h[0]->SetMaximum(maxs[i]*1.1);
	
	// if (mins[i]<min)
	//     h[0]->SetMinimum(mins[i]*1.1);
    }

    gStyle->SetOptStat(111111);
    h[0]->Draw();
    for (int i=0;i<numH;i++)
	    h[i]->Draw("SAMES");


   //Add Legend
    TLegend* leg = new TLegend(0.78,0.4,0.98,0.4+0.05*numH);
    for (int l=0;l<numH;l++)
	leg->AddEntry(h[l],inames[l],"l");
    leg->Draw();



    c->Update();


    for (int f=0;f<numH;f++)
    {	
    	//Retrieve stat box
    	TPaveStats *st = (TPaveStats*)h[f]->FindObject("stats");
   
    	//Color code
	st->SetTextColor(pow(2,f));

    	//Reposition
    	st->SetX1NDC(.78-0.22*f);  //new x start position
    	st->SetX2NDC(.98-0.22*f);  //new x end position
    }
}


//Layers only histograms with names listed in vector-hnames
void layerSpecific(vector<vector<TH1*>> hs, vector<const char*> hnames,vector<TString> inames, TCanvas *can)
{
    int numF = hs.size();
    int numH = hs[0].size();
    vector<TH1*> plot_hists;
    int complete=0;
    cout << "locating histograms... " << endl;
    for (int i=0;i<numH;i++)
    {
    	const char* histname = hs[0][i]->GetName();
	
	for (int n=0;n<hnames.size();n++)
	{
	    plot_hists.clear();
	    if ( strcmp(histname,hnames[n])==0 )
	    { 
		complete++;
		cout << "Found "<< hnames[n] <<endl;
		for (int j=0;j<numF;j++)
		    plot_hists.push_back( hs[j][i] );
		layerHistogram(plot_hists,inames,can,n+1);
	    }
	}
    }
    if (complete==hnames.size())
    {
    	can->Write();
    }
    else
    	cout << "Unable to locate all desired Histograms." << endl;
}

//Creates canvases containing all histograms for each file
void plotAll(vector<vector<TH1*>> hs, vector<TString> inames, TString opath)
{
    cout << "Plotting all histograms." << endl;
    int numF = hs.size();
    int numH = hs[0].size();
    int c_side = ceil(sqrt(numH));
    for (int i=0;i<numF;i++)
    {
	TString name = "All_Histograms-";
	name+=inames[i];
	TCanvas* allc = newCanvas(name);
	allc->SetName(name);
	allc->SetTitle(name);
	double ipart;
	if(modf(sqrt(numH),&ipart) < 0.4)
	    allc->Divide(c_side,c_side-1);
	else
	    allc->Divide(c_side,c_side);
	for (int j=0;j<numH;j++)
	{
	    allc->cd(j+1);
	    hs[i][j]->SetMarkerColor(kBlue+3);
	    hs[i][j]->SetMarkerStyle(1);
	    hs[i][j]->SetFillStyle(1001);
	    hs[i][j]->SetFillColor(kBlue+3);
	    hs[i][j]->SetLineColor(kBlue+3);
	    gStyle->SetOptStat(111111);
 	    hs[i][j]->DrawCopy();
	}
	allc->Write();
	
	//Print in different formats
	gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_pdfs/");
	allc->Print(opath+"individual_pdfs/"+inames[i]+".pdf","pdf");
	gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_pngs/");
	allc->Print(opath+"individual_pngs/"+inames[i]+".png","png");
	gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_jpgs/");
	allc->Print(opath+"individual_jpgs/"+inames[i]+".jpg","jpg");
	gROOT->ProcessLine(".!mkdir -p "+opath+"/individual_roots/");
	allc->Print(opath+"individual_roots/"+inames[i]+".root","root");
    }
}


void layerAll(vector<vector<TH1*>> hs, vector<TString> inames)
{
    cout << "Layering all histograms." << endl;
    int numF = hs.size();
    int numH = hs[0].size();
    vector<const char*> names;
    for (int i=0;i<numH;i++)
	names.push_back(hs[0][i]->GetName());
    TCanvas* layc = newCanvas("All Histograms Layered");
    int c_side = ceil(sqrt(numH));
    double ipart;
    if(modf(sqrt(numH),&ipart) < 0.4)
	layc->Divide(c_side,c_side-1);
    else
	layc->Divide(c_side,c_side);
    layc->SetName("AllHistogramsLayered");
    layerSpecific(hs, names, inames, layc);
}
