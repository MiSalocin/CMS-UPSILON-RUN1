#include <RooFitResult.h>
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "TTreeReaderArray.h"
#include "RooGenericPdf.h"

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

        {0, "dymumu"  ,WEIGHT_DYmumu  ,2  ,
                "PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},

        {0, "dymumuL" ,WEIGHT_DYmumuL ,2  ,
                "PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},

        {0, "dymumuH" ,WEIGHT_DYmumuH ,2  ,
                "PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},

        {0, "inelinel",WEIGHT_inelinel,419,
                "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},

        {0, "inelel"  ,WEIGHT_inelel  ,30 ,
                "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},

        {1, "elel"    ,WEIGHT_elel    ,800,
                "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},

        {0, "inclY1S" ,WEIGHT_inclY1S ,5  ,
                "PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},

        {0, "inclY2S" ,WEIGHT_inclY2S ,5 },

        {0, "inclY3S" ,WEIGHT_inclY3S ,5 },

        {1, "signal1" ,WEIGHT_signal1 ,40 ,
                "STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},

        {1, "signal2" ,WEIGHT_signal2 ,40},

        {1, "signal3" ,WEIGHT_signal3 ,40}};


///////////////////////////////////////////////////////////////////////////////////////////////

void Fit() {
    sampleStruct sample = samples[sampleName];
    // Create an output canvas
    auto *c = new TCanvas("canvas", "canvas", 1800, 1000);
    c->SetTicks();

    // Read the experimental data and create a histogram out of it
    unique_ptr<TFile> dataCluster(TFile::Open(datafile));
    string file = sampleName + "_" + mass + "_data";
    auto *dataHist(dataCluster->Get<TH1>(file.c_str()));

    // Get the max value of the x-axis
    Double_t upperLimit = dataHist->GetBinCenter(dataHist->GetNbinsX()) +
                          dataHist->GetBinWidth(dataHist->GetNbinsX()) / 2;

///////////////////////////////////////////////////////////////////////////////////////////////

    // Create three RooFit variables, one generic and two that will hold the fitting values
    const char *title;
    if (sample.unit.empty()) title = string(sample.description + " (" + sample.unit + ")").c_str();
    else                     title = string(sample.description).c_str();
    RooRealVar x("x", title, 0, upperLimit);

    // Create two empty histograms that hold the exclusive and dissociative data
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

    // Create a model that will fit the data using the histogram templates
    RooRealVar excScale("excScale", "Exclusive scale", 5, 0, 1000);
    RooRealVar disScale("disScale", "Dissociative scale", 5, 0, 1000);
    RooAddPdf model("model", "model", RooArgList(excPDF, disPDF),
                    RooArgList(excScale, disScale));

    // Fit the data with minimum logging
    model.fitTo(data,PrintLevel(-1));

///////////////////////////////////////////////////////////////////////////////////////////////

    excHist->Scale(excScale.getVal()/excHist->Integral());
    disHist->Scale(disScale.getVal()/disHist->Integral());

    RooDataHist newExc("exc", "Exclusive fit", x, excHist);
    RooDataHist newDis("dis", "Dissociative fit", x, disHist);

    RooRealVar excA("excA", "exclusive A value", 1, 0, 10000);
    RooRealVar excB("excB", "exclusive B value", 1, 0, 100);
    RooRealVar disA("disA", "dissociative A value", 1, 0, 10000);
    RooRealVar disB("disB", "dissociative B value", 1, 0, 100);

    RooGenericPdf excFit("excFit", "excFit", "x*exp(-excB*x*x)",
                         RooArgSet(x, excB));
    RooGenericPdf disFit("disFit", "disFit", "x*exp(-disB*x*x)",
                         RooArgSet(x, disB));

    disFit.fitTo(newDis, PrintLevel(-1));
    excFit.fitTo(newExc, PrintLevel(-1));
    RooAddPdf res("res", "res", RooArgList(excFit, disFit),
                  RooArgList(excA, disA));
    res.fitTo(data, PrintLevel(-1));

///////////////////////////////////////////////////////////////////////////////////////////////

    RooPlot *xFrame = x.frame(Title("Fitting results"));

    newDis.plotOn(xFrame, Invisible());
    disFit.plotOn(xFrame, Name("disFit"),LineColor(kRed));

    newExc.plotOn(xFrame, Invisible());
    excFit.plotOn(xFrame,Name("excFit"), LineColor(kBlue));

    data.plotOn(xFrame, Name("data"));
    res.plotOn(xFrame, Name("Final fit"), LineColor(kBlack));

    gPad->SetLeftMargin(0.15);
    xFrame->GetYaxis()->SetTitleOffset(1.4);
    xFrame->SetMinimum(0);
    xFrame->Draw("SAME");

    auto *leg1 = new TLegend(0.65,0.73,0.86,0.87);
    leg1->SetFillColor(kWhite);
    leg1->SetLineColor(kWhite);
    leg1->AddEntry("data","Data", "P");
    leg1->AddEntry("disFit","Dissociative fit","L");
    leg1->AddEntry("excFit","Exclusive fit", "L");
    leg1->AddEntry("dataFit","Summed fit", "L");
    leg1->Draw();

    c->SaveAs("./fitSplitHist.png");

///////////////////////////////////////////////////////////////////////////////////////////////

    // Print the resulting values
    cout <<"----------------------------------------" <<  endl;
    cout << "Dissociative A: " << disA.getVal() << endl;
    cout << "Dissociative B: " << disB.getVal() << endl;
    cout <<"----------------------------------------" <<  endl;
    cout << "Exclusive A: " << excA.getVal() << endl;
    cout << "Exclusive B: " << excB.getVal() << endl;
    cout <<"----------------------------------------" <<  endl;
    cout << "Formula: A*x*exp(-B*x*x)" << endl;
    cout <<"----------------------------------------" <<  endl;

}