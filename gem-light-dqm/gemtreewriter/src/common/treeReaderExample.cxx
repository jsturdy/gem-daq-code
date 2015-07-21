#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>
#include <TFile.h>
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

/**
* GEM Tree Reader example (reader) application 
*/

/*! 
  \brief GEM Tree reader reads file with the GEM Event Tree and fills the histogram with number of vfats per event
*/

using namespace std;
int main(int argc, char** argv)
{
    TString ifilename = argv[1];
    TString ofilename = argv[2];
    // read the tree generated with gtc
    TFile *ifile = new TFile(ifilename, "READ");
    TTree *tree = (TTree*)ifile->Get("GEMtree");
    // create a pointer to an event object. This will be used
    // to read the branch values.
    Event *event = new Event();
    // get branch and set the branch address
    TBranch *branch = tree->GetBranch("GEMEvents");
    branch->SetAddress(&event);
    Int_t nentries = tree->GetEntries();

    // create the output file
    TFile *ofile = new TFile(ofilename, "RECREATE");
    //book a histogram for number of VFATs in the event
    TH1F* hiVFAT = new TH1F("VFAT", "Number VFAT per event", 100, (Double_t)-0.5,(Double_t)300.5 );
    hiVFAT->SetFillColor(48);
    Int_t nVFAT;
    // loop over tree entries
    for (Int_t i = 0; i < nentries; i++)
    {
        // clear number of VFATs
        nVFAT = 0;
        // Retrieve next entry
        branch->GetEntry(i);
        cout << "Event number " << i << endl;
        // create vector of GEBdata. For data format details look at Event.h
        vector<GEBdata> v_geb;
        v_geb = event->gebs();
        // loop over gebs
        for (Int_t j = 0; j < v_geb.size(); j++)
        {
            // create vector of VFATdata. For data format details look at Event.h
            vector<VFATdata> v_vfat;
            v_vfat = v_geb.at(j).vfats();
            // Increment number of VFATs in the given event
            nVFAT += v_vfat.size();
        }
        hiVFAT->Fill(nVFAT);
    }
    ofile->Write();
    return 0;
}
