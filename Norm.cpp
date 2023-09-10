#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TTreeReaderArray.h"


using namespace std;

// Create a structure that holds the data and sample in organized form
struct dataStruct {
    int    id;
    string name;
    Double_t weight;
    Color_t color;
    string legend;
};
struct sampleStruct {
    string type;
    string title;
    string description;
    string unit;
    bool   log = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// FITTING SELECTION

// Distribution
const sampleStruct dist = {"AcoplZoom",
                           "AcoplZoom",
                           "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|",
                           ""   };
// Mass Region
const string mass  = "RESOM";

// Bin to be normalized
const int nBin = 1;

// Dataset to be normalized (by ID)
const int fitId = 4;

///////////////////////////////////////////////////////////////////////////////////////////////

// INTEGRATED LUMI OBTAINED WITH pixelLumiCalc:
const Double_t INTEGRATED_LUMI = 937. ;

// Variables that holds the weights for each data
// CONTINUUM BKG:
Double_t WEIGHT_DYmumu   = INTEGRATED_LUMI / 1.5e6   * 1320.  * 2.;            // FACTOR 2. FROM nET-2-6
Double_t WEIGHT_DYmumuL  = INTEGRATED_LUMI / 1e5     * 13.94      ;
Double_t WEIGHT_DYmumuH  = INTEGRATED_LUMI / 2966364 * 1297.      ;
Double_t WEIGHT_inelinel = INTEGRATED_LUMI / 1e5     * 17.902     ;
Double_t WEIGHT_inelel   = INTEGRATED_LUMI / 1e5     * 15.398 * 2.;            // FACTOR 2 FOR INELASTIC IN BOTH SIDES (LPAIR IS ONE-SIDE ONLY)
Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 31.220     ; // newSAMPLE
// Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 21.075,

// RESONANT BKG:
Double_t WEIGHT_inclY1S  = INTEGRATED_LUMI / 2183761 * 78963. * 0.15080; // FILTER EFFICIENCIES FROM PREP
Double_t WEIGHT_inclY2S  = INTEGRATED_LUMI / 1065233 * 58961. * 0.08386 * 0.4; // FACTOR 0.4 BASED ON PLOT FOR nET-2-6
Double_t WEIGHT_inclY3S  = INTEGRATED_LUMI / 533761  * 11260. * 0.57950 * 0.4;

// SIGNAL:
Double_t WEIGHT_signal1  = INTEGRATED_LUMI / 1e5 * 542.710 * 0.025 * 0.651;
Double_t WEIGHT_signal2  = INTEGRATED_LUMI / 1e5 * 234.240 * 0.019 * 0.690;
Double_t WEIGHT_signal3  = INTEGRATED_LUMI / 1e5 * 163.700 * 0.022 * 0.710;

///////////////////////////////////////////////////////////////////////////////////////////////

// DATASETS
const dataStruct dataset[12] = {
        {1, "dymumu"  ,WEIGHT_DYmumu  ,2  ,"PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
        {1, "dymumuL" ,WEIGHT_DYmumuL ,2  ,"PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
        {1, "dymumuH" ,WEIGHT_DYmumuH ,2  ,"PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
        {2, "inelinel",WEIGHT_inelinel,419,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
        {3, "inelel"  ,WEIGHT_inelel  ,30 ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
        {4, "elel"    ,WEIGHT_elel    ,800,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},
        {5, "inclY1S" ,WEIGHT_inclY1S ,5  ,"PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},
        {5, "inclY2S" ,WEIGHT_inclY2S ,5 },
        {5, "inclY3S" ,WEIGHT_inclY3S ,5 },
        {6, "signal1" ,WEIGHT_signal1 ,40 ,"STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},
        {6, "signal2" ,WEIGHT_signal2 ,40},
        {6, "signal3" ,WEIGHT_signal3 ,40}};

///////////////////////////////////////////////////////////////////////////////////////////////

void Norm(const char* datafile = "dataFile.root") {

    // Config canvas for plots
    auto *c = new TCanvas("canvas", "canvas", 1200, 1000);
    c->SetTicks();
	if (dist.log)
		c->SetLogy();

    // Create a THStack with adequate units in the x-axis
    THStack *histStack;
    if (dist.unit.empty())
        histStack = new THStack(dist.title.c_str(),
                                string(dist.type + "_" + mass + ";" + dist.description).c_str());
    else
        histStack = new THStack(dist.title.c_str(),
                                string(dist.type + "_" + mass + ";" + dist.description + " (" + dist.unit + ")").c_str());

    // Config legend and it's position
    auto legend = new TLegend(0.45,.68,.88,0.87);
    legend->SetBorderSize(0);
    legend->SetFillColorAlpha(kWhite, 0);

///////////////////////////////////////////////////////////////////////////////////////////////

    // Read experimental data
    unique_ptr<TFile> dataCluster(TFile::Open(datafile));
    string file = dist.type + "_" + mass + "_" + "data";
    auto *expData(dataCluster->Get<TH1>(file.c_str()));
    expData->SetMarkerStyle(20);
    expData->SetLineColor(kBlack);
    legend->AddEntry(expData, "Data", "lp");

    // Read simulated data and config variables used in the normalization
    double dataValue = expData->GetBinContent(nBin), sum = 0, fit = 0;
    for (const dataStruct& data: dataset){
        file = dist.type + "_" + mass + "_" + data.name;
        TH1* h(dataCluster->Get<TH1>(file.c_str()));

        // If the histogram is empty, skip it
        if (h->Integral() == 0)
            continue;

        // Get the sum of all histograms and the one to be normalized
        h->Scale(data.weight);
        if(data.id == fitId) fit += h->GetBinContent(nBin);
        sum += h->GetBinContent(1);
    }

///////////////////////////////////////////////////////////////////////////////////////////////

    // Print the normalization value
    std::cout << (fit + dataValue - sum) / fit << std::endl;

    // Draw the histograms
    for (const dataStruct& data: dataset){
        file = dist.type + "_" + mass + "_" + data.name;
        TH1* h(dataCluster->Get<TH1>(file.c_str()));

        // If the histogram is empty, skip it
        if (h->Integral() == 0) {
            continue;
        }

        // Personalize the histogram
        h->SetLineColor(kBlack);
        h->SetFillColor(data.color);

        // Fit the histogram and return the calculated value
        if (data.id == fitId){
            h->Scale((fit + dataValue - sum) / fit);
        }

        // Add a legend to the histogram (if it's needed)
        if (!data.legend.empty()) {
            legend->AddEntry(h, data.legend.c_str(), "f");
        }

        // Add the histogram to the stack
        histStack->Add(h);
    }

///////////////////////////////////////////////////////////////////////////////////////////////

    // If both histograms have data, then draw both
    if (histStack->GetMaximum() < expData->GetMaximum() + expData->GetBinError(expData->GetMaximumBin()) * 1.3)
        histStack->SetMaximum(expData->GetMaximum() + expData->GetBinError(expData->GetMaximumBin()) * 1.3);
    histStack->Draw("HIST");
    expData->Draw("E1X0P SAME");

    // Draw the legend and plot the graph
    legend->Draw("SAME");
    c->SaveAs(string(dist.type + "_" + mass + "_NORM" + (fit + dataValue - sum) / fit + ".png").c_str());
}