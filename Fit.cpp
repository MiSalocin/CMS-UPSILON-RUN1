#include <RooFitResult.h>
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "THStack.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "TTreeReaderArray.h"

#include "datatypes.h"

using namespace std;
using namespace RooFit;

///////////////////////////////////////////////////////////////////////////////////////////////

// FITTING SELECTION
// Distribution and mass region
const string sampleName = "PtPair";
const string mass = "RESOM";

const sampleStruct selSample = samples[sampleName];

// File with data
const char* datafile = "dataFile.root";

///////////////////////////////////////////////////////////////////////////////////////////////

// DATASETS
const dataStruct dataset[12] = {
        {0, "dymumu"  ,WEIGHT_DYmumu  ,2  ,"PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuL" ,WEIGHT_DYmumuL ,2  ,"PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "dymumuH" ,WEIGHT_DYmumuH ,2  ,"PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
        {0, "inelinel",WEIGHT_inelinel,419,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
        {0, "inelel"  ,WEIGHT_inelel  ,30 ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
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
    string file = sampleName + "_" + mass + "_data";
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
        file = sampleName + "_" + mass + "_" + simData.name;
        auto h =dataCluster->Get<TH1>(file.c_str());
        h->Scale(simData.weight);
        if (simData.isUsed == 0)
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
    cout << "Exclusive fit scale result: "    << excFitResult.getVal() / excHist->Integral() << endl;
    cout << "Dissociative fit scale result: " << disFitResult.getVal() / disHist->Integral() << endl;
    cout <<"----------------------------------------" <<  endl << endl;

///////////////////////////////////////////////////////////////////////////////////////////////

    auto legend = new TLegend(0.45,.68,.88,0.87);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.027);
    auto *histStack = new THStack(sampleName.c_str(),
                          string(sampleName + "_" + mass + ";" + selSample.description + " (" + selSample.unit + ")").c_str());

    dataHist->SetMarkerStyle(20);
    dataHist->SetLineColor(kBlack);
    legend->AddEntry(dataHist, "Data", "lp");

    // Create a stack of histograms containing the generated datasets
    for (const auto & simData : dataset) {
        file = sampleName + "_" + mass + "_" + simData.name;
        TH1 *h(dataCluster->Get<TH1>(file.c_str()));

        // If the histogram is empty, skit it
        if (h->Integral() == 0)
            continue;

        // Personalize the histogram
        h->SetLineColor(kBlack);
        if (simData.isUsed == 0) h->Scale(disFitResult.getVal() / disHist->Integral());
        else h->Scale(excFitResult.getVal() / excHist->Integral());
        h->SetFillColor(simData.color);

        // Add a legend to the histogram (if its needed)
        if (!simData.legend.empty())
            legend->AddEntry(h, simData.legend.c_str(), "f");

        // Add the histogram to a stack
        histStack->Add(h);
    }

    if (histStack->GetMaximum() < dataHist->GetMaximum() + dataHist->GetBinError(dataHist->GetMaximumBin()) * 1.3)
        histStack->SetMaximum(dataHist->GetMaximum() + dataHist->GetBinError(dataHist->GetMaximumBin()) * 1.3);
    dataHist  ->Draw("e1x0p SAME");
    histStack->Draw("HIST");

    // Plot the data and resulting fit histogram
    RooPlot* xFrame = x.frame(Title("Fitting result"));
    data.plotOn(xFrame, XErrorSize(0), DrawOption("P"));
    model.plotOn(xFrame, LineWidth(3));
    xFrame->Draw("SAME");

    // Draw the legend and plot the graph
    legend->Draw("SAME");

    // Save the plots
    c->SaveAs("./fitHist.png");

///////////////////////////////////////////////////////////////////////////////////////////////

}