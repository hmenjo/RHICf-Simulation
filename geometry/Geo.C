{
  gSystem->Load("libGeom");
  gSystem->Load("libGdml");
  
  //TGeoManager::Import("./full_ts.gdml");
  TGeoManager::Import("./full_ts_woZDC_adhoc_pipe.gdml");
  TList* mat = gGeoManager->GetListOfMaterials();
  TIter next(mat);
  TObject *obj;
  while(obj = next()) obj->Print();
  
  gGeoManager->CheckOverlaps(0.01);
  gGeoManager->PrintOverlaps();
  
 
  TObjArray *va = gGeoManager->GetListOfVolumes();
  int nv = va->GetEntries();
  for(int i = 0; i < nv; i++){
    TGeoVolume *v = (TGeoVolume*)va->At(i);
    //cout << "Volume" << i
    //<< " Name: " << v->GetName()
    //<< " Mat: " << v->GetMaterial()->GetName()
    //<< endl;
    char *m = v->GetMaterial()->GetName();
    char *air = "Air";
    
    //cout << i << " " << strncmp("Iron", m, 4) << " " << m << endl;    
  
    if(strncmp("Duralumin", m, 9)==0){ 
      v->SetFillStyle(1001);
      v->SetFillColor(kGray+50);
      v->SetLineColor(kGray+50);
      
    }
    /*
    else if(strncmp("Air", m, 3)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kOrange+7);
      v->SetLineColor(kOrange+7);
      
    }
    */
    else if(strncmp("Iron", m, 4)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kRed);
      v->SetLineColor(kRed);
    }
    
    else if(strncmp("Scintillator", m, 12)==0) {
      cout << i << " " << strncmp("Scintillator", m, 12) << " " << m << endl;
      v->SetFillStyle(1001);
      v->SetFillColor(kCyan);
      v->SetLineColor(kCyan);
    }
    
    else if(strncmp("PMMA", m, 4)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kBlack);
      v->SetLineColor(kBlack);
    }
    else if(strncmp("FPMMA", m, 5)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kBlack);
      v->SetLineColor(kBlack);
    }


    else if(strncmp("Alumiunum", m, 9)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kGray);
      v->SetLineColor(kGray);
    }
    else if(strncmp("G10", m, 3)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kMagenta);
      v->SetLineColor(kMagenta);
    }
    else if(strncmp("Acrylic", m, 7)==0)  {
      v->SetFillStyle(1001);
      v->SetFillColor(kYellow);
      v->SetLineColor(kYellow);
    }
    else if(strncmp("Tungsten", m, 8)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kBlack);
      v->SetLineColor(kBlack);
    }
    else if(strncmp("GSO", m, 3)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kBlue);
      v->SetLineColor(kBlue);
    }
    else if(strncmp("GSObar", m, 6)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kBlue);
      v->SetLineColor(kBlue);
    }
    else if(strncmp("Quartz", m, 6)==0) {
      v->SetFillStyle(1001);
      v->SetFillColor(kOrange);
      v->SetLineColor(kOrange);
    }
    else if(strncmp("Galactic", m, 8)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kMagenta);
      v->SetLineColor(kMagenta);
    }
    
    else if(strncmp("Galactic2", m, 8)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kMagenta);
      v->SetLineColor(kMagenta);
    }

    else if(strncmp("BeamGas", m, 7)==0){
      v->SetFillStyle(1001);
      v->SetFillColor(kGreen);
      v->SetLineColor(kGreen);
    }
    v->SetTransparency(0);
  }

  TCanvas *c = new TCanvas("c","c",500,500);
  gGeoManager->GetTopVolume()->Draw("ogl");
  //c->Update();
  //c->Modified();
  c->Print("./geometry.gif");
}
