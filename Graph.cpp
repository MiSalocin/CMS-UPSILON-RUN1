#include <TSystem.h>
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

// INTEGRATED LUMI OBTAINED WITH pixelLumiCalc:
const Double_t INTEGRATED_LUMI = 937. ;

// Variables that holds the weights for each data
// CONTINUUM BKG:
Double_t WEIGHT_DYmumu   = INTEGRATED_LUMI / 1.5e6   * 1320.  * 2.      ; // FACTOR 2. FROM nET-2-6
Double_t WEIGHT_DYmumuL  = INTEGRATED_LUMI / 1e5     * 13.94            ;
Double_t WEIGHT_DYmumuH  = INTEGRATED_LUMI / 2966364 * 1297.            ;
Double_t WEIGHT_inelinel = INTEGRATED_LUMI / 1e5     * 17.902           ;
Double_t WEIGHT_inelel   = INTEGRATED_LUMI / 1e5     * 15.398 * 2.      ; // FACTOR 2 FOR INELASTIC IN BOTH SIDES (LPAIR IS ONE-SIDE ONLY)
Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 31.220 * 0.938985; // newSAMPLE
// Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 21.075,

// RESONANT BKG:
Double_t WEIGHT_inclY1S  = INTEGRATED_LUMI / 2183761 * 78963. * 0.15080      ; // FILTER EFFICIENCIES FROM PREP
Double_t WEIGHT_inclY2S  = INTEGRATED_LUMI / 1065233 * 58961. * 0.08386 * 0.4; // FACTOR 0.4 BASED ON PLOT FOR nET-2-6
Double_t WEIGHT_inclY3S  = INTEGRATED_LUMI / 533761  * 11260. * 0.57950 * 0.4;

// SIGNAL:
Double_t WEIGHT_signal1  = INTEGRATED_LUMI / 1e5 * 542.710 * 0.025 * 0.651 * 0.534879;
Double_t WEIGHT_signal2  = INTEGRATED_LUMI / 1e5 * 234.240 * 0.019 * 0.690 * 0.534879;
Double_t WEIGHT_signal3  = INTEGRATED_LUMI / 1e5 * 163.700 * 0.022 * 0.710 * 0.534879;

///////////////////////////////////////////////////////////////////////////////////////////////

// Datasets
const dataStruct dataset[12] = {
    {"dymumu"  ,WEIGHT_DYmumu  ,2    ,"PYTHIS Low-mass Drell-Yan #mu^{+}#mu^{-}"},
    {"dymumuL" ,WEIGHT_DYmumuL ,2    ,"PYTHIA mid-mass Drell-Yan #mu^{+}#mu^{-}"},
    {"dymumuH" ,WEIGHT_DYmumuH ,2    ,"PYTHIA high-mass Drell-Yan #mu^{+}#mu^{-}"},
    {"inelinel",WEIGHT_inelinel,419  ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (double dissociation)"},
    {"inelel"  ,WEIGHT_inelel  ,30   ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (single dissociation)"},
    {"elel"    ,WEIGHT_elel    ,800  ,"LPAIR #gamma#gamma #rightarrow #mu^{+}#mu^{-} (elastic)"},
    {"inclY1S" ,WEIGHT_inclY1S ,5    ,"PYTHIA/EvtGen Z2 #Upsilon(nS) #rightarrow #mu^{+}#mu^{-}"},
    {"inclY2S" ,WEIGHT_inclY2S ,5   },
    {"inclY3S" ,WEIGHT_inclY3S ,5   },
    {"signal1" ,WEIGHT_signal1 ,40   ,"STARLIGHT #gamma p #rightarrow#Upsilon(nS) p #rightarrow #mu^{+}#mu^{-} (elast)"},
    {"signal2" ,WEIGHT_signal2 ,40},
    {"signal3" ,WEIGHT_signal3 ,40}};

///////////////////////////////////////////////////////////////////////////////////////////////

// Distributions
const sampleStruct distributions[65] = {

        // EXTRA TRACKS:
        {"numExtraTracks_1to6",  "numExtraTracks_1to6",  "Number of extra tracks on dimuon vertex", ""   },
        {"numExtraTracks_1to15", "numExtraTracks_1to15", "Number of extra tracks on dimuon vertex", ""   },
        {"numExtraTracks_1to75", "numExtraTracks_1to75", "Number of extra tracks on dimuon vertex", ""   },

        // PAIR KINEMATICS:
        {"3DOpeningAngle",   "3DOpeningAngle",   "3D Opening Angle",                    ""   },
        {"Acopl",            "Acopl",            "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""   },
        {"AcoplZoom",        "AcoplZoom",        "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""   },
        {"AcoplZoomLOG",     "AcoplZoomLOG",     "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""    ,true},
        {"dPt",              "dPt",              "#Delta p_{T}",                        "GeV"},
        {"dPtZoom",          "dPtZoom",          "#Delta p_{T}",                        "GeV"},
        {"dPtZoomLOG",       "dPtZoomLOG",       "#Delta p_{T}",                        "GeV", true},
        {"EtaPair",          "EtaPair",          "#eta(#mu^{+}#mu^{-})",                ""   },
        {"invariantMass",    "invariantMass",    "m(#mu^{+}#mu^{-})",                   "GeV"},
        {"invariantMassLOG", "invariantMassLOG", "m(#mu^{+}#mu^{-})",                   "GeV" ,true},
        {"invmass_Y1S",      "invmass-Y1S",      "M(#mu^{+}#mu^{-})",                   "GeV"},
        {"invmass_Y2S",      "invmass-Y2S",      "M(#mu^{+}#mu^{-})",                   "GeV"},
        {"invmass_Y3S",      "invmass-Y3S",      "M(#mu^{+}#mu^{-})",                   "GeV"},
        {"Pt2Pair",          "Pt2Pair",          "p_{T}^{2}(#mu^{+}#mu^{-})",           "GeV" ,true},
        {"Pt2PairZoom",      "Pt2PairZoom",      "p_{T}^{2}(#mu^{+}#mu^{-})",           "GeV" ,true},
        {"PtPair",           "PtPair",           "p_{T}(#mu^{+}#mu^{-})",               "GeV"},
        {"PtPairLOG",        "PtPairLOG",        "p_{T}(#mu^{+}#mu^{-})",               "GeV" ,true},
        {"PtPair_BKG",       "PtPair-BKG",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"},
        {"PtPair_Y1S",       "PtPair-Y1S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"},
        {"PtPair_Y2S",       "PtPair-Y2S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"},
        {"PtPair_Y3S",       "PtPair-Y3S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"},
        {"RapPair",          "YPair",            "Y(#mu^{+}#mu^{-})",                   ""   },

        // SINGLE MUON KINEMATICS:
        {"EtaSingle",     "EtaSingle",     "#eta(#mu)",      ""   },
        {"EtaSingleMuM",  "EtaSingleMuM",  "#eta(#mu^{-})",  ""   },
        {"EtaSingleMuP",  "EtaSingleMuP",  "#eta(#mu^{+})",  ""   },
        {"EtaSingle_BKG", "EtaSingle-BKG", "#eta(#mu)",      "GeV"},
        {"muEnergy",      "muEnergy",      "E(#mu)",         ""   },
        {"PhiSingle",     "PhiSingle",     "#phi(#mu)",      "GeV"},
        {"PhiSingleMuM",  "PhiSingleMuM",  "#phi(#mu^{-})",  "GeV"},
        {"PhiSingleMuP",  "PhiSingleMuP",  "#phi(#mu^{+})",  "GeV"},
        {"PtSingle",      "PtSingle",      "p_{T}(#mu)",     "GeV"},
        {"PtSingleMuM",   "PtSingleMuM",   "p_{T}(#mu^{-})", "GeV"},
        {"PtSingleMuP",   "PtSingleMuP",   "p_{T}(#mu^{+})", "GeV"},
        {"PtSingle_BKG",  "PtSingle-BKG",  "p_{T}(#mu)",     "GeV"},
        {"PtSingle_Y1S",  "PtSingle-Y1S",  "p_{T}(#mu)",     "GeV"},
        {"PtSingle_Y2S",  "PtSingle-Y2S",  "p_{T}(#mu)",     "GeV"},
        {"PtSingle_Y3S",  "PtSingle-Y3S",  "p_{T}(#mu)",     "GeV"},

        // PLOTS WITH BIN OF THE EFFICIENCY CORRECTIONS:
        {"EtaSingleEffbin",                "EtaSingleEffbin",                "#eta(#mu)",      ""   },
        {"EtaSingleEffbinPerWidth",        "EtaSingleEffbinPerWidth",        "#eta(#mu)",      ""   },
        {"EtaSingleMuMEffbin",             "EtaSingleMuMEffbin",             "#eta(#mu^{-})",  ""   },
        {"EtaSingleMuMEffbinPerWidth",     "EtaSingleMuMEffbinPerWidth",     "#eta(#mu^{-})",  ""   },
        {"EtaSingleMuPEffbin",             "EtaSingleMuPEffbin",             "#eta(#mu^{+})",  ""   },
        {"EtaSingleMuPEffbinPerWidth",     "EtaSingleMuPEffbinPerWidth",     "#eta(#mu^{+})",  ""   },
        {"EtaSingleYnSEffbin_BKG",         "EtaSingleYnSEffbin_BKG",         "#eta(#mu)",      ""   },
        {"EtaSingleYnSEffbinPerWidth_BKG", "EtaSingleYnSEffbinPerWidth_BKG", "#eta(#mu)",      ""   },
        {"PtSingleEffbin",                 "PtSingleEffbin",                 "p_{T}(#mu)",     "GeV"},
        {"PtSingleEffbinPerWidth",         "PtSingleEffbinPerWidth",         "p_{T}(#mu)",     "GeV"},
        {"PtSingleMuMEffbin",              "PtSingleMuMEffbin",              "p_{T}(#mu^{-})", "GeV"},
        {"PtSingleMuMEffbinPerWidth",      "PtSingleMuMEffbinPerWidth",      "p_{T}(#mu^{-})", "GeV"},
        {"PtSingleMuPEffbin",              "PtSingleMuPEffbin",              "p_{T}(#mu^{+})", "GeV"},
        {"PtSingleMuPEffbinPerWidth",      "PtSingleMuPEffbinPerWidth",      "p_{T}(#mu^{+})", "GeV"},
        {"PtSingleYnSEffbin_BKG",          "PtSingleYnSEffbin_BKG",          "p_{T}(#mu)",     "GeV"},
        {"PtSingleYnSEffbinPerWidth_BKG",  "PtSingleYnSEffbinPerWIdth_BKG",  "p_{T}(#mu)",     "GeV"},

        // EXTRA TRACKS KINEMATICS:
        {"ExtTrkEtaAfter",   "ExtTrkEtaAfter",   "#eta (extra tracks)",         ""   },
        {"ExtTrkPhiAfter",   "ExtTrkPhiAfter",   "#phi (extra tracks)",         ""   },
        {"ExtTrkPtAfter",    "ExtTrkPtAfter",    "p_{T} (extra tracks)",        "GeV"},
//        {"ExtTrkSumPtAfter", "ExtTrkSumPtAfter", "#Sigma p_{T} (extra tracks)", "GeV"},

        // CONTROLPLOTS:
        {"numVtxAfterCuts", "numVtxAfterCuts", "Number of vertices in the event", ""   },
        {"vtxZ",            "vtxZ",            "",                                ""   },

        // CUTS:
        {"EscapingCuts",         "EscapingCuts",         "", "", true},
        {"EscapingCutsNoWeight", "EscapingCutsNoWeight", "", "", true},
        {"PassingCuts",          "PassingCuts",          "", "", true},
        {"PassingCutsNoWeight",  "PassingCutsNoWeight",  "", "", true},
};

///////////////////////////////////////////////////////////////////////////////////////////////

// Mass Regions
const string massList[9]  = {"LOWMM","RESOM","HIGHM","SIDEB","FULLM","EXTMM","ZPEAK","LARGE","HIGGS"};

///////////////////////////////////////////////////////////////////////////////////////////////

// Main function receives the output folder and file to read the data
void Graph(const string outputFolder = "./Plots/",
           const char* datafile = "dataFile.root") {

    // Create output folder
    gSystem->mkdir(outputFolder.c_str());

    // Create canvas with ticks
    auto *c = new TCanvas("canvas", "canvas", 1800, 1000);
    c->SetTicks();

    // Loop through all simulated mass regions and samples
    for (const auto & i : massList){
        for(const auto & j : distributions){

            // If the samples have a logarithmic scale, apply it in the canvas
            if (j.log)
                c->SetLogy();
            else
                c->SetLogy(false);

            // Create output directory
            string location = outputFolder + i + "/";
            gSystem->mkdir(location.c_str());

            // Config legend proprieties
            auto legend = new TLegend(0.45,.68,.88,0.87);
            legend->SetBorderSize(0);
			legend->SetTextSize(0.03);
            auto *histStack = new THStack(j.title.c_str(),
                                  string(j.type + "_" + i + ";" + j.description + " (" + j.unit + ")").c_str());

///////////////////////////////////////////////////////////////////////////////////////////////

            // Get the experimental data
            unique_ptr<TFile> dataCluster(TFile::Open(datafile));
            string file = j.type + "_" + i + "_" + "data";
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
                file = j.type + "_" + i + "_" + data.name;
                TH1 *h(dataCluster->Get<TH1>(file.c_str()));

                // If the histogram is empty, skit it
                if (h->Integral() == 0)
                    continue;

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
            c->SaveAs(location.append(j.type + "_" + i + ".png").c_str());
        }
    }
}