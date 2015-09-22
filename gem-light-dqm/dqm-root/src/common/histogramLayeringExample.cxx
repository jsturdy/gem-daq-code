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
#include <math.h>

#include "plotter.cxx"

using namespace std;
int main(int argc, char** argv)
{
    if (argc!=2) 
    {
        cout << "Please provide ONE text file containing root input files separated by lines." << endl;
        cout << "Usage: <path>/reader input.txt" << endl;
        return 0;
    }

    //Create output file in proper location
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char oPath [64];
    char oName [64];
    char oCommand [64];
    strftime (oPath, 64, "./output/%m_%d-%H/%b%d_%G-%H_%M_%S/", timePtr);
    strftime (oName, 64, "./output/%m_%d-%H/%b%d_%G-%H_%M_%S/%b%d_%G-%H_%M_%S.root", timePtr);
    strftime (oCommand, 64, ".!mkdir -p ./output/%m_%d-%H/%b%d_%G-%H_%M_%S/", timePtr);
   
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
    
    if (numF < 1)
    {
	cout << "Number of files must be at least 1." << endl;
	return 0;
    }

    vector<TString> inames; //name of file without extension
    for (int i=0;i<ifilenames.size();i++)
    {
	    TString name = ifilenames[i];
	    int counter=0;
	    int loc = name.Last('/')+1;
	    int len = name.Length()-loc-5;
	    TSubString subname = name(loc,len);
	    inames.push_back(subname);
    }

    gROOT->ProcessLine(oCommand);
    TFile *ofile = new TFile(oName, "RECREATE");
    cout<<"Output File: "<< oName <<endl<<endl;

    int counter;
    vector<TH1*> tempH;

    //Retrieve histograms from all files
    for (int i=0;i<numF;i++)
    {
	    TList* keylist = new TList;
	    keylist = ifiles[i]->GetListOfKeys();
	    //cout << endl << "List of keys in Input File " << i << ":" << endl;
	    //keylist->Print();
	    //cout << "==Begin Loops==" << endl;
	    
	    TIter nextkey(keylist);
	    TKey *key = new TKey;

	    counter=0;
	    while (key = (TKey*)nextkey())
	    {
	        counter++;
	        //cout << "Loop through file "<<i<<": "<< counter;
	        TClass *cl = gROOT->GetClass(key->GetClassName());
	        if (!cl->InheritsFrom("TH1")) 
	        {
	    	//cout << endl;
	    	continue;
	        }
	        //cout << " - found " << key->GetClassName() << endl;
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

    //Plot single canvas with all histograms layered
    layerAll(hs, inames);

    vector<TH1*> hists_ready; //holds similar histogram from each file - ready to be passed to plotter

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
         
	    TCanvas *mainc = newCanvas();
	    mainc->Divide(1,1);
	    const char* cname = hs[0][i]->GetName();
	    //char* fcname;
	    //strcat(fcname, cname);
	    mainc->SetTitle(cname);
	    mainc->SetName(cname);
	        
	    layerHistogram(hists_ready,inames,mainc,1);
	    mainc->Write();
    }

    //CRC Canvas
    cout << endl << "Drawing CRC Canvas" << endl;
    TCanvas *crc = newCanvas();
    crc->SetTitle("CRC Comparison");
    crc->SetName("CRC Comparison");
    crc->Divide(2,1);
        
    vector<const char*> crc_names;
    crc_names.push_back("CRC1_vs_CRC2");
    crc_names.push_back("DiffCRC");  
    layerSpecific(hs,crc_names,inames,crc); 

    //Control Bit/Flag Canvas
    cout << endl << "Drawing Control Bit Flag Canvas" << endl;
    TCanvas *cbf = newCanvas();
    cbf->SetTitle("Control Bit Flag Comparison");
    cbf->SetName("Control Bit Flag Comparison");

    cbf->Divide(2,2);

    vector<const char*> cbf_names;
    cbf_names.push_back("1010");
    cbf_names.push_back("1100");
    cbf_names.push_back("1110");
    cbf_names.push_back("Flag");
    layerSpecific(hs,cbf_names,inames,cbf);

    //GGSW check
    cout << endl << "Drawing GGSW Check Canvas" << endl;
    TCanvas *ggsw = newCanvas();
    ggsw->SetTitle("GGSW Check");
    ggsw->SetName("GGSW Check");
    ggsw->Divide(2,1);
    vector<const char*> ggsw_names;
    ggsw_names.push_back("VFAT");
    ggsw_names.push_back("ChipID");
    layerSpecific(hs,ggsw_names,inames,ggsw);

    //Print canvases as other types
    TString pdf = oPath;
    pdf+="pdf/";
    TString png = oPath;
    png+="png/";
    TString jpg = oPath;
    jpg+="jpg/";
    TString root = oPath;
    root+="root/";
    printCanvases("pdf", pdf);
    printCanvases("png", png);
    printCanvases("jpg", jpg);
    //printCanvases("root", root);

    return 0;
}
