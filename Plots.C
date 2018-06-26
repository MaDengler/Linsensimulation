//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"
//#include "AGeoAsphericDisc.h"

void Plots()
{
const double cm = AOpticsManager::cm();
const double mm = AOpticsManager::mm();
const double nm = AOpticsManager::nm();

double_t n = 1.5;

double_t f = 50*mm;					//Brennweite
double_t d = 50*mm;					//Durchmesser


double_t r = 2*f*(n-1.0);				//Krümmungsradius

Double_t z1 = 0*mm;
Double_t z2 = z1+2*(r-sqrt(r*r-0.25*d*d));

Double_t curv1 = 1/r;
Double_t curv2 = -1/r;

Double_t rmax = 0.5*d;
Double_t rmin = 0*mm;



	AOpticsManager* manager = new AOpticsManager("manager", "My first manager");
	TGeoBBox* box = new TGeoBBox(100*mm, 100*mm, 100*mm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	manager->SetNsegments(100);
	
	AGeoAsphericDisk* disk = new AGeoAsphericDisk("disk", z1, curv1, z2, curv2, rmax, rmin); 
	ALens* AsLens = new ALens("AsLens", disk);   
	AsLens->SetRefractiveIndex(AGlassCatalog::GetRefractiveIndex("N-BK7"));  
	world->AddNode(AsLens,1);


	TGeoTube* focus = new TGeoTube("foc", 0*mm, 50*mm, 1*nm);
	TGeoTranslation* foctrans = new TGeoTranslation("foctrans", 0, 0,90*mm);
	AFocalSurface* focal = new AFocalSurface("focal", focus);
	world->AddNode(focal,1,foctrans);

	TGeoTranslation* obstrans = new TGeoTranslation("obstrans", 0, 0,90*mm);
	AObscuration* obs = new AObscuration("Obs", focus);
	world->AddNode(obs,1,obstrans);
   
	world->Draw("ogl");

    
   	TGeoTranslation* phtrans = new TGeoTranslation("PhotonTrans", 0, 0,-50*mm);
	ARayArray* Rays = ARayShooter::Rectangle(660*nm, 0*mm,50*mm,1,51,0, phtrans,0);
	manager->TraceNonSequential(Rays);	
    
	TObjArray* Focused = Rays -> GetFocused();
	TObjArray* Exited = Rays -> GetExited();
	TObjArray* Suspended = Rays -> GetSuspended();

	std::cout << "Focused: \t" << (Focused -> GetLast())+1 << std::endl; 
	std::cout << "Exited: \t" << (Exited -> GetLast())+1  << std::endl;
	std::cout << "Suspended: \t" << (Suspended -> GetLast()) +1 << "\n" << std::endl;

	std::fstream datei("r_ueber_z-660nm.txt", std::ios::out);

	for(int n=0;n<(Focused->GetLast())+1; n++) 
	{   
		ARay* RayN = (ARay*)(*Focused)[n];
		Double_t Direction[3];
		RayN -> GetDirection(Direction);
		double_t Point[3];
		RayN -> GetLastPoint(Point);
		cout << Direction[1]<<"," << Direction[2]<<":" <<Point[1]<<","  << Point[2]<< "\t Schnittpunkt mit z-Achse:"<<(Point[1]*Direction[2])/Direction[1]<<endl;
		
		if(Direction[1])
			datei << (50*mm*n)/((Focused->GetLast()))<<"\t"<<Point[2]-((Point[1]*Direction[2])/Direction[1])<<endl;
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(2);
		polN -> Draw();
	}
	
	datei.close();	
	
    	for(int n=0;n<(Exited->GetLast())+1; n++) 
	{   
		ARay* RayN = (ARay*)(*Exited)[n];
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(3);
		polN -> Draw("ogl");
	}
  

}
