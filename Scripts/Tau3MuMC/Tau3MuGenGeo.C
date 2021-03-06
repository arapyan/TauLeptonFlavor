//================================================================================================
//
// Select tau->mumumu candidates
//
//  * outputs ROOT files of events passing selection
//
//________________________________________________________________________________________________

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TROOT.h>                  // access to gROOT, entry point to ROOT system
#include <TSystem.h>                // interface to OS
#include <TFile.h>                  // file handle class
#include <TTree.h>                  // class to access ntuples
#include <TClonesArray.h>           // ROOT array class
#include <TBenchmark.h>             // class to track macro running statistics
#include <TVector2.h>               // 2D vector class
#include <TMath.h>                  // ROOT math library
#include <vector>                   // STL vector class
#include <iostream>                 // standard I/O
#include <iomanip>                  // functions to format standard I/O
#include <fstream>                  // functions for file I/O
#include "TLorentzVector.h"         // 4-vector class
#include <THStack.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TPad.h>
#include <TF1.h>
#include <TH2F.h>
#include "TH1D.h"
#include "TRandom.h"
#include "ConfParse.hh"             // input conf file parser
#include "../Utils/CSample.hh"      // helper class to handle samples
// define structures to read in ntuple
#include "BaconAna/DataFormats/interface/BaconAnaDefs.hh"
#include "BaconAna/DataFormats/interface/TEventInfo.hh"
#include "BaconAna/DataFormats/interface/TGenEventInfo.hh"
#include "BaconAna/DataFormats/interface/TGenParticle.hh"
#include "BaconAna/DataFormats/interface/TMuon.hh"
#include "BaconAna/DataFormats/interface/TVertex.hh"
#include "BaconAna/Utils/interface/TTrigger.hh"
// lumi section selection with JSON files
#include "BaconAna/Utils/interface/RunLumiRangeMap.hh"
#include "../Utils/LeptonIDCuts.hh" // helper functions for lepton ID selection
#include "../Utils/MyTools.hh"      // various helper functions
#endif

bool Findtaudecay(vector<baconhep::TGenParticle *> muonarr,vector<TLorentzVector> arr,Int_t* record,const int arr_len, baconhep::TGenParticle *genPartArr)
{
  Int_t multicount = 0;
  Bool_t isTaudecay = kFALSE;
  for(int i=0; i<arr_len; i++){
    for(int j=i+1; j<arr_len; j++){
      for(int k=j+1; k<arr_len; k++){
	//code here:
	if(!(muonarr[i]->parent == muonarr[j]->parent && muonarr[i]->parent == muonarr[k]->parent)) continue;
	if(muonarr[0]->pdgId == muonarr[1]->pdgId && muonarr[0]->pdgId == muonarr[2]->pdgId) continue;
	TLorentzVector vcom,v1,v2,v3;
	isTaudecay = kTRUE;
	record[0]=i;
	record[1]=j;
	record[2]=k;
	multicount++;
      }	
    }
  }
  //if(multicount > 1) cout<<"more than one system have come from same tau"<<endl;
  return isTaudecay;
}

void select3Mu(const TString conf="samples.conf", // input file
              const TString outputDir=".",  // output directory
	      const Bool_t  doScaleCorr=0   // apply energy scale corrections?
) {
  gBenchmark->Start("select3Mu");

  //--------------------------------------------------------------------------------------------------------------
  // Settings 
  //============================================================================================================== 
  const Bool_t isGENLevel = kTRUE;
  const Double_t PT_CUT_LEAD    = 2.5;
  const Double_t PT_CUT_SUB = 1;
  const Double_t ETA_CUT   = 2.4;
  const Double_t MUON_MASS = 0.105658369;
  
  const Int_t BOSON_ID  = 24;
  const Int_t LEPTON_ID = 13;

  TH1F* hist0 = new TH1F("tau -> 3 muon 0","invariant mass",100,0,10);
  TH1F* hist1 = new TH1F("tau -> 3 muon 1","invariant mass",100,0,10);
  TH1F* hist2 = new TH1F("tau -> 3 muon 2","pT",30,0,15);
  TH1F* hist3 = new TH1F("tau -> 3 muon 3","Eta",50,-3,3);
  TH1F* hist4 = new TH1F("tau -> 3 muon 4","Phi",25,-3.5,3.5);
  TH1F* hist5 = new TH1F("tau -> 3 muon 5","pT",30,0,15);
  TH1F* hist6 = new TH1F("tau -> 3 muon 6","Eta",50,-3,3);
  TH1F* hist7 = new TH1F("tau -> 3 muon 7","Phi",25,-3.5,3.5);
  TH1F* hist8 = new TH1F("tau -> 3 muon 8","pT",30,0,15);
  TH1F* hist9 = new TH1F("tau -> 3 muon 9","Eta",50,-3,3);
  TH1F* hist10 = new TH1F("tau -> 3 muon 10","Phi",25,-3.5,3.5);
  TH1F* hist11 = new TH1F("tau -> 3 muon 11","pT",25,0,10);
  TH1F* hist12 = new TH1F("tau -> 3 muon 12","Eta",50,-3,3);
  TH1F* hist13 = new TH1F("tau -> 3 muon 13","Phi",25,-3.5,3.5);
  TH1F* hist14 = new TH1F("tau -> 3 muon 14","pT",25,0,10);
  TH1F* hist15 = new TH1F("tau -> 3 muon 15","Eta",50,-3,3);
  TH1F* hist16 = new TH1F("tau -> 3 muon 16","Phi",25,-3.5,3.5);
  TH1F* hist17 = new TH1F("tau -> 3 muon 17","pT",25,0,10);
  TH1F* hist18 = new TH1F("tau -> 3 muon 18","Eta",50,-3,3);
  TH1F* hist19 = new TH1F("tau -> 3 muon 19","Phi",25,-3.5,3.5);
  TH1F* hist20 = new TH1F("tau -> 3 muon 20","Cos #theta",110,0,1.1);
  TH1F* hist21 = new TH1F("tau -> 3 muon 21","Cos #theta",110,0,1.1);
  TH1F* hist22 = new TH1F("tau -> 3 muon 22","Cos #theta",110,0,1.1);
  TH1F* hist23 = new TH1F("tau -> 3 muon 23","DeltaR lead",100,0,5);
  TH1F* hist23a = new TH1F("tau -> 3 muon 23a","DeltaR sublead",100,0,5);
  TH1F* hist23b = new TH1F("tau -> 3 muon 23b","DeltaR third",100,0,5);
  TH1F* hist24 = new TH1F("tau -> 3 muon 24","pT",100,0,50);
  TH1F* hist25 = new TH1F("tau -> 3 muon 25","Eta",50,-3,3);
  TH1F* hist26 = new TH1F("tau -> 3 muon 26","pdgId",60,-30,30);
  TH2F* hista = new TH2F("tau -> 3 muon a","deltaRSum vs #tau pT",100,0,10,100,0,50);
  TH2F* hista1 = new TH2F("tau -> 3 muon a1","deltaR1 vs #tau pT",100,0,10,100,0,50);
  TH2F* hista2 = new TH2F("tau -> 3 muon a2","deltaR2 vs #tau pT",100,0,10,100,0,50);
  TH2F* hista3 = new TH2F("tau -> 3 muon a3","deltaR3 vs #tau pT",100,0,10,100,0,50);
  TH2F* histb = new TH2F("tau -> 3 muon b","Dalitz",100,0,3,100,0,3);
  UInt_t count1=0, count2=0, count3=0, count4=0, count5=0, count6=0, count7=0;
  gStyle->SetOptStat(111111);

  //--------------------------------------------------------------------------------------------------------------
  // Main analysis code 
  //==============================================================================================================  

  vector<TString>  snamev;      // sample name (for output files)  
  vector<CSample*> samplev;     // data/MC samples

  // parse .conf file
  confParse(conf, snamev, samplev);
  const Bool_t hasData = (samplev[0]->fnamev.size()>0);

  // Create output directory
  gSystem->mkdir(outputDir,kTRUE);
  const TString ntupDir = outputDir + TString("/ntuples");
  gSystem->mkdir(ntupDir,kTRUE);
  
  // Declare output ntuple variables
  UInt_t  runNum, lumiSec, evtNum;
  UInt_t  npv, npu;
  Float_t scale1fb, scale1fbUp, scale1fbDown, puWeight,puWeightUp,puWeightDown;
  Float_t met, metPhi, sumEt;
  Float_t tkMet, tkMetPhi, tkSumEt;
  //Float_t mvaMet, mvaMetPhi, mvaSumEt, mvaMt, mvaU1, mvaU2;
  //Float_t puppiMet, puppiMetPhi, puppiSumEt, puppiMt, puppiU1, puppiU2;
  Int_t   q[3];
  TLorentzVector *lep1=0, *lep2=0, *lep3=0;
  Int_t lepID[3];
  ///// muon specific /////
  Float_t trkIso[3], emIso[3], hadIso[3];
  Float_t pfChIso[3], pfGamIso[3], pfNeuIso[3], pfCombIso[3];
  Float_t d0[3], dz[3];
  Float_t muNchi2[3];
  UInt_t nPixHits[3], nTkLayers[3], nValidHits[3], nMatch[3], typeBits[3];
  UInt_t TolMuNum;
  UInt_t TolPassPreSel=0;
  
  
  // Data structures to store info from TTrees
  baconhep::TEventInfo *info  = new baconhep::TEventInfo();
  baconhep::TGenEventInfo *gen  = new baconhep::TGenEventInfo();
  TClonesArray *genPartArr = new TClonesArray("baconhep::TGenParticle");
  TClonesArray *muonArr    = new TClonesArray("baconhep::TMuon");
  TClonesArray *vertexArr  = new TClonesArray("baconhep::TVertex");
  
  TFile *infile=0;
  TTree *eventTree=0;

  // loop over samples
  for(UInt_t isam=0; isam<samplev.size(); isam++) {
    
    // Assume data sample is first sample in .conf file
    // If sample is empty (i.e. contains no ntuple files), skip to next sample
    Bool_t isData=kFALSE;
    if(isam==0 && !hasData) continue;
    else if (isam==0) isData=kTRUE;

    // Assume signal sample is given name "dstau" -- flag to store GEN Ds kinematics
    Bool_t isSignal = (snamev[isam].CompareTo("dstau",TString::kIgnoreCase)==0);
    CSample* samp = samplev[isam];

    cout<<"begin loop over files"<<endl;
    // loop through files
    const UInt_t nfiles = samp->fnamev.size();
    for(UInt_t ifile=0; ifile<nfiles; ifile++) {  
      
      // Read input file and get the TTrees
      cout << "Processing " << samp->fnamev[ifile] << " [xsec = " << samp->xsecv[ifile] << " pb] ... "; cout.flush();
      infile = TFile::Open(samp->fnamev[ifile]); 
      assert(infile);

      // Access Event Tree
      eventTree = (TTree*)infile->Get("Events");
      assert(eventTree);  
      eventTree->SetBranchAddress("Info", &info);    TBranch *infoBr = eventTree->GetBranch("Info");
      eventTree->SetBranchAddress("Muon", &muonArr); TBranch *muonBr = eventTree->GetBranch("Muon");
      eventTree->SetBranchAddress("PV",   &vertexArr); TBranch *vertexBr = eventTree->GetBranch("PV");
      Bool_t hasGen = eventTree->GetBranchStatus("GenEvtInfo");
      TBranch *genBr=0, *genPartBr=0;
      eventTree->SetBranchAddress("GenEvtInfo", &gen); genBr = eventTree->GetBranch("GenEvtInfo");
      eventTree->SetBranchAddress("GenParticle",&genPartArr); genPartBr = eventTree->GetBranch("GenParticle");

      //Loop over events
      int passpresel = 0;
      for(UInt_t ientry=0; ientry<eventTree->GetEntries(); ientry++) {
        infoBr->GetEntry(ientry);
	
        if(ientry%50000==0) cout << "Processing event " << ientry << ". " << (double)ientry/(double)eventTree->GetEntries()*100 << " percent done with this file." << endl;
	Int_t taumuon = 0;
	Int_t Nummuon[50];
	Int_t runcount = 0;
	Bool_t passSel=kFALSE;

	//===========================BEGIN OF GEN LEVEL======================
	if(isGENLevel){
	  genPartArr->Clear();
	  genPartBr->GetEntry(ientry);
	  //Loop over genParArr to find muon
	  for(Int_t i=0; i<genPartArr->GetEntriesFast(); i++) {
	    const baconhep::TGenParticle *genpar = (baconhep::TGenParticle*)((*genPartArr)[i]);
	    
	    //Select muon
	    if(genpar->pdgId != 13 && genpar->pdgId != -13) continue;
	    if(genpar->status != 1) continue;
            Int_t parentid1=dynamic_cast<baconhep::TGenParticle *>(genPartArr->At(genpar->parent>-1 ? genpar->parent : 0))->pdgId;
	    if(parentid1 != 15 && parentid1 != -15) continue;

	    taumuon++;
	    Nummuon[runcount] = i;
	    runcount++;
	    
	  }//End of muon array loop 1
	}//============================END OF GEN LEVEL================================
	
	if (taumuon > 2){
	  count1++;
	  passSel=kTRUE;//only events with 3 muons can pass the selection
	}

		//Store GEN level tau muon
	vector<baconhep::TGenParticle*> genmuonArr;
	for(int i=0; i<genPartArr->GetEntries(); i++){
	  baconhep::TGenParticle *genpar = (baconhep::TGenParticle*)((*genPartArr)[i]);
	  if(genpar->pdgId != 13 && genpar->pdgId != -13) continue;
	  if(genpar->status != 1) continue;
	  Int_t parentid1=dynamic_cast<baconhep::TGenParticle *>(genPartArr->At(genpar->parent>-1 ? genpar->parent : 0))->pdgId;
	  if(parentid1 != 15 && parentid1 != -15) continue;
	  genmuonArr.push_back(genpar);
	}

	//Only study events contain single decay 229041
	if(genmuonArr.size() > 3) continue;

	Int_t parent = genmuonArr[0]->parent;
	baconhep::TGenParticle *gtau = dynamic_cast<baconhep::TGenParticle *>(genPartArr->At(parent>-1 ? parent : 0));
	hist24->Fill(gtau->pt);
	hist25->Fill(gtau->eta);
	hist26->Fill(gtau->pdgId);
	
	
	if(passSel){	  
	  if (isGENLevel){
	    vector<baconhep::TGenParticle *> TauMuonArr;
	    //const baconhep::TGenParticle* GenMuonArr[muon];
	    const baconhep::TGenParticle *SelMuonArr[6];
	    Int_t record[3]={0,0,0};
	    vector<TLorentzVector> vLep;
	    TLorentzVector vLepRe[3], vCom;

	    //store global muon into array
	    for(int p=0; p<taumuon; p++){
	      TLorentzVector vtemp;
	      TauMuonArr.push_back((baconhep::TGenParticle*)((*genPartArr)[Nummuon[p]])); //store muon
	      vtemp.SetPtEtaPhiM(TauMuonArr[p]->pt, TauMuonArr[p]->eta, TauMuonArr[p]->phi, MUON_MASS);
	      vLep.push_back(vtemp);//store corresponding 4-vector
	    }
	    
	    baconhep::TGenParticle *genpartarr = (baconhep::TGenParticle*)(genPartArr);
	    if(Findtaudecay(TauMuonArr,vLep,record,taumuon,genpartarr)){     
	      for(int p=0; p<3; p++){
	      SelMuonArr[p] = TauMuonArr[record[p]];
	      }	    
	      Double_t maxpt=0, submaxpt=0;
	      for(int l=0; l<3; l++){
		if(SelMuonArr[l]->pt >= maxpt){
		  submaxpt = maxpt;
		  maxpt = SelMuonArr[l]->pt;	  
		  SelMuonArr[5]=SelMuonArr[4];
		  SelMuonArr[4]=SelMuonArr[3];
		  SelMuonArr[3]=SelMuonArr[l];
		}
		else if(SelMuonArr[l]->pt < maxpt && SelMuonArr[l]->pt >= submaxpt){
		  submaxpt = SelMuonArr[l]->pt;
		  SelMuonArr[5]=SelMuonArr[4];
		  SelMuonArr[4]=SelMuonArr[l];
		}
		else{
		  SelMuonArr[5]=SelMuonArr[l];
		}
	      }
	      for(int p=3; p<6; p++){
		vLepRe[p-3].SetPtEtaPhiM(SelMuonArr[p]->pt, SelMuonArr[p]->eta, SelMuonArr[p]->phi, MUON_MASS);
	      }
	      vCom = vLepRe[0]+vLepRe[1]+vLepRe[2];
	      Double_t invmass = vCom.M();
	      count2++;

	      bool passcut = kTRUE;
	      /*
	      if(fabs(SelMuonArr[0]->eta) > ETA_CUT) passcut=kFALSE;  // lepton |eta| cut
	      if(fabs(SelMuonArr[1]->eta) > ETA_CUT) passcut=kFALSE;  // lepton |eta| cut
	      if(fabs(SelMuonArr[2]->eta) > ETA_CUT) passcut=kFALSE;  // lepton |eta| cut
	      if(passcut) count2++;
	      if(GenMuonArr[0]->pt<PT_CUT_LEAD || GenMuonArr[1]->pt<PT_CUT_LEAD || GenMuonArr[2]->pt<PT_CUT_SUB) passcut=kFALSE;  // lepton pT cut
	      */
	     
	      if(passcut){
		hist0->Fill(invmass);//store events come from tau
		hist2->Fill(SelMuonArr[3]->pt); hist5->Fill(SelMuonArr[4]->pt); hist8->Fill(SelMuonArr[5]->pt);
		hist3->Fill(SelMuonArr[3]->eta); hist6->Fill(SelMuonArr[4]->eta); hist9->Fill(SelMuonArr[5]->eta);
		hist4->Fill(SelMuonArr[3]->phi); hist7->Fill(SelMuonArr[4]->phi); hist10->Fill(SelMuonArr[5]->phi);

		//compute cos theta
		TLorentzVector vG[3];
		TVector3 G[3];
		for(int p=0; p<3; p++){
		  vG[p].SetPtEtaPhiM(SelMuonArr[p+3]->pt, SelMuonArr[p+3]->eta, SelMuonArr[p+3]->phi, MUON_MASS);
		  G[p].SetX(vG[p].Px());
		  G[p].SetY(vG[p].Py());
		  G[p].SetZ(vG[p].Pz());
		}
		hist0->Fill((vG[0]+vG[1]+vG[2]).M());//store events come from tau
		TLorentzVector Psum;
		Psum = vG[0]+vG[1]+vG[2];
		Double_t deltaR1 = toolbox::deltaR(Psum.Eta(),Psum.Phi(),vG[0].Eta(),vG[0].Phi());
		Double_t deltaR2 = toolbox::deltaR(Psum.Eta(),Psum.Phi(),vG[1].Eta(),vG[1].Phi());
		Double_t deltaR3 = toolbox::deltaR(Psum.Eta(),Psum.Phi(),vG[2].Eta(),vG[2].Phi());
		Double_t deltaRSum = deltaR1+deltaR2+deltaR3;
		hist23->Fill(deltaR1);
		hist23a->Fill(deltaR2);
		hist23b->Fill(deltaR3);
		TVector3 sum;
		sum.SetX(Psum.Px());
		sum.SetY(Psum.Py());
		sum.SetZ(Psum.Pz());
		Double_t theta1 = sum.Dot(G[0])/sum.Mag()/G[0].Mag();
		Double_t theta2 = sum.Dot(G[1])/sum.Mag()/G[1].Mag();
		Double_t theta3 = sum.Dot(G[2])/sum.Mag()/G[2].Mag();
		hist20->Fill(theta1);
		hist21->Fill(theta2);
		hist22->Fill(theta3);
		count3++;
		hista->Fill(deltaRSum,gtau->pt);
		hista1->Fill(deltaR1,gtau->pt);
		hista2->Fill(deltaR2,gtau->pt);
		hista3->Fill(deltaR3,gtau->pt);
		if(gtau->pdgId == 15)
		if(SelMuonArr[3]->pdgId == -13)
		  histb->Fill(TMath::Power((vG[0]+vG[1]).M(),2),TMath::Power((vG[0]+vG[2]).M(),2));
		if(SelMuonArr[4]->pdgId == -13)
		  histb->Fill(TMath::Power((vG[1]+vG[0]).M(),2),TMath::Power((vG[1]+vG[2]).M(),2));
		if(SelMuonArr[5]->pdgId == -13)
		  histb->Fill(TMath::Power((vG[2]+vG[0]).M(),2),TMath::Power((vG[2]+vG[1]).M(),2));
		if(gtau->pdgId == -15)
		if(SelMuonArr[3]->pdgId == 13)
		  histb->Fill(TMath::Power((vG[0]+vG[1]).M(),2),TMath::Power((vG[0]+vG[2]).M(),2));
		if(SelMuonArr[4]->pdgId == 13)
		  histb->Fill(TMath::Power((vG[1]+vG[0]).M(),2),TMath::Power((vG[1]+vG[2]).M(),2));
		if(SelMuonArr[5]->pdgId == 13)
		  histb->Fill(TMath::Power((vG[2]+vG[0]).M(),2),TMath::Power((vG[2]+vG[1]).M(),2));
	      }
	    }
	  }
        }
      }//end of event loop
       cout<<"There is "<<passpresel<<" events passed pre selection"<<endl;
      delete infile;
      infile=0, eventTree=0;    
    }
  }
  delete info;
  delete gen;
  delete genPartArr;
  delete muonArr;
  delete vertexArr;
    
  //--------------------------------------------------------------------------------------------------------------
  // Output
  //==============================================================================================================
  cout << "*" << endl;
  cout << "* SUMMARY" << endl;
  cout << "*--------------------------------------------------" << endl;
  cout << " tau -> mu mu mu" << endl;
  cout << "  pT > " << PT_CUT_LEAD << endl;
  cout << "  |eta| < " << ETA_CUT << endl;
  cout << endl;
  
  cout << endl;
  cout << "  <> Output saved in " << outputDir << "/" << endl;    
  cout << endl;

  //Draw Graph
  gStyle->SetOptStat(0);
  //THStack *all = new THStack("tau -> 3 muons","invariant mass");
  TCanvas *c0 = new TCanvas("c0","invariant mass",1200,900);
  TAxis *xaxis = hist0->GetXaxis();
  TAxis *yaxis = hist0->GetYaxis();

  xaxis->SetTitle("Invariant mass (GeV)");
  yaxis->SetTitle("Entries / 0.1 GeV");
  yaxis->SetTitleOffset(1.2);
  yaxis->SetRangeUser(0.5,400000);
  c0->cd();
  

  hist0->SetFillColor(5);
  //hist1->SetFillColor(7);
  //hist3->SetFillColor(7);
  //hist4->SetFillColor(8);


  hist0->Draw();
  //hist1->Draw("SAME");
  
  // all->Add(hist1);
  //all->Add(hist2);
  //all->Add(hist3);
  //all->Add(hist4);
  //c0->cd();
  c0->SetLogy();
  /*
  all->Draw();
  TAxis *xaxis = all->GetXaxis();
  TAxis *yaxis = all->GetYaxis();
  xaxis->SetTitle("Invariant mass (GeV)");
  yaxis->SetTitle("Entries");
  yaxis->SetTitleOffset(1.2);
  all->SetMinimum(8.);
  all->SetMaximum(120000.);
  */
  TF1 *f1 = new TF1("m1","gaus",1.5,2);
  TF1 *f2 = new TF1("m2","gaus",0,5);
  TF1 *total = new TF1("mstotal","gaus(0)+gaus(3)",0,5);
  Double_t par[6]={50,1.5,0.1,5,2.5,2};
  hist0->Fit(f1,"R0");
  //hist2->Fit(f2,"R0+");
  //f1->GetParameters(&par[0]);
  //f2->GetParameters(&par[3]);
  total->SetParameters(par);
  hist0->Fit(total,"R0+");
  total->SetLineColor(2);
  total->SetLineWidth(2);
  //total->Draw("SAME");
  //f1->SetLineColor(3);
  //f1->Draw("SAME");

  auto legend = new TLegend(0.5,0.7,0.7,0.8);
  //legend->AddEntry(hist1,"3 muons with opposite signs","f");
  legend->AddEntry(hist0,"3 global muons with opposite signs","f");
  legend->Draw();
  

  c0->Print("invariant mass.png");
  cout<<count1<<" "<<count2<<" "<<count3<<" "<<count4<<" "<<endl;

  TCanvas *c1 = new TCanvas("1","muon pT",1200,900);
  xaxis = hist2->GetXaxis();
  yaxis = hist2->GetYaxis();

  xaxis->SetTitle("pT (GeV)");
  yaxis->SetTitle("Entries / 0.6 GeV");
  yaxis->SetTitleOffset(1.2);
  yaxis->SetRangeUser(0,100000);
  c1->cd();

  hist2->SetLineColor(2);
  hist5->SetLineColor(6);
  hist8->SetLineColor(4);
  //hist11->SetLineColor(2);
  //hist14->SetLineColor(6);
  //hist17->SetLineColor(4);
  //hist11->SetLineStyle(2);
  //hist14->SetLineStyle(2);
  //hist17->SetLineStyle(2);

  hist2->Draw();
  hist5->Draw("SAME");
  hist8->Draw("SAME");
  //hist11->Draw("SAME");
  //hist14->Draw("SAME");
  //hist17->Draw("SAME");

  legend = new TLegend(0.35,0.75,0.6,0.85);
  legend->AddEntry(hist2,"3 global muons with opposite signs, lead","f");
  legend->AddEntry(hist5,"3 global muons with opposite signs, sublead","f");
  legend->AddEntry(hist8,"3 global muons with opposite signs, third","f");
  //legend->AddEntry(hist11,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist14,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist17,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c1->Print("pt.png");

  TCanvas *c2 = new TCanvas("2","muon eta",1200,900);
  xaxis = hist3->GetXaxis();
  yaxis = hist3->GetYaxis();

  xaxis->SetTitle("eta");
  yaxis->SetTitle("Entries");
  yaxis->SetTitleOffset(1.2);
  yaxis->SetRangeUser(0,15000);
  c2->cd();

  hist3->SetLineColor(2);
  hist6->SetLineColor(6);
  hist9->SetLineColor(4);
  //hist12->SetLineColor(2);
  //hist15->SetLineColor(6);
  //hist18->SetLineColor(4);
  //hist12->SetLineStyle(2);
  //hist15->SetLineStyle(2);
  //hist18->SetLineStyle(2);

  hist3->Draw();
  hist6->Draw("SAME");
  hist9->Draw("SAME");
  //hist12->Draw("SAME");
  //hist15->Draw("SAME");
  //hist18->Draw("SAME");

  legend = new TLegend(0.15,0.75,0.4,0.85);
  legend->AddEntry(hist3,"3 global muons with opposite signs, lead","f");
  legend->AddEntry(hist6,"3 global muons with opposite signs, sublead","f");
  legend->AddEntry(hist9,"3 global muons with opposite signs, third","f");
  //legend->AddEntry(hist12,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist15,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist18,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c2->Print("eta.png");

  TCanvas *c3 = new TCanvas("3","muon phi",1200,900);
  xaxis = hist4->GetXaxis();
  yaxis = hist4->GetYaxis();

  xaxis->SetTitle("phi");
  yaxis->SetTitle("Entries / 0.28 rad");
  yaxis->SetTitleOffset(1.2);
  yaxis->SetRangeUser(0,15000);
  c3->cd();

  hist4->SetLineColor(2);
  hist7->SetLineColor(6);
  hist10->SetLineColor(4);
  //hist13->SetLineColor(2);
  //hist16->SetLineColor(6);
  //hist19->SetLineColor(4);
  //hist13->SetLineStyle(2);
  //hist16->SetLineStyle(2);
  //hist19->SetLineStyle(2);

  hist4->Draw();
  hist7->Draw("SAME");
  hist10->Draw("SAME");
  //hist13->Draw("SAME");
  //hist16->Draw("SAME");
  //hist19->Draw("SAME");

  legend = new TLegend(0.15,0.75,0.4,0.85);
  legend->AddEntry(hist4,"3 global muons with opposite signs, lead","f");
  legend->AddEntry(hist7,"3 global muons with opposite signs, sublead","f");
  legend->AddEntry(hist10,"3 global muons with opposite signs, third","f");
  //legend->AddEntry(hist13,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist16,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c3->Print("phi.png");

    TCanvas *c6 = new TCanvas("6","CosTheta",1200,900);
  xaxis = hist20->GetXaxis();
  yaxis = hist20->GetYaxis();

  xaxis->SetTitle("Cos #theta");
  yaxis->SetTitle("Entries");
  yaxis->SetTitleOffset(1.2);
  //yaxis->SetRangeUser(0,1000);
  c6->cd();

  hist20->SetLineColor(4);
  hist21->SetLineColor(2);
  hist22->SetLineColor(6);
  hist20->SetFillStyle(2);
  hist21->SetFillStyle(2);
  hist22->SetFillStyle(2);

  hist20->Draw();
  hist21->Draw("SAME");
  hist22->Draw("SAME");
  //hist19->Draw("SAME");

  legend = new TLegend(0.15,0.75,0.4,0.85);
  legend->AddEntry(hist20,"2 global + 1 tracker muon, lead","f");
  legend->AddEntry(hist21,"2 global + 1 tracker muon, sublead","f");
  legend->AddEntry(hist22,"2 global + 1 tracker muon, third","f");
  //legend->AddEntry(hist13,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist16,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();
  c6->SetLogy();

  c6->Print("costheta.png");

  TCanvas *c7 = new TCanvas("7","deltaR123",1200,900);
  xaxis = hist23->GetXaxis();
  yaxis = hist23->GetYaxis();

  xaxis->SetTitle("deltaR");
  yaxis->SetTitle("Entries");
  yaxis->SetTitleOffset(1.2);
  //yaxis->SetRangeUser(0,1000);
  c7->cd();

  hist23->SetLineColor(4);
  hist23->SetFillStyle(2);
  hist23a->SetLineColor(2);
  hist23a->SetFillStyle(2);
  hist23b->SetLineColor(8);
  hist23b->SetFillStyle(2);

  hist23->Draw();
  hist23a->Draw("same");
  hist23b->Draw("same");

  legend = new TLegend(0.4,0.75,0.65,0.85);
  legend->AddEntry(hist23,"GEN muon, lead","f");
  legend->AddEntry(hist23a,"GEN muon, sublead","f");
  legend->AddEntry(hist23b,"GEN muon, third","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c7->Print("deltaR123.png");

  TCanvas *c8 = new TCanvas("8","taupt",1200,900);
  xaxis = hist24->GetXaxis();
  yaxis = hist24->GetYaxis();

  xaxis->SetTitle("pT (GeV)");
  yaxis->SetTitle("Entries / 0.5 GeV");
  yaxis->SetTitleOffset(1.2);
  //yaxis->SetRangeUser(0,1000);
  c8->cd();

  hist24->SetLineColor(4);
  hist24->SetFillStyle(2);

  hist24->Draw();

  legend = new TLegend(0.35,0.82,0.6,0.85);
  legend->AddEntry(hist24,"GEN #tau","f");
  //legend->AddEntry(hist13,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist16,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c8->Print("taupT.png");

  TCanvas *c9 = new TCanvas("9","tauid",1200,900);
  xaxis = hist26->GetXaxis();
  yaxis = hist26->GetYaxis();

  xaxis->SetTitle("pdgId");
  yaxis->SetTitle("Entries");
  yaxis->SetTitleOffset(1.6);
  //yaxis->SetRangeUser(0,1000);
  c9->cd();

  hist26->SetLineColor(4);
  hist26->SetFillStyle(2);

  hist26->Draw();

  legend = new TLegend(0.35,0.82,0.6,0.85);
  legend->AddEntry(hist26,"GEN #tau","f");
  //legend->AddEntry(hist13,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist16,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c9->Print("tauId.png");

  TCanvas *c10 = new TCanvas("10","taueta",1200,900);
  xaxis = hist25->GetXaxis();
  yaxis = hist25->GetYaxis();

  xaxis->SetTitle("#eta");
  yaxis->SetTitle("Entries / 0.12");
  yaxis->SetTitleOffset(1.5);
  yaxis->SetRangeUser(0,13000);
  c10->cd();

  hist25->SetLineColor(4);
  hist25->SetFillStyle(2);

  hist25->Draw();

  legend = new TLegend(0.35,0.82,0.6,0.85);
  legend->AddEntry(hist25,"GEN #tau","f");
  //legend->AddEntry(hist13,"2 muons 1 track with same signs, lead","f");
  //legend->AddEntry(hist16,"2 muons 1 track with same signs, sublead","f");
  //legend->AddEntry(hist19,"2 muons 1 track with same signs, track","f");
  legend->Draw();

  c10->Print("taueta.png");

  TCanvas *c11 = new TCanvas("11","cos vs pt",1600,900);
  xaxis = hista->GetXaxis();
  yaxis = hista->GetYaxis();

  xaxis->SetTitle("deltaRSum");
  yaxis->SetTitle("pT (GeV)");
  //yaxis->SetRangeUser(0,1000);
  c11->cd();

  hista->SetLineColor(4);
  hista->SetFillStyle(2);
  hista->SetContour(100);

  hista->Draw("colz");

  c11->Print("deltaRSumvspt.png");

  TCanvas *c15 = new TCanvas("15","Dalitz",1200,900);
  xaxis = histb->GetXaxis();
  yaxis = histb->GetYaxis();

  xaxis->SetTitle(" (GeV)");
  yaxis->SetTitle("invmass sublead and third (GeV)");
  //yaxis->SetRangeUser(0,1000);
  c15->cd();

  histb->SetLineColor(4);
  histb->SetFillStyle(2);
  histb->SetContour(100);
  gStyle->SetPalette(kRainBow);

  histb->Draw("colz");

  c15->Print("Dalitz.png");

    TCanvas *c12 = new TCanvas("12","deltaR1 vs pt",1600,900);
  xaxis = hista1->GetXaxis();
  yaxis = hista1->GetYaxis();

  xaxis->SetTitle("deltaR, lead");
  yaxis->SetTitle("pT (GeV)");
  //yaxis->SetRangeUser(0,1000);
  c12->cd();

  hista1->SetLineColor(4);
  hista1->SetFillStyle(2);
  hista1->SetContour(100);

  hista1->Draw("colz");

  c12->Print("deltaR1vspt.png");

    TCanvas *c13 = new TCanvas("13","deltaR2 vs pt",1600,900);
  xaxis = hista2->GetXaxis();
  yaxis = hista2->GetYaxis();

  xaxis->SetTitle("deltaR, sublead");
  yaxis->SetTitle("pT (GeV)");
  //yaxis->SetRangeUser(0,1000);
  c13->cd();

  hista2->SetLineColor(4);
  hista2->SetFillStyle(2);
  hista2->SetContour(100);

  hista2->Draw("colz");

  c13->Print("deltaR2vspt.png");

    TCanvas *c14 = new TCanvas("14","deltaR3 vs pt",1600,900);
  xaxis = hista3->GetXaxis();
  yaxis = hista3->GetYaxis();

  xaxis->SetTitle("deltaR, third");
  yaxis->SetTitle("pT (GeV)");
  //yaxis->SetRangeUser(0,1000);
  c14->cd();

  hista3->SetLineColor(4);
  hista3->SetFillStyle(2);
  hista3->SetContour(100);

  hista3->Draw("colz");

  c14->Print("deltaR3vspt.png");


  gBenchmark->Show("select3Mu");

}
