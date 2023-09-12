//
// Created by Nicolas on 9/12/2023.
//

#ifndef DATATYPES_H
#define DATATYPES_H


#include <string>
#include <map>
#include "RtypesCore.h"

struct dataStruct {
    int         isUsed;
    std::string name;
    Double_t    weight;
    Color_t     color;
    std::string legend;
};

struct sampleStruct {
    std::string title;
    std::string description;
    std::string unit;
    bool        log = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// INTEGRATED LUMI OBTAINED WITH pixelLumiCalc:
const Double_t INTEGRATED_LUMI = 937. ;

// Variables that holds the weights for each data
// CONTINUUM BKG:
const Double_t WEIGHT_DYmumu   = INTEGRATED_LUMI / 1.5e6   * 1320.  * 2.;
const Double_t WEIGHT_DYmumuL  = INTEGRATED_LUMI / 1e5     * 13.94;
const Double_t WEIGHT_DYmumuH  = INTEGRATED_LUMI / 2966364 * 1297.;
const Double_t WEIGHT_inelinel = INTEGRATED_LUMI / 1e5     * 17.902;
const Double_t WEIGHT_inelel   = INTEGRATED_LUMI / 1e5     * 15.398 * 2.;
const Double_t WEIGHT_elel     = INTEGRATED_LUMI / 1e5     * 31.220 * 0.938985;

// RESONANT BKG:
const Double_t WEIGHT_inclY1S  = INTEGRATED_LUMI / 2183761 * 78963. * 0.15080;
const Double_t WEIGHT_inclY2S  = INTEGRATED_LUMI / 1065233 * 58961. * 0.08386 * 0.4;
const Double_t WEIGHT_inclY3S  = INTEGRATED_LUMI / 533761  * 11260. * 0.57950 * 0.4;

// SIGNAL:
const Double_t WEIGHT_signal1  = INTEGRATED_LUMI / 1e5 * 542.710 * 0.025 * 0.651 * 0.534879;
const Double_t WEIGHT_signal2  = INTEGRATED_LUMI / 1e5 * 234.240 * 0.019 * 0.690 * 0.534879;
const Double_t WEIGHT_signal3  = INTEGRATED_LUMI / 1e5 * 163.700 * 0.022 * 0.710 * 0.534879;

std::map<std::string, sampleStruct>samples = {

    // EXTRA TRACKS:
    {{"numExtraTracks_1to6" },{"numExtraTracks_1to6",  "Number of extra tracks on dimuon vertex", "" }},
    {{"numExtraTracks_1to15"},{"numExtraTracks_1to15", "Number of extra tracks on dimuon vertex", "" }},
    {{"numExtraTracks_1to75"},{"numExtraTracks_1to75", "Number of extra tracks on dimuon vertex", "" }},

    // PAIR KINEMATICS:
    {{"3DOpeningAngle"  },{"3DOpeningAngle",   "3D Opening Angle",                    ""         }},
    {{"Acopl"           },{"Acopl",            "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""         }},
    {{"AcoplZoom"       },{"AcoplZoom",        "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""         }},
    {{"AcoplZoomLOG"    },{"AcoplZoomLOG",     "#1-|#Delta#phi(#mu^{+}#mu^{-})/#pi|", ""    ,true}},
    {{"dPt"             },{"dPt",              "#Delta p_{T}",                        "GeV"      }},
    {{"dPtZoom"         },{"dPtZoom",          "#Delta p_{T}",                        "GeV"      }},
    {{"dPtZoomLOG"      },{"dPtZoomLOG",       "#Delta p_{T}",                        "GeV", true}},
    {{"EtaPair"         },{"EtaPair",          "#eta(#mu^{+}#mu^{-})",                ""         }},
    {{"invariantMass"   },{"invariantMass",    "m(#mu^{+}#mu^{-})",                   "GeV"      }},
    {{"invariantMassLOG"},{"invariantMassLOG", "m(#mu^{+}#mu^{-})",                   "GeV" ,true}},
    {{"invmass_Y1S"     },{"invmass-Y1S",      "M(#mu^{+}#mu^{-})",                   "GeV"      }},
    {{"invmass_Y2S"     },{"invmass-Y2S",      "M(#mu^{+}#mu^{-})",                   "GeV"      }},
    {{"invmass_Y3S"     },{"invmass-Y3S",      "M(#mu^{+}#mu^{-})",                   "GeV"      }},
    {{"Pt2Pair"         },{"Pt2Pair",          "p_{T}^{2}(#mu^{+}#mu^{-})",           "GeV" ,true}},
    {{"Pt2PairZoom"     },{"Pt2PairZoom",      "p_{T}^{2}(#mu^{+}#mu^{-})",           "GeV" ,true}},
    {{"PtPair"          },{"PtPair",           "p_{T}(#mu^{+}#mu^{-})",               "GeV"      }},
    {{"PtPairLOG"       },{"PtPairLOG",        "p_{T}(#mu^{+}#mu^{-})",               "GeV" ,true}},
    {{"PtPair_BKG"      },{"PtPair-BKG",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"      }},
    {{"PtPair_Y1S"      },{"PtPair-Y1S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"      }},
    {{"PtPair_Y2S"      },{"PtPair-Y2S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"      }},
    {{"PtPair_Y3S"      },{"PtPair-Y3S",       "p_{T}(#mu^{+}#mu^{-})",               "GeV"      }},
    {{"RapPair"         },{"YPair",            "Y(#mu^{+}#mu^{-})",                   ""         }},

    // SINGLE MUON KINEMATICS:
    {{"EtaSingle"    },{"EtaSingle",     "#eta(#mu)",      ""   }},
    {{"EtaSingleMuM" },{"EtaSingleMuM",  "#eta(#mu^{-})",  ""   }},
    {{"EtaSingleMuP" },{"EtaSingleMuP",  "#eta(#mu^{+})",  ""   }},
    {{"EtaSingle_BKG"},{"EtaSingle-BKG", "#eta(#mu)",      "GeV"}},
    {{"muEnergy"     },{"muEnergy",      "E(#mu)",         ""   }},
    {{"PhiSingle"    },{"PhiSingle",     "#phi(#mu)",      "GeV"}},
    {{"PhiSingleMuM" },{"PhiSingleMuM",  "#phi(#mu^{-})",  "GeV"}},
    {{"PhiSingleMuP" },{"PhiSingleMuP",  "#phi(#mu^{+})",  "GeV"}},
    {{"PtSingle"     },{"PtSingle",      "p_{T}(#mu)",     "GeV"}},
    {{"PtSingleMuM"  },{"PtSingleMuM",   "p_{T}(#mu^{-})", "GeV"}},
    {{"PtSingleMuP"  },{"PtSingleMuP",   "p_{T}(#mu^{+})", "GeV"}},
    {{"PtSingle_BKG" },{"PtSingle-BKG",  "p_{T}(#mu)",     "GeV"}},
    {{"PtSingle_Y1S" },{"PtSingle-Y1S",  "p_{T}(#mu)",     "GeV"}},
    {{"PtSingle_Y2S" },{"PtSingle-Y2S",  "p_{T}(#mu)",     "GeV"}},
    {{"PtSingle_Y3S" },{"PtSingle-Y3S",  "p_{T}(#mu)",     "GeV"}},

    // PLOTS WITH BIN OF THE EFFICIENCY CORRECTIONS:
    {{"EtaSingleEffbin"               },{"EtaSingleEffbin",                "#eta(#mu)",      ""   }},
    {{"EtaSingleEffbinPerWidth"       },{"EtaSingleEffbinPerWidth",        "#eta(#mu)",      ""   }},
    {{"EtaSingleMuMEffbin"            },{"EtaSingleMuMEffbin",             "#eta(#mu^{-})",  ""   }},
    {{"EtaSingleMuMEffbinPerWidth"    },{"EtaSingleMuMEffbinPerWidth",     "#eta(#mu^{-})",  ""   }},
    {{"EtaSingleMuPEffbin"            },{"EtaSingleMuPEffbin",             "#eta(#mu^{+})",  ""   }},
    {{"EtaSingleMuPEffbinPerWidth"    },{"EtaSingleMuPEffbinPerWidth",     "#eta(#mu^{+})",  ""   }},
    {{"EtaSingleYnSEffbin_BKG"        },{"EtaSingleYnSEffbin_BKG",         "#eta(#mu)",      ""   }},
    {{"EtaSingleYnSEffbinPerWidth_BKG"},{"EtaSingleYnSEffbinPerWidth_BKG", "#eta(#mu)",      ""   }},
    {{"PtSingleEffbin"                },{"PtSingleEffbin",                 "p_{T}(#mu)",     "GeV"}},
    {{"PtSingleEffbinPerWidth"        },{"PtSingleEffbinPerWidth",         "p_{T}(#mu)",     "GeV"}},
    {{"PtSingleMuMEffbin"             },{"PtSingleMuMEffbin",              "p_{T}(#mu^{-})", "GeV"}},
    {{"PtSingleMuMEffbinPerWidth"     },{"PtSingleMuMEffbinPerWidth",      "p_{T}(#mu^{-})", "GeV"}},
    {{"PtSingleMuPEffbin"             },{"PtSingleMuPEffbin",              "p_{T}(#mu^{+})", "GeV"}},
    {{"PtSingleMuPEffbinPerWidth"     },{"PtSingleMuPEffbinPerWidth",      "p_{T}(#mu^{+})", "GeV"}},
    {{"PtSingleYnSEffbin_BKG"         },{"PtSingleYnSEffbin_BKG",          "p_{T}(#mu)",     "GeV"}},
    {{"PtSingleYnSEffbinPerWidth_BKG" },{"PtSingleYnSEffbinPerWIdth_BKG",  "p_{T}(#mu)",     "GeV"}},

    // EXTRA TRACKS KINEMATICS:
    {{"ExtTrkEtaAfter"},{"ExtTrkEtaAfter",   "#eta (extra tracks)",         ""   }},
    {{"ExtTrkPhiAfter"},{"ExtTrkPhiAfter",   "#phi (extra tracks)",         ""   }},
    {{"ExtTrkPtAfter" },{"ExtTrkPtAfter",    "p_{T} (extra tracks)",        "GeV"}},
//    {{"ExtTrkSumPtAfter"},{ "ExtTrkSumPtAfter", "#Sigma p_{T} (extra tracks)", "GeV"}},

    // CONTROLPLOTS:
    {{"numVtxAfterCuts"},{"numVtxAfterCuts", "Number of vertices in the event", ""   }},
    {{"vtxZ"           },{"vtxZ",            "",                                ""   }},

    // CUTS:
    {{"EscapingCuts"        },{"EscapingCuts",         "", "", true}},
    {{"EscapingCutsNoWeight"},{"EscapingCutsNoWeight", "", "", true}},
    {{"PassingCuts"         },{"PassingCuts",          "", "", true}},
    {{"PassingCutsNoWeight" },{"PassingCutsNoWeight",  "", "", true}},
};

#endif //DATATYPES_H
