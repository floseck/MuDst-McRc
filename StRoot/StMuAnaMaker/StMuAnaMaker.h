#ifndef STMUANAMAKER_H     
#define STMUANAMAKER_H

//  Include files
#include "StMaker.h"
#include <string>

//  Forward declarations
class StMuTrack;
class TFile;
class TH1F;
class TH2F;
class StMuDst;

class StMuAnaMaker : public StMaker {
public:

    StMuAnaMaker( const Char_t *name="muAna" );   // constructor
    ~StMuAnaMaker();                              // destructor
    
    Int_t  Init();                   // called once at the beginning of your job
    Int_t  Make();                   // invoked for every event
    Int_t  Finish();                 // called once at the end
    void muEventInfo( const StMuDst* mu );

private:

// data member  
    int           mEventCounter; //!
    std::string   mFileName;     //!
    TFile*        mFile;         //!

    TH2F* mVxVy;
    TH1F* mVz;
    TH2F* mPtCorr;
    TH2F* mPzCorr;
    TH2F* mMomCorr;

    bool accept( StMuTrack* );            

    ClassDef( StMuAnaMaker, 1 )
};
#endif
