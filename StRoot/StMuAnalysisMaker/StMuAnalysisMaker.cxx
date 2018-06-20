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
#include "StBTofHeader.h"

#include "StMuAnalysisMaker.h"

//ClassImp(StMuAnalysisMaker)

// The constructor. Initialize data members here.
StMuAnalysisMaker::StMuAnalysisMaker(const Char_t *name) : StMaker(name)
{ mEventCounter = 0; mFile = 0; }

StMuAnalysisMaker::~StMuAnalysisMaker() { /* noop */ }
//
//  Called once at the beginning.
Int_t StMuAnalysisMaker::Init()
{
    //  Output file and histogram booking
    mFileName = "muAnalysis.root";

    mVxVy = new TH2F( "VxVy", "", 200, -1., 1., 200, -1., 1. );
    mVz   = new TH1F( "Vz","",600,-30.,30. );   // VzVpd vs. VzTpc
    
    return StMaker::Init();
}

//  Called once at the end.
Int_t StMuAnalysisMaker::Finish()
{
//  Summarize the run.
    cout << "StMuAnalysisMaker::Finish()\n";
    cout << "\tProcessed " << mEventCounter << " events." << endl;
//
//  Output histograms
    mFile =  new TFile(mFileName.c_str(), "RECREATE");
    cout << "\tHistograms will be stored in file '"
	 <<  mFileName.c_str() << "'" << endl;

    mVxVy->Write();
    mVz->Write();

    //  Write histos to file and close it.
    if( mFile){
      mFile->Write();  
      mFile->Close();
    }

    return kStOK;
}

//  This method is called every event.
Int_t StMuAnalysisMaker::Make()
{
    mEventCounter++;  // increase counter

//    DEBUGVALUE2(mEventCounter);
//  Get MuDst
    StMuDst* mu; 
    mu =  (StMuDst*) GetInputDS("MuDst"); 
//    DEBUGVALUE2(mu);

    if (!mu){
	  gMessMgr->Warning() << "StMuAnalysisMaker::Make : No MuDst" << endm;
          return kStOK;        // if no event, we're done
    }
//
//  Check StMuEvent branch
    StMuEvent* muEvent;
    muEvent = (StMuEvent*) mu->event();
    if(muEvent) {
      int refMult = muEvent->refMult();
    }

//
//  Printout information of StMuEvent 
    muEventInfo(*mu); 

//
//  MC track information, fix the mapping
    TClonesArray *mcTracks = mu->mcArray(MCTrack);
    int nMcTracks = mcTracks->GetEntriesFast();

    int index2McTrack[10000];
    for(int i=0;i<10000;i++) index2McTrack[i] = -1;
    
    printf("Mc track # = %d\n",nMcTracks);
    for(int i=0;i<nMcTracks;i++) {
      StMuMcTrack *mcT = (StMuMcTrack *)mcTracks->UncheckedAt(i);
      if(!mcT) continue;
      int id = mcT->Id();
      int gId = mcT->GePid();
      //if(gId!=8) continue; // not a pion
      int idVtx = mcT->IdVx();
      //if(idVtx!=1) continue;  // not from primary vertex
      //cout << "idVtx = " << idVtx << endl;
      index2McTrack[id] = i;      
    }
    
    //for( int i=0; i<10000; i++) {
    //  if( index2McTrack[i] != -1 ) cout << i  << " --> " << index2McTrack[i] << endl;
    //}	
/*

// 
//  Check track branches
    StMuTrack* muTrack; 
    int nTracks;
    nTracks= mu->globalTracks()->GetEntries();
    printf("Global track # = %d\n",nTracks);
    for (int l=0; l<nTracks; l++) { 
      muTrack =  (StMuTrack*) mu->globalTracks(l); 
      if(!muTrack) continue;

      if(!accept(muTrack)) {
	//cout << "track: " << l << " not accepted (flag, dca or nHits) " << endl;
	continue;
      }

      mGlobalPt->Fill(muTrack->pt());
      const StThreeVectorF mom = muTrack->momentum();
      const float pt = mom.perp();
      const float eta = mom.pseudoRapidity();

      if(pt<0.2 || fabs(eta)>1.0) { 
	//cout << "track: " << l << " not accepted (pt or eta) " << endl;
	continue;
      }
      int nHft = muTrack->nHft();
      int nGoodHft = muTrack->nGoodHft();
      unsigned int map0 = muTrack->topologyMap().data(0);
      //bool isHFT = ((map0>>1) & 0x1) && ((map0>>2) & 0x3) && ((map0>>4) & 0x3);
      bool hasPxl1Hit = ( map0 >> 1 ) & 0x1;
      bool hasPxl2Hit = ( map0 >> 2 ) & 0x3;
      bool hasIstHit  = ( map0 >> 4 ) & 0x3;

      // get HFT information
      StTrackTopologyMap TopologyMap = muTrack->topologyMap();
      bool hasPxl1Hit_ = TopologyMap.hasHitInPxlLayer(1);
      bool hasPxl2Hit_ = TopologyMap.hasHitInPxlLayer(2);
      bool hasIstHit_  = TopologyMap.hasHitInIstLayer(1);
        
        // only use tracks with HFT hits
        //if( !(hasPxl1Hit && hasPxl2Hit && hasIstHit) ) continue;

      cout << "track: " << l << " with HFT hits: " << hasPxl1Hit << hasPxl2Hit << hasIstHit << "   nHft:" << nHft << "  nGoodHft: " << nGoodHft << endl; 
      cout << "track: " << l << " with HFT hits: " << hasPxl1Hit_ << hasPxl2Hit_ << hasIstHit_ << endl; 

      int idTruth = muTrack->idTruth();
      
      cout << "track: " << l << "  with idTruth: " << idTruth << endl;

      if(idTruth<0) continue;
      if(idTruth>10000) { // reconstructed tracks
        mRcTPCPtEta->Fill(pt, eta);
        if(nHft>=3) mRcHFTPtEta->Fill(pt, eta);
//        cout << " real data tracks: nHft = " << nHft << "\t isHFT = " << isHFT << endl;
 
      } else {      
//        cout << " MC data tracks: nHft = " << nHft << "\t isHFT = " << isHFT << endl;
        int index2Mc = index2McTrack[idTruth];
        if(index2Mc>=0) {
          StMuMcTrack *mcT = (StMuMcTrack *)mcTracks->UncheckedAt(index2Mc);
          if(mcT) {
            float pt_mc = mcT->pT();
            mPtCorr->Fill(pt_mc, pt-pt_mc);
            mMcTPCPtEta->Fill(pt, eta);
            if(nHft>=3) mMcHFTPtEta->Fill(pt, eta);
            if(nHft>=3 && nGoodHft>=3) mMcHFTGoodPtEta->Fill(pt, eta);
          }
        } // end if (index2Mc)
      }
    }
    nTracks= mu->primaryTracks()->GetEntries();
    printf("Primary track # = %d\n",nTracks);
    for (int l=0; l<nTracks; l++) { 
      muTrack =  (StMuTrack*) mu->primaryTracks(l); 
      if(!muTrack) continue;
      if(!accept(muTrack)) continue;
      mPrimaryPt->Fill(muTrack->pt());
      
      //int idTruth = muTrack->idTruth();
      //if(idTruth>10000) continue; // reconstructed tracks
      
      //int index2Mc = index2McTrack[idTruth];
      //StMuMcTrack *mcT = (StMuMcTrack *)mcTracks->UncheckedAt(index2Mc);
            
    }

*/
    return kStOK;
}
//
//  A simple track filter
bool StMuAnalysisMaker::accept(StMuTrack* track)
{
//  check for positive flags.
    return track && track->flag() >= 0 && track->dcaGlobal().mag()<1.5 && (int)track->nHitsFit()>20;// && fabs(track->nSigmaPion())<2.0;
}
//
//  Prototype 
void StMuAnalysisMaker::muEventInfo(const StMuDst& mu)
{
  StMuEvent* ev = mu.event();
  if(!ev) return;
  
  StThreeVectorF pVtx = ev->primaryVertexPosition();
  mVxVy->Fill(pVtx.x(), pVtx.y());
  mVz  ->Fill(pVtx.z());

}
