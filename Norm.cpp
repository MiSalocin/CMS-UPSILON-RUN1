#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TTreeReaderArray.h"

#include "datatypes.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

// FITTING SELECTION

// Sample
string sampleName = "AcoplZoom";
const sampleStruct selSample = samples[sampleName];

// Mass Region
const string mass  = "RESOM";

// Bin to be normalized
const int nBin = 1;

///////////////////////////////////////////////////////////////////////////////////////////////

// DATASETS
const dataStruct dataset[12] = {
        {0, "dymumu"  ,WEIGHT_DYmumu  ,2  ,"PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuL" ,WEIGHT_DYmumuL ,2  ,"PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuH" ,WEIGHT_DYmumuH ,2  ,"PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "inelinel",WEIGHT_inelinel,419,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
        {0, "inelel"  ,WEIGHT_inelel  ,30 ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
        {0, "elel"    ,WEIGHT_elel    ,800,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},
        {0, "inclY1S" ,WEIGHT_inclY1S ,5  ,"PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},
        {0, "inclY2S" ,WEIGHT_inclY2S ,5 },
        {0, "inclY3S" ,WEIGHT_inclY3S ,5 },
        {1, "signal1" ,WEIGHT_signal1 ,40 ,"STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},
        {1, "signal2" ,WEIGHT_signal2 ,40},
        {1, "signal3" ,WEIGHT_signal3 ,40}};

///////////////////////////////////////////////////////////////////////////////////////////////

void Norm(const char* datafile = "dataFile.root") {

    // Add the mass information, using fewer variables
    sampleName += "_" + mass;

    // Config canvas for plots
    auto *c = new TCanvas("canvas", "canvas", 1200, 1000);
    c->SetTicks();

    // Create a THStack with adequate units in the x-axis
    THStack *histStack;
    if (selSample.unit.empty())
        histStack = new THStack(selSample.title.c_str(),
                                string(sampleName + ";" + selSample.description).c_str());
    else
        histStack = new THStack(selSample.title.c_str(),
                                string(sampleName + ";" + selSample.description + " (" + selSample.unit + ")").c_str());

    // Config legend and it's position
    auto legend = new TLegend(0.45,.68,.88,0.87);
    legend->SetBorderSize(0);
    legend->SetFillColorAlpha(kWhite, 0);

///////////////////////////////////////////////////////////////////////////////////////////////

    // Read experimental data
    unique_ptr<TFile> dataCluster(TFile::Open(datafile));
    string file = sampleName + "_" + "data";
    auto *expData(dataCluster->Get<TH1>(file.c_str()));
    expData->SetMarkerStyle(20);
    expData->SetLineColor(kBlack);
    legend->AddEntry(expData, "Data", "lp");

    // Read simulated data and config variables used in the normalization
    double dataValue = expData->GetBinContent(nBin), sum = 0, fit = 0;
    for (const dataStruct& data: dataset){
        file = sampleName + "_" + data.name;
        TH1* h(dataCluster->Get<TH1>(file.c_str()));

        // If the histogram is empty, skip it
        if (h->Integral() == 0)
            continue;

        // Get the sum of all histograms and the one to be normalized
        h->Scale(data.weight);
        if(data.isUsed == 1) fit += h->GetBinContent(nBin);
        sum += h->GetBinContent(1);
    }

///////////////////////////////////////////////////////////////////////////////////////////////

    // Print the normalization value
    std::cout << (fit + dataValue - sum) / fit << std::endl;

    // Draw the histograms
    for (const dataStruct& data: dataset){
        file = sampleName + "_" + data.name;
        TH1* h(dataCluster->Get<TH1>(file.c_str()));

        // If the histogram is empty, skip it
        if (h->Integral() == 0) {
            continue;
        }

        // Personalize the histogram
        h->SetLineColor(kBlack);
        h->SetFillColor(data.color);

        // Fit the histogram and return the calculated value
        if (data.isUsed == 1){
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
    expData->Draw(" SAME");

    // Draw the legend and plot the graph
    legend->Draw("SAME");
    c->SaveAs(string(sampleName + "_NORM" + (fit + dataValue - sum) / fit + ".png").c_str());
}