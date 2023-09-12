#include <TSystem.h>
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

// Datasets
const dataStruct dataset[12] = {
    {1, "dymumu"  , WEIGHT_DYmumu  , 2   , "PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
    {1, "dymumuL" , WEIGHT_DYmumuL , 2   , "PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
    {1, "dymumuH" , WEIGHT_DYmumuH , 2   , "PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
    {1, "inelinel", WEIGHT_inelinel, 419 , "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
    {1, "inelel"  , WEIGHT_inelel  , 30  , "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
    {1, "elel"    , WEIGHT_elel    , 800 , "LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},
    {1, "inclY1S" , WEIGHT_inclY1S , 5   , "PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},
    {1, "inclY2S" , WEIGHT_inclY2S , 5  },
    {1, "inclY3S" , WEIGHT_inclY3S , 5  },
    {1, "signal1" , WEIGHT_signal1 , 40  , "STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},
    {1, "signal2" , WEIGHT_signal2 , 40 },
    {1, "signal3" , WEIGHT_signal3 , 40}};

///////////////////////////////////////////////////////////////////////////////////////////////

// Mass Regions
const string massList[9]  = {"LOWMM","RESOM","HIGHM","SIDEB","FULLM","EXTMM","ZPEAK","LARGE","HIGGS"};

///////////////////////////////////////////////////////////////////////////////////////////////

// Main function receives the output folder and file to read the data
void Graph(const string& outputFolder = "./Plots/",
           const char* datafile = "dataFile.root") {

    // Create output folder
    gSystem->mkdir(outputFolder.c_str());

    // Create canvas with ticks
    auto *c = new TCanvas("canvas", "canvas", 1800, 1000);
    c->SetTicks();

    // Loop through all simulated mass regions and samples
    for (const auto & i : massList){
        for(const auto & j : samples){
            sampleStruct sample = j.second;

            // If the samples have a logarithmic scale, apply it in the canvas
            if (sample.log)
                c->SetLogy();
            else
                c->SetLogy(false);

            // Create output directory
            string location = outputFolder + i + "/";
            gSystem->mkdir(location.c_str());

            // Config legend proprieties
            auto legend = new TLegend(0.45,.68,.88,0.87);
            legend->SetBorderSize(0);
			legend->SetTextSize(0.027);
            auto *histStack = new THStack(sample.title.c_str(),
                                  string(j.first + "_" + i + ";" + sample.description + " (" + sample.unit + ")").c_str());

///////////////////////////////////////////////////////////////////////////////////////////////

            // Get the experimental data
            unique_ptr<TFile> dataCluster(TFile::Open(datafile));
            string file = j.first + "_" + i + "_" + "data";
            auto *expData(dataCluster->Get<TH1>(file.c_str()));

            // If the experimental data exists, personalize it and add a legend`s entry for it
            if (expData->Integral() != 0) {
                expData->SetMarkerStyle(20);
                expData->SetLineColor(kBlack);
                legend->AddEntry(expData, "Data", "lp");
            }

///////////////////////////////////////////////////////////////////////////////////////////////

            // Create a stack of histograms containing the generated datasets
            for (const auto & data : dataset) {
                if (data.isUsed == 0) continue;
                file = j.first + "_" + i + "_" + data.name;
                TH1 *h(dataCluster->Get<TH1>(file.c_str()));

                // If the histogram is empty, skit it
                if (h->Integral() == 0) continue;

                // Personalize the histogram
                h->SetLineColor(kBlack);
                h->Scale(data.weight);
                h->SetFillColor(data.color);

                // Add a legend to the histogram (if its needed)
                if (!data.legend.empty())
                    legend->AddEntry(h, data.legend.c_str(), "f");

                // Add the histogram to a stack
                histStack->Add(h);
            }

///////////////////////////////////////////////////////////////////////////////////////////////

            // Check if there is any data to be drawn
            if (histStack->GetMaximum() == histStack->GetMinimum() && expData->Integral() == 0)
                continue;

            // If there is, check if either the expeimental or the generated data is empty and print only one histogram
            else if (histStack->GetMaximum() == histStack->GetMinimum()) {
				//expData->SetStats(0);
                expData->Draw("");
            } else if (expData->Integral() == 0)
                histStack->Draw("HIST");

            // If both histograms have data, then draw both
            else {
                if (histStack->GetMaximum() < expData->GetMaximum() + expData->GetBinError(expData->GetMaximumBin()) * 1.3)
                    histStack->SetMaximum(expData->GetMaximum() + expData->GetBinError(expData->GetMaximumBin()) * 1.3);
                histStack->Draw("HIST");
                expData  ->Draw("e1x0p SAME");
            }

            // Draw the legend and plot the graph
            legend->Draw("SAME");
            c->SaveAs(location.append(j.first + "_" + i + ".png").c_str());
        }
    }
}