#include <RooFitResult.h>
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TTreeReaderArray.h"
#include "RooRealVar.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "RooPlot.h"

using namespace std;
using namespace RooFit;

// Create a structure that holds the data and sample in organized form
struct dataStruct {
    int    type;
    string name;
    Double_t weight;
    Color_t color;
    string legend;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// FITTING SELECTION
// Sample and mass region
const string toBeFitted = "PtPair_SIDEB";

// File with data
const char* datafile = "dataFile.root";

///////////////////////////////////////////////////////////////////////////////////////////////

// INTEGRATED LUMI OBTAINED WITH pixelLumiCalc:
const Double_t INTEGRATED_LUMI = 937. ;

// Variables that holds the weights for each data
// CONTINUUM BKG:
Double_t WEIGHT_DYmumu   = INTEGRATED_LUMI / 1.5e6   * 1320.  * 2.;
Double_t WEIGHT_DYmumuL  = INTEGRATED_LUMI / 1e5     * 13.94;
Double_t WEIGHT_DYmumuH  = INTEGRATED_LUMI / 2966364 * 1297.;
Double_t WEIGHT_inelinel = INTEGRATED_LUMI / 1e5     * 17.902;
Double_t WEIGHT_inelel   = INTEGRATED_LUMI / 1e5     * 15.398 * 2.;
Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 31.220 * 0.938985;

// RESONANT BKG:
Double_t WEIGHT_inclY1S  = INTEGRATED_LUMI / 2183761 * 78963. * 0.15080;
Double_t WEIGHT_inclY2S  = INTEGRATED_LUMI / 1065233 * 58961. * 0.08386 * 0.4;
Double_t WEIGHT_inclY3S  = INTEGRATED_LUMI / 533761  * 11260. * 0.57950 * 0.4;

// SIGNAL:
Double_t WEIGHT_signal1  = INTEGRATED_LUMI / 1e5 * 542.710 * 0.025 * 0.651 * 0.534879;
Double_t WEIGHT_signal2  = INTEGRATED_LUMI / 1e5 * 234.240 * 0.019 * 0.690 * 0.534879;
Double_t WEIGHT_signal3  = INTEGRATED_LUMI / 1e5 * 163.700 * 0.022 * 0.710 * 0.534879;

///////////////////////////////////////////////////////////////////////////////////////////////

// DATASETS
const dataStruct dataset[12] = {
        {0, "dymumu"  ,WEIGHT_DYmumu  ,2  ,"PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuL" ,WEIGHT_DYmumuL ,2  ,"PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuH" ,WEIGHT_DYmumuH ,2  ,"PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "inelinel",WEIGHT_inelinel,419,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
        {0, "inelel"  ,WEIGHT_inelel    ,30 ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
        {1, "elel"    ,WEIGHT_elel    ,800,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},
        {0, "inclY1S" ,WEIGHT_inclY1S ,5  ,"PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},
        {0, "inclY2S" ,WEIGHT_inclY2S ,5 },
        {0, "inclY3S" ,WEIGHT_inclY3S ,5 },
        {1, "signal1" ,WEIGHT_signal1 ,40 ,"STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},
        {1, "signal2" ,WEIGHT_signal2 ,40},
        {1, "signal3" ,WEIGHT_signal3 ,40}};

///////////////////////////////////////////////////////////////////////////////////////////////

void Fit() {

    // Create an output canvas
    auto *c = new TCanvas("canvas", "canvas", 1800, 1000);
    c->SetTicks();

    // Read the experimental data and create a histogram out of it
    unique_ptr<TFile> dataCluster(TFile::Open(datafile));
    string file = toBeFitted + "_" + "data";
    auto *dataHist(dataCluster->Get<TH1>(file.c_str()));

    // Get the max value of the x-axis
    Double_t upperLimit = dataHist->GetBinCenter(dataHist->GetNbinsX()) + dataHist->GetBinWidth(dataHist->GetNbinsX()) / 2;

///////////////////////////////////////////////////////////////////////////////////////////////

    // Create three RooFit variables, one generic and two that will hold the fitting values
    RooRealVar x("x", "x", 0, upperLimit);
    RooRealVar disFitResult("DisFit", "Value", 0.5, 0, 1000);
    RooRealVar excFitResult("ExcFit", "NV", 5, 0, 1000);

    // Create two empty histograms that holds the exclusive and dissociative data
    auto excHist = new TH1F("excHist", "Exclusive", dataHist->GetNbinsX(), 0, upperLimit);
    auto disHist = new TH1F("disHist", "Dissociative", dataHist->GetNbinsX(), 0, upperLimit);

    // Fill the histograms
    for (const dataStruct& simData : dataset) {
        file = toBeFitted + "_" + simData.name;
        auto h =dataCluster->Get<TH1>(file.c_str());
        h->Scale(simData.weight);
        if (simData.type == 0)
            disHist->Add(h);
        else
            excHist->Add(h);

    }

///////////////////////////////////////////////////////////////////////////////////////////////

    // Convert the TH1 histograms to RooFit histograms
    RooDataHist data("data", "Experimental data", x, dataHist);
    RooDataHist exc("exc", "Exclusive fit", x, excHist);
    RooDataHist dis("dis", "Dissociative fit", x, disHist);

    // Then make probability density functions out of the histograms
    RooHistPdf excPDF("excPdf", "Exclusive PDF", x, exc);
    RooHistPdf disPDF("disPdf", "Dissociative PDF", x, dis);

    // Create a model that will fit the data using the histograms templates
    RooAddPdf model("model", "Model", RooArgList(excPDF, disPDF),
                    RooArgList(excFitResult, disFitResult));

    // Fit the data with minimum logging
    model.fitTo(data,PrintLevel(-1));

///////////////////////////////////////////////////////////////////////////////////////////////

    // Print the resulting values
    cout << endl << "----------------------------------------" << endl;
    cout << "Exclusive fit scale result: " << excFitResult.getVal() / (excHist->Integral()) << endl;
    cout << "Dissociative fit scale result: " << disFitResult.getVal() / (disHist->Integral()) << endl;
    cout <<"----------------------------------------" <<  endl << endl;

    // Plot the data and resulting fit histogram
    RooPlot* xFrame = x.frame(Title("Fitting result"));
    data.plotOn(xFrame, XErrorSize(0));
    model.plotOn(xFrame, LineWidth(1));
    xFrame->Draw();

    // Save the plots
    c->SaveAs("fitHist.png");
}