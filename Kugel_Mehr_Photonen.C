//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"


void Kugel_Mehr_Photonen()
{
	const double cm = AOpticsManager::cm();
	const double nm = AOpticsManager::nm();

	TCanvas *c = new TCanvas("c", "c",0,0,600,600);

	AOpticsManager* manager = new AOpticsManager("manager", "My first manager");
	TGeoBBox* box = new TGeoBBox(2000*cm, 2000*cm, 2000*cm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	
	

	TGeoSphere *sphe = new TGeoSphere("SPHE1",0,120*cm, 0,180, 0,180);
	manager->SetNsegments(200);

	ALens* sphere = new ALens("SpLens", sphe);
	sphere->SetRefractiveIndex(AGlassCatalog::GetRefractiveIndex("N-BK7"));
	
    TGeoTranslation* trans = new TGeoTranslation("Translation",20,0,0);
    TGeoRotation* rot = new TGeoRotation("Rotation",0,90,0);

    world->AddNode(sphere,1,new TGeoCombiTrans(50,0,0,rot));
//	sphere->Draw("ogl");

	world->Draw("ogl");
	TView *view = gPad->GetView();
	view->ShowAxis();
	 
    ARayArray* Rays = ARayShooter::RandomCircle(400, 15*cm, 50, 0, new TGeoTranslation("PhotonTrans", 60,0,-70), 0);
    manager->TraceNonSequential(Rays);	
    
    TObjArray* Focused = Rays -> GetFocused();
    TObjArray* Exited = Rays -> GetExited();
    TObjArray* Suspended = Rays -> GetSuspended();

    std::cout << "Focused: \t" << (Focused -> GetLast()) +1 << std::endl; 
    std::cout << "Exited: \t" << (Exited -> GetLast()) +1 << std::endl;
    std::cout << "Suspended: \t" << (Suspended -> GetLast()) +1 << std::endl;

    for(int n=0;n<(Exited -> GetLast()) +1; n++) 
    {   
        ARay* RayN = (ARay*)(*Exited)[n];
        TPolyLine3D* polN = RayN -> MakePolyLine3D();
        polN -> SetLineColor(2);
        polN -> Draw();
    }



//  ARay* red = new ARay(0, 650*nm, -5*cm, 3*cm, 0*cm, 0, 1, 0, 0);
//  ARay* green = new ARay(0, 500*nm, 20*cm, 4*cm, 0*cm, 0, 1, 0, 0);
//	ARay* blue = new ARay(0, 400*nm, 20*cm, 2*cm, 0*cm, 0, 1, 0, 0);
	
//	manager->TraceNonSequential(red);
//	manager->TraceNonSequential(green);
//	manager->TraceNonSequential(blue);
	
//	TPolyLine3D* polR = red->MakePolyLine3D();
//	TPolyLine3D* polG = green->MakePolyLine3D();
//	TPolyLine3D* polB = blue->MakePolyLine3D();
	
//	polB->SetLineColor(4);
//	polB->SetLineWidth(3);
//	polB->Draw();

//	polG->SetLineColor(3);
//	polG->SetLineWidth(3);
//	polG->Draw();

//	polR->SetLineColor(2);
//	polR->SetLineWidth(3);
//	polR->Draw();

//  sphe->SetFillStyle(1001);
//	sphe->SetLineColor(4);
//	sphe->SetLineWidth(6);	
	
}
