#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TNtuple.h>
#include <TH2.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <TBenchmark.h>
#include <TInterpreter.h>
#include <TApplication.h>
#include <TString.h>
#include <Event.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TRefArray.h>
#include <TRef.h>
#include <TH1.h>
#include <TBits.h>
#include <TMath.h>
#include <TFile.h>
#include <TClassTable.h>
#include <TTree.h>
#include <TBranch.h>

#include "dataChecker.cc"
#include "plotter.cxx"

using namespace std;
int main(int argc, char** argv)
{
    cout<<endl;


    if (argc!=2) 
    {
        cout << "Please provide ONE text file containing ";
	cout <<"2 root input files and one root output file ";
        cout << "separated by lines." << endl;
        cout << "Usage: <path>/reader input.txt" << endl;
	cout << endl;
        return 0;
    }

    //Currently only able to support 2 layers
    int MAX_HIST = 2;




    vector<TString> ifilenames;
    TString ofilename;
    TString outname;
    TString outday;
    vector<TFile*> ifiles;
    
    ifstream textfile (argv[1]);
    if (textfile.is_open())
    {
	int counter=0;
	string line;
    	while( getline (textfile,line) )
    	{
	    if(line.length()<6)
		continue;
	    if(line.substr(line.length()-5,5)!=".root")
	    {		
		cout << "########################"<< endl;
		cout << "Detected incorrect file: " << line << endl;
		cout << "########################"<< endl;
		cout << endl;
		cout << "Input and Output files must be in .root format." << endl;
		cout << endl;
		return 0;
	    }
	    counter++;
	    if (counter<=MAX_HIST)
	    {
		ifilenames.push_back(line);
		ifiles.push_back(new TFile(ifilenames[counter-1], "READ"));
		cout<<"Input File "<<counter<<": "<<line<<endl;
	    }
	    else if (counter<=MAX_HIST+1)
	    {
		ofilename = line;
	        outname = ofilename;
		outname = outname.Remove(outname.Length()-5, 5);
		outday = outname;
		outday.Remove(4,outday.Length()-1);
		gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/root/");
		TFile *ofile = new TFile("./output/"+outday+"/"+outname+"/root/"+ofilename, "RECREATE");
		cout<<"Output File: "<<"./output/"+outday+"/"+outname+"/root/"+ofilename<<endl<<endl;
	    }
	    else
	    {
		cout << "####################"<< endl;
		cout << "Detected Extra File: "<<line<<endl;
		cout << "####################"<< endl;
		cout << endl;
		cout << "Only list 2 input files and one output file."<<endl;
		cout << "Make sure you have no more than 3 files listed in text file."<<endl; 
		cout << endl;
		return 0;
	    }
	}
    	textfile.close();
    }
    else 
    {
    	cout << "Unable to open input file: " << argv[1];
    	return 0;
    }


   
    vector<TH1*> hist1; 
    vector<TH1*> hist2;
    int cnt1=0; //counts iterations through ifile1
    int cnt2=0; //counts iterations through ifile2

    //List all keys
    TList *keylist1 = ifiles[0]->GetListOfKeys();
    cout << endl << "List of keys in Input File 1:" << endl;
    keylist1->Print();
    TList *keylist2 = ifiles[1]->GetListOfKeys();
    cout << endl << "List of keys in Input File 2:" << endl;
    keylist2->Print();
    cout << endl;

    cout << "begin loops" << endl;
    TIter next(keylist1);
    TKey *key1;
    while ((key1 = (TKey*)next())) 
    {
	cnt1++;
	cout << "loop through file1: " << cnt1;
	TClass *cl = gROOT->GetClass(key1->GetClassName());
        if (!cl->InheritsFrom("TH1"))
	{
	    cout << endl;
	    continue;
	}
	cout << " - found " << key1->GetClassName() << endl;
        TH1 *h = (TH1*)key1->ReadObj();
	hist1.push_back(h);
    }
    cout << "----------" << endl;
    TIter next2(keylist2);
    TKey *key2;
    while ((key2 = (TKey*)next2())) 
    {
	cnt2++;
	cout << "loop through file2: " << cnt2;
	TClass *cl = gROOT->GetClass(key2->GetClassName());
        if (!cl->InheritsFrom("TH1"))
	{ 
	    cout<<endl;
	    continue;
	}
	cout << " - found " << key2->GetClassName() << endl;
        TH1 *h = (TH1*)key2->ReadObj();
	hist2.push_back(h);
    }
    cout << "end loops" << endl;
    cout << endl;
    cout << "hist1 size: " << hist1.size() << endl;
    cout << "hist2 size: " << hist2.size() << endl;

    if(hist1.size() != hist2.size())
    {
	cout<<"Error: differing number of histograms."<<endl;
	return 0;
    }
    



    //Layer Histograms, print/save individually
    for (int i=0;i<hist1.size();i++)
    {
		
	//Make sure histograms have same title
	const char* class1 = hist1[i]->GetName();
	const char* class2 = hist2[i]->GetName();
	if (strcmp(class1,class2)==0)
	{
	    TCanvas *c = newCanvas();
	    c->SetTitle(class1);
	    c->SetName(class1);
	    cout << endl << class1 <<  " histogram match. Layering..." << endl;
	    //layerHistogram(hist1[i], hist2[i], "output/"+outname+"/");
	    layerHistogram(hist1[i], hist2[i], c);
	    c->Write();

	    gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
	    c->Print( "output/"+outday+"/"+outname+"/"+class1+".pdf","pdf");
	    c->Print( "output/"+outday+"/"+outname+"/"+class1+".root","root");
	}
    }
    



    //Better method for layering
    //Layers all histograms onto same canvas, prints/saves
    TCanvas *c = newCanvas();
    c->Divide(3,4);
    for (int i=0;i<hist1.size();i++)
    {
	layerHistogram(hist1[i], hist2[i], c, i+1);
    }
    cout<<endl<<"Printing all Histograms."<<endl;
    c->Write();
    gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
    c->Print( "output/"+outday+"/"+outname+"/"+"histograms.pdf","pdf");
    c->Print( "output/"+outday+"/"+outname+"/"+"histograms.root","root");

     

    //CRC Canvas
    cout << endl << "Drawing CRC Canvas" << endl;
    TCanvas *crc = newCanvas();
    crc->SetTitle("CRC Comparison");
    crc->Divide(2,1);
    cout << ">>>>>>>ATTEMPTING TO LOCATE HISTOGRAMS<<<<<<<" << endl;
    int complete = 0;
    for (int i=0;i<hist1.size();i++)
    {
    	cout << "locating CRC histograms..." << i << endl;
    	const char* histname = hist1[i]->GetName();
    	if ( strcmp(histname,"CRC")==0 )
    	{ 
	    complete++;
    	    cout << "Found CRC" << endl;
    	    layerHistogram(hist1[i], hist2[i], crc, 1);
    	}
    	if ( strcmp(histname,"DiffCRC")==0 )
    	{
	    complete++;
    	    cout << "Found DiffCRC" << endl;
    	    layerHistogram(hist1[i], hist2[i], crc, 2);
    	}
    }
    if (complete==2)
    {
	crc->Write();
	gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
	crc->Print( "output/"+outday+"/"+outname+"/"+"crc_check.pdf","pdf");
	crc->Print( "output/"+outday+"/"+outname+"/"+"crc_check.root","root");
    }
    else
	cout << "Unable to locate all CRC Histograms." << endl;
  


    //Control Bit/Flag Canvas
    cout << "Drawing Control Bit/Flag Canvas" << endl;
    TCanvas *cbf = newCanvas();
    cbf->SetTitle("Control Bit/Flag Comparison");
    cbf->Divide(2,2);
    cout << ">>>>>>>ATTEMPTING TO LOCATE HISTOGRAMS<<<<<<<" << endl;
    complete = 0;
    for (int i=0;i<hist1.size();i++)
    {
    	cout << "locating histograms..." << i << endl;
    	const char* histname = hist1[i]->GetName();
    	if ( strcmp(histname,"1010")==0 )
    	{ 
	    complete++;
    	    cout << "Found CB1010" << endl;
    	    layerHistogram(hist1[i], hist2[i], cbf, 1);
    	}
    	if ( strcmp(histname,"1100")==0 )
    	{ 
	    complete++;
    	    cout << "Found CB1100" << endl;
    	    layerHistogram(hist1[i], hist2[i], cbf, 2);
    	}
    	if ( strcmp(histname,"1110")==0 )
    	{ 
	    complete++;
    	    cout << "Found CB1110" << endl;
    	    layerHistogram(hist1[i], hist2[i], cbf, 3);
    	}
    	if ( strcmp(histname,"Flag")==0 )
    	{
	    complete++;
    	    cout << "Found Flag" << endl;
    	    layerHistogram(hist1[i], hist2[i], cbf, 4);
    	}
    }
    if (complete==4)
    {
	cbf->Write();
	gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
	cbf->Print( "output/"+outday+"/"+outname+"/"+"cbf_check.pdf","pdf");
	cbf->Print( "output/"+outday+"/"+outname+"/"+"cbf_check.root","root");
    }
    else
	cout << "Unable to locate all Control Bit/Flag Histograms." << endl;



    cout << endl;
    cout << ifilenames[0] << " histograms plotted in blue." << endl;
    cout << ifilenames[1] << " histograms plotted in red." << endl;





    return 0;
}
