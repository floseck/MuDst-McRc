//  Include header files. 
#include "TFile.h"
#include "StMessMgr.h"
#include "TH1.h"
#include "TH2.h"

#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuMcTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuDebug.h"

#include "StMuAnaMaker.h"

// The constructor. Initialize data members here.
StMuAnaMaker::StMuAnaMaker( const Char_t* name ) : StMaker( name )
{
  mEventCounter = 0;
  mFile = 0;
}

StMuAnaMaker::~StMuAnaMaker()
{ /* noop */ }

//  Called once at the beginning.
Int_t StMuAnaMaker::Init()
{
    //  Output file and histogram booking
    mFileName = "muAna.root";

    mVxVy = new TH2F( "VxVy", "", 250, -5., 5., 250, -5., 5. );
    mVz   = new TH1F( "Vz", "",500, -10., 10. );
    
    return StMaker::Init();
}

//  Called once at the end.
Int_t StMuAnaMaker::Finish()
{
//  Summarize the run.
    cout << "StMuAnaMaker::Finish()\n";
    cout << "\tProcessed " << mEventCounter << " events." << endl;
//
//  Output histograms
    mFile =  new TFile( mFileName.c_str(), "RECREATE" );
    cout << "\tHistograms will be stored in file '" <<  mFileName.c_str() << "'" << endl;

    mVxVy->Write();
    mVz  ->Write();

    //  Write histos to file and close it.
    if( mFile ) {
      mFile->Write();  
      mFile->Close();
    }

    return kStOK;
}

//  This method is called every event.
Int_t StMuAnaMaker::Make()
{
  mEventCounter++;  // increase counter

  //  Get MuDst
  StMuDst* mu; 
  mu = ( StMuDst* ) GetInputDS( "MuDst" ); 

  if ( !mu ) {
    gMessMgr->Warning() << "StMuAnalysisMaker::Make : No MuDst" << endm;
    return kStOK;
  }

  //  Check StMuEvent branch
  StMuEvent* muEvent;
  muEvent = ( StMuEvent* ) mu->event();

  //  Printout information of StMuEvent 
  muEventInfo( mu ); 

  //  MC track information, fix the mapping
  TClonesArray *mcTracks = mu->mcArray( MCTrack );
  int nMcTracks = mcTracks->GetEntriesFast();

  int index2McTrack[ 10000 ];
  for( int i=0; i<10000; i++ ) {
    index2McTrack[i] = -1;
  }
  printf( "Mc track # = %d\n", nMcTracks );

  for( int i=0; i<nMcTracks; i++ ) {
    StMuMcTrack *mcT = ( StMuMcTrack* ) mcTracks->UncheckedAt( i );
    if( !mcT ) continue;
    int id = mcT->Id();
    int gId = mcT->GePid();
    //if(gId!=8) continue; // not a pion
    int idVtx = mcT->IdVx();
    //if(idVtx!=1) continue;  // not from primary vertex
    
    index2McTrack[ id ] = i;      
  }


  //  Check track branches
  StMuTrack* muTrack; 
  int nTracks = mu->globalTracks()->GetEntries();
  printf( "Global track # = %d\n", nTracks );

  for( int l=0; l<nTracks; l++ ) { 
    muTrack = ( StMuTrack* ) mu->globalTracks( l ); 
    if( !muTrack ) continue;

    if( !accept( muTrack ) ) {
      //cout << "track: " << l << " not accepted (flag, dca or nHits) " << endl;
      continue;
    }

    //mGlobalPt->Fill( muTrack->pt() );
    const StThreeVectorF mom = muTrack->momentum();
    const float pt  = mom.perp();
    const float eta = mom.pseudoRapidity();

    if( pt<0.2 || fabs( eta )>1.6 ) { 
      //cout << "track: " << l << " not accepted (pt or eta) " << endl;
      continue;
    }

    int idTruth = muTrack->idTruth();
    
    cout << "track: " << l << "  with idTruth: " << idTruth << endl;

    if( idTruth < 0 ) continue;

    if( idTruth>10000 ) { // reconstructed tracks
      //mRcTPCPtEta->Fill(pt, eta);
    }
    else {
      int index2Mc = index2McTrack[ idTruth ];
      if( index2Mc >= 0 ) {
        StMuMcTrack *mcT = ( StMuMcTrack* ) mcTracks->UncheckedAt( index2Mc );
        if( mcT ) {
            float pt_mc = mcT->pT();
            //PtCorr->Fill(pt_mc, pt-pt_mc);
            //mMcTPCPtEta->Fill(pt, eta);
        }
      } // end if (index2Mc)
    }
  }
    
  nTracks = mu->primaryTracks()->GetEntries();
  printf( "Primary track # = %d\n", nTracks );
  
  for( int l=0; l<nTracks; l++ ) { 
    muTrack = ( StMuTrack* ) mu->primaryTracks( l ); 
    if( !muTrack ) continue;
    
    if( !accept( muTrack ) ) continue;
    
    //mPrimaryPt->Fill(muTrack->pt());
      
    //int idTruth = muTrack->idTruth();
    //if(idTruth>10000) continue; // reconstructed tracks
     
    //int index2Mc = index2McTrack[idTruth];
    //StMuMcTrack *mcT = (StMuMcTrack *)mcTracks->UncheckedAt(index2Mc);
            
  }
  
  return kStOK;
}

//  A simple track filter
bool StMuAnaMaker::accept( StMuTrack* track )
{
  //  check for positive flags.
  return track && track->flag() >= 0 && track->dcaGlobal().mag() < 3. && ( int ) track->nHitsFit() > 10;
}

void StMuAnaMaker::muEventInfo( const StMuDst* mu )
{
  StMuEvent* ev = mu->event();
  if( !ev ) return;
  
  StThreeVectorF pVtx = ev->primaryVertexPosition();
  mVxVy->Fill(pVtx.x(), pVtx.y());
  mVz  ->Fill(pVtx.z());
}
