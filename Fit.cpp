#include <RooGenericPdf.h>
#include <RooRealVar.h>
#include <TPaveStats.h>
#include "datatypes.h"
#include <RooAddPdf.h>
#include <RooPlot.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>

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

    // Create parameters variables
    RooRealVar excA("excA", "exclusive A value", 1, 0, 10000);
    RooRealVar excB("excB", "exclusive B value", 1, 0, 100);
    RooRealVar disA("disA", "dissociative A value", 1, 0, 10000);
    RooRealVar disB("disB", "dissociative B value", 1, 0, 100);

    // Create two fit templates
    RooGenericPdf excFit("excFit", "excFit", "x*exp(-excB*x*x)",
                         RooArgSet(x, excB));
    RooGenericPdf disFit("disFit", "disFit", "x*exp(-disB*x*x)",
                         RooArgSet(x, disB));

    // fit them to the histograms, defining the "B" parameter
    disFit.fitTo(dis, PrintLevel(-1));
    excFit.fitTo(exc, PrintLevel(-1));

    // Create a new PDF that's the sum of the other 2 fitted
    RooAddPdf finalPDF("finalPDF", "finalPDF", RooArgList(excFit, disFit),
                       RooArgList(excA, disA));
    finalPDF.fitTo(data, PrintLevel(-1));

///////////////////////////////////////////////////////////////////////////////////////////////

    // Create a function that has the fitting parameters
    auto *function = new TF1("finalFunc", "[0]*x*exp(-[1]*x*x) + [2]*x*exp(-[3]*x*x)", 0, 5);
    auto *f2 = new TF1("f2", "[0]*x*exp(-[1]*x*x)", 0, 5);
    auto *leg1 = new TLegend(0.60,0.75,0.85,0.88);
    leg1->SetTextSize(0.025);

    // Plot the fitted data in a frame
    RooPlot *xFrame = x.frame(Title("Fitting results"));
    data.plotOn(xFrame,Name("data"));
    finalPDF.plotOn(xFrame, Name("sumfit"), LineColor(kBlack));
    leg1->AddEntry("data","Data", "P");
    leg1->AddEntry("sumfit","Summed fit", "L");

    // Draw the frame
    xFrame->GetYaxis()->SetTitleOffset(1.4);
    xFrame->SetMinimum(0);
    gPad->SetLeftMargin(0.15);
    xFrame->Draw();

    // Variable that holds the fit results and errors
    double par[4][2];

    // Setup variable values
    f2->SetParameters(1,disB.getVal());
    par[0][0] = disA.getVal() / (f2->Integral(0, 2) * 10);
    par[0][1] = disA.getError() / (f2->Integral(0, 2) * 10);
    par[1][0] = disB.getVal();
    par[1][1] = disB.getError();
    f2->SetParameters(1, excB.getVal());
    par[2][0] = excA.getVal() / (f2->Integral(0, 2) * 10);
    par[2][1] = excA.getError() / (f2->Integral(0, 2) * 10);
    par[3][0] = excB.getVal();
    par[3][1] = excB.getError();

    // Draw dissociative fit
    f2->SetParameters(par[0][0], par[1][0]);
    f2->SetLineColor(kBlue);
    f2->DrawClone("SAME");
    leg1->AddEntry(f2->Clone(),"Exclusive fit", "L");

    // Draw exclusive fit
    f2->SetParameters(par[2][0], par[3][0]);
    f2->SetLineColor(kGreen);
    f2->Draw("SAME");
    leg1->AddEntry(f2,"Dissociative fit","L");

    // Draw legend
    leg1->SetFillColorAlpha(0, 0);
    leg1->SetLineColorAlpha(0, 0);
    leg1->Draw();

    // Create a statistics window using TPaveStats
    auto *stats = new TPaveStats();

    // Set the text and position of the TPaveStats object.
    stats->SetX1NDC(0.8);
    stats->SetY1NDC(0.65);
    stats->SetX2NDC(0.95);
    stats->SetY2NDC(0.95);
    stats->SetTextSize(0.025);

    // Set model function parameters
    function->SetParameter(0, par[0][0]);
    function->SetParameter(1, par[1][0]);
    function->SetParameter(2, par[2][0]);
    function->SetParameter(3, par[3][0]);

    // Set up the statistics window.
    stats->AddText("Results");
    stats->AddText(("Integral = " + to_string(function->Integral(0,3)*10)).c_str());
    stats->AddText(("Mean = " + to_string(function->Mean(0,3))).c_str());
    auto *text = new TLatex();
    text->SetText(0,0,"±");
    stats->AddText(("Dis. A = "+to_string(par[0][0]).substr(0,5)+" #pm "+to_string(par[0][1]).substr(0,5)).c_str());
    stats->AddText(("Dis. B = "+to_string(par[1][0]).substr(0,5)+" #pm "+to_string(par[1][1]).substr(0,5)).c_str());
    stats->AddText(("Exc. A = "+to_string(par[2][0]).substr(0,5)+" #pm "+to_string(par[2][1]).substr(0,5)).c_str());
    stats->AddText(("Exc. B = "+to_string(par[3][0]).substr(0,5)+" #pm "+to_string(par[3][1]).substr(0,5)).c_str());

    // Add the TPaveStats object to the canvas.
    stats->Draw();

    // Save the plot
    c->SaveAs("./fitSplitHist.png");

}