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
#include <time.h>

#include "dataChecker.cc"
#include "plotter.cxx"

using namespace std;
int main(int argc, char** argv)
{

    cout<<endl;

    if (argc!=2) 
    {
        cout << "Please provide ONE text file containing root input files separated by lines." << endl;
        cout << "Usage: <path>/reader input.txt" << endl;
	cout << endl;
        return 0;
    }


    
    //Create output file in proper location
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char oPath [32];
    char oName [64];
    char oCommand [32];
    strftime (oPath, 32, "./output/%m_%d/", timePtr);
    strftime (oName, 64, "./output/%m_%d/%b%d_%G-%H_%M.root", timePtr);
    strftime (oCommand, 32, ".!mkdir -p ./output/%m_%d/", timePtr);
    
    cout << oPath << endl;
    cout << oName << endl;
    cout << oCommand << endl;

    //return 0;
   

    vector<vector<TH1*>> hs;
  
   
   
    vector<TString> ifilenames;
    vector<TFile*> ifiles;
    int numF = 0; //number of files
    ifstream textfile (argv[1]);
    if (textfile.is_open())
    {
	string line;
    	while( getline (textfile,line) )
    	{
	    if(line.length()<6 or line.substr(0,2)=="//")
		continue;
	    if(line.substr(line.length()-5,5)!=".root")
	    {		
		cout << "########################"<< endl;
		cout << "Detected incorrect file: " << line << endl;
		cout << "########################"<< endl;
		cout << endl;
		cout << "Input Files must be in .root format." << endl;
		cout << endl;
		return 0;
	    }
	    numF++;

	    ifilenames.push_back(line);
	    ifiles.push_back(new TFile(ifilenames[numF-1], "READ"));
	    cout<<"Input File "<<numF<<": "<<line<<endl;
	      
	}
    	textfile.close();
    }
    else 
    {
    	cout << "Unable to open input file: " << argv[1];
    	return 0;
    }
    
    if (numF < 2)
    {
	cout << "Number of files must be at least 2." << endl;
	return 0;
    }
    
    gROOT->ProcessLine(oCommand);
    TFile *ofile = new TFile(oName, "RECREATE");
    cout<<"Output File: "<< oName <<endl<<endl;

    int counter;
    // vector<TList*> keylists;
    // vector<TIter> nexts;
    // vector<TKey*> keys;

    //TIter next;
    vector<TH1*> tempH;

    for (int i=0;i<numF;i++)
    {
	// keylists.push_back(new TList* [ifiles[i]->GetListOfKeys()]);
	// cout << endl << "List of keys in Input File " << i << ":" << endl;
	// keylists[i]->Print();
	// cout << "begin loops" << endl;
	// nexts.push_back(new TIter* [keylists[i]]);
	// keys.push_back(new TKey*);
	// counters.push_back(new int[0]);

	TList* keylist = new TList;
	keylist = ifiles[i]->GetListOfKeys();
	cout << endl << "List of keys in Input File " << i << ":" << endl;
	keylist->Print();
	cout << "==Begin Loops==" << endl;
	
	TIter nextkey(keylist);
	TKey *key = new TKey;

	counter=0;
	while (key = (TKey*)nextkey())
	{
	    counter++;
	    cout << "Loop through file "<<i<<": "<< counter;
	    TClass *cl = gROOT->GetClass(key->GetClassName());
	    if (!cl->InheritsFrom("TH1")) 
	    {
		cout << endl;
		continue;
	    }
	    cout << " - found " << key->GetClassName() << endl;
	    TH1 *h = (TH1*)key->ReadObj();
	    tempH.push_back(h);
	}

	hs.push_back(tempH);
	tempH.clear();

    }

    //Make sure all files have same number of histograms
    for(int i=0;i<numF;i++)
    {
	for(int j=0;j<hs[i].size();j++)
	{
	    if((i+1)!=numF and hs[i].size() != hs[i+1].size())
	    {
		cout<<"Histogram sizes do not match!"<<endl;
		return 0;
	    }
	}
    }
    int numH = hs[0].size(); //number of histograms in each file

    // vector<TH1*> hist1; 
    // vector<TH1*> hist2;
    // int cnt1=0; //counts iterations through ifile1
    // int cnt2=0; //counts iterations through ifile2

    // //List all keys
    // TList *keylist1 = ifiles[0]->GetListOfKeys();
    // cout << endl << "List of keys in Input File 1:" << endl;
    // keylist1->Print();
    // TList *keylist2 = ifiles[1]->GetListOfKeys();
    // cout << endl << "List of keys in Input File 2:" << endl;
    // keylist2->Print();
    // cout << endl;

    // cout << "begin loops" << endl;
    // TIter next1(keylist1);
    // TKey *key1;
    // while ((key1 = (TKey*)next1())) 
    // {
    // 	cnt1++;
    // 	cout << "loop through file1: " << cnt1;
    // 	TClass *cl = gROOT->GetClass(key1->GetClassName());
    //     if (!cl->InheritsFrom("TH1"))
    // 	{
    // 	    cout << endl;
    // 	    continue;
    // 	}
    // 	cout << " - found " << key1->GetClassName() << endl;
    //     TH1 *h = (TH1*)key1->ReadObj();
    // 	hist1.push_back(h);
    // }
    // cout << "----------" << endl;
    // TIter next2(keylist2);
    // TKey *key2;
    // while ((key2 = (TKey*)next2())) 
    // {
    // 	cnt2++;
    // 	cout << "loop through file2: " << cnt2;
    // 	TClass *cl = gROOT->GetClass(key2->GetClassName());
    //     if (!cl->InheritsFrom("TH1"))
    // 	{ 
    // 	    cout<<endl;
    // 	    continue;
    // 	}
    // 	cout << " - found " << key2->GetClassName() << endl;
    //     TH1 *h = (TH1*)key2->ReadObj();
    // 	hist2.push_back(h);
    // }
    // cout << "end loops" << endl;
    // cout << endl;
    // cout << "hist1 size: " << hist1.size() << endl;
    // cout << "hist2 size: " << hist2.size() << endl;

    // if(hist1.size() != hist2.size())
    // {
    // 	cout<<"Error: differing number of histograms."<<endl;
    // 	return 0;
    // }
    

    
    

    vector<TH1*> hists_ready; //holds same individual histogram from each file - ready to be passed to plotter

    //Layer Histograms, print/save individually
    char layerAnyways = 'n';
    for (int i=0;i<numH;i++)
    {
	hists_ready.erase(hists_ready.begin(),hists_ready.end());
	//Make sure histograms have same title
      	for(int j=0;j<numF-1;j++)
	{
	    if (strcmp(hs[j][i]->GetName(),hs[j+1][i]->GetName())!=0 and layerAnyways!='y')
	    {
		cout << "Histogram names do not match!" << endl;
		cout << "Attempt to layer anyways? [y/n] ";
		cin >> layerAnyways;
		if (layerAnyways != 'y')
		    return 0;
	    }
	}
	
	for(int k=0;k<numF;k++)
	    hists_ready.push_back(hs[k][i]);

     
	TCanvas *c = newCanvas();
	c->SetTitle(hs[0][i]->GetName());
	c->SetName(hs[0][i]->GetName());
	    
	layerHistogram(hists_ready, c);
	c->Write();

    // 	gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
    // 	c->Print( "output/"+outday+"/"+outname+"/"+class1+".pdf","pdf");
    // 	c->Print( "output/"+outday+"/"+outname+"/"+class1+".root","root");
    }

    



    // //Better method for layering
    // //Layers all histograms onto same canvas, prints/saves
    // TCanvas *c = newCanvas();
    // c->Divide(3,4);
    // for (int i=0;i<hist1.size();i++)
    // {
    // 	layerHistogram(hist1[i], hist2[i], c, i+1);
    // }
    // cout<<endl<<"Printing all Histograms."<<endl;
    // c->Write();
    // // gROOT->ProcessLine(".!mkdir -p ./output/"+outday+"/"+outname+"/");
    // // c->Print( "output/"+outday+"/"+outname+"/"+"histograms.pdf","pdf");
    // // c->Print( "output/"+outday+"/"+outname+"/"+"histograms.root","root");

     

    //CRC Canvas
    cout << endl << "Drawing CRC Canvas" << endl;
    TCanvas *crc = newCanvas();
    crc->SetTitle("CRC Comparison");
    crc->SetName("CRC Comparison");
    crc->Divide(2,1);
        
    vector<const char*> crc_names;
    crc_names.push_back("CRC");
    crc_names.push_back("DiffCRC");  
    layerSpecific(hs, crc_names, crc); 





    //Control Bit/Flag Canvas
    cout << "Drawing Control Bit Flag Canvas" << endl;
    TCanvas *cbf = newCanvas();
    cbf->SetTitle("Control Bit Flag Comparison");
    cbf->SetName("Control Bit Flag Comparison");

    cbf->Divide(2,2);

    vector<const char*> cbf_names;
    cbf_names.push_back("1010");
    cbf_names.push_back("1100");
    cbf_names.push_back("1110");
    cbf_names.push_back("Flag");
    layerSpecific(hs, cbf_names, cbf);



    return 0;
}
