//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"
//#include "AGeoAsphericDisc.h"

void Asphaerisch()
{
const double cm = AOpticsManager::cm();
const double mm = AOpticsManager::mm();
const double nm = AOpticsManager::nm();

Double_t z1 = -15 * cm;
Double_t z2 = z1 + 40*cm;

Double_t curv1 = 1. / (400 * cm);
Double_t curv2 = 1. / (-400 * cm);

Double_t rmax = 100 * cm;
Double_t rmin = 0. * cm;

	TCanvas *c = new TCanvas("c", "c",0,0,600,600);

	AOpticsManager* manager = new AOpticsManager("manager", "My first manager");
	TGeoBBox* box = new TGeoBBox(200*cm, 200*cm, 200*cm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	manager->SetNsegments(100);
	
	AGeoAsphericDisk* disk = new AGeoAsphericDisk("disk", z1, curv1, z2, curv2, rmax, rmin); 
	ALens* AsLens = new ALens("AsLens", disk);   
	AsLens->SetRefractiveIndex(AGlassCatalog::GetRefractiveIndex("N-BK7"));  
	world->AddNode(AsLens,1);


	TGeoTube* focus = new TGeoTube("foc", 0*cm, 100*cm, 0.01*cm);
	TGeoTranslation* foctrans = new TGeoTranslation("foctrans", 0, 0,190);
	AFocalSurface* focal = new AFocalSurface("focal", focus);
	world->AddNode(focal,1,foctrans);

	TGeoTranslation* obstrans = new TGeoTranslation("obstrans", 0, 0,190.01);
	AObscuration* obs = new AObscuration("Obs", focus);
	world->AddNode(obs,1,obstrans);
   
	world->Draw("ogl");

    
   	TGeoTranslation* phtrans = new TGeoTranslation("PhotonTrans", 0, 0,-200);
	ARayArray* Rays = ARayShooter::RandomSphericalCone(440, 100,26,0, phtrans);
	manager->TraceNonSequential(Rays);	
    
	TObjArray* Focused = Rays -> GetFocused();
	TObjArray* Exited = Rays -> GetExited();
	TObjArray* Suspended = Rays -> GetSuspended();

	std::cout << "Focused: \t" << (Focused -> GetLast())+1 << std::endl; 
	std::cout << "Exited: \t" << (Exited -> GetLast())+1  << std::endl;
	std::cout << "Suspended: \t" << (Suspended -> GetLast()) +1 << "\n" << std::endl;

	for(int n=0;n<(Focused->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Focused)[n];
		Double_t Direction[3];
		RayN -> GetDirection(Direction);
		cout << Direction[0]<<"\t" << Direction[1]<<"\t" << Direction[2]<<"\t" << endl;
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(2);
		polN -> Draw();
	}
    	for(int n=0;n<(Exited->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Exited)[n];
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(3);
		polN -> Draw();
	}
  

}
