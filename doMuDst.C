class StMaker;
class StChain;
class StMuDstMaker;


void doMuDst(const Char_t *inputFileList = "test.list", int nEvents = 10)
{
  gROOT->Macro("loadMuDst.C");
  gSystem->Load("StMuAnalysisMaker");
  
  StChain* chain = new StChain();

  StMuDstMaker* MuDstMaker = new StMuDstMaker(0, 0, "", inputFileList, "MuDst", 100);
  MuDstMaker->SetStatus("*", 1);
  /*
  MuDstMaker->SetStatus("MuEvent", 1);
  MuDstMaker->SetStatus("PrimaryVertices", 1);
  MuDstMaker->SetStatus("PrimaryTracks", 1);
  MuDstMaker->SetStatus("GlobalTracks", 1);
  MuDstMaker->SetStatus("CovGlobTrack", 1);
  MuDstMaker->SetStatus("StMuMc*", 1); 
  MuDstMaker->SetStatus("BTof*", 1);
*/
  StMuAnalysisMaker *anaMaker = new StMuAnalysisMaker();

  chain->Init();
  cout << "chain->Init();" << endl;

  int total = 0;
  for (Int_t i = 0; i < nEvents; i++)
  {
    if (i % 100 == 0)
      cout << "Working on eventNumber " << i << endl;

    chain->Clear();
    int iret = chain->Make(i);

    if (iret)
    {
      cout << "Bad return code!" << iret << endl;
      break;
    }

    total++;
  }

  cout << "****************************************** " << endl;
  cout << "Work done... now its time to close up shop!" << endl;
  cout << "****************************************** " << endl;
  chain->Finish();
  cout << "****************************************** " << endl;
  cout << "total number of events  " << total << endl;
  cout << "****************************************** " << endl;

  delete chain;
}
