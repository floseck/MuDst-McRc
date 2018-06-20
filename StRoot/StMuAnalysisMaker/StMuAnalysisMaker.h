#ifndef StMuAnalysisMaker_hh     
#define StMuAnalysisMaker_hh

//  Include files
#include "StMaker.h"
#include <string>

//  Forward declarations
class StMuTrack;
class TFile;
class TH1F;
class TH2F;
class StMuDst;

#ifndef ST_NO_NAMESPACES
using std::string;
#endif

//  The class declaration. It innherits from StMaker.
class StMuAnalysisMaker : public StMaker {
public:

    StMuAnalysisMaker(const Char_t *name="muAnalysis");   // constructor
    ~StMuAnalysisMaker();                                 // destructor
    
    Int_t  Init();                   // called once at the beginning of your job
    Int_t  Make();                   // invoked for every event
    Int_t  Finish();                 // called once at the end
    void muEventInfo( const StMuDst &mu );

private:

// data member  
    int        mEventCounter;  //!
    string     mFileName;      //!
    TFile      *mFile;         //!

    TH2F *mVxVy;
    TH1F *mVz;

    // method (a simple track filter)
    bool accept(StMuTrack*);            // and this is used to select tracks

    ClassDef(StMuAnalysisMaker, 1)
};
#endif
