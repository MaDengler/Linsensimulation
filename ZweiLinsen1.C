//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"
//#include "AGeoAsphericDisc.h"

void ZweiLinsen1()
{
const double cm = AOpticsManager::cm();
const double mm = AOpticsManager::mm();
const double nm = AOpticsManager::nm();


double_t n = 1.5;						//Brechungsindex 

//Parameter Fresnellinse

double_t f_Fresnel = 502.1*mm;					//Brennweite Fresenllinse
double_t d_Fresnel = 502.1*mm;					//Durchmesser Fresnellinse


double_t r_Fresnel = 2*f_Fresnel*(n-1.0);			//Krümmungsradius Fresnellinse

Double_t z1_Fresnel = 0*cm;
Double_t z2_Fresnel = z1_Fresnel+2*(r_Fresnel-sqrt(r_Fresnel*r_Fresnel-0.25*d_Fresnel*d_Fresnel));

Double_t curv1_Fresnel = 1/1E19;
Double_t curv2_Fresnel = -1/(0.7*f_Fresnel);

Double_t rmax_Fresnel = 0.5*d_Fresnel;
Double_t rmin_Fresnel = 0*cm;

//Parameter Linse

double_t d_Lens = 50*mm;					//Durchmesser Linse
double_t f_Lens = f_Fresnel*d_Lens/d_Fresnel;			//Brennweite Linse

double_t r_Lens = 2*f_Lens*(n-1.0);				//Krümmungsradius Linse

Double_t z1_Lens = (f_Fresnel+f_Lens);
Double_t z2_Lens = z1_Lens+2*(r_Lens-sqrt(r_Lens*r_Lens-0.25*d_Lens*d_Lens));

Double_t curv1_Lens = 1/r_Lens;
Double_t curv2_Lens = -1/r_Lens;

Double_t rmax_Lens = 0.5*d_Lens;
Double_t rmin_Lens = 0;


	AOpticsManager* manager = new AOpticsManager("manager", "My first manager");
	TGeoBBox* box = new TGeoBBox(100*cm, 100*cm, 100*cm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	manager->SetNsegments(100);
	
	AGeoAsphericDisk* disk_Fresnel = new AGeoAsphericDisk("disk_Fr", z1_Fresnel, curv1_Fresnel, z2_Fresnel, curv2_Fresnel, rmax_Fresnel, rmin_Fresnel); 
	ALens* Lens_Fresnel = new ALens("Lens_Fresnel", disk_Fresnel);   
	Lens_Fresnel->SetConstantRefractiveIndex(1.5);  
	world->AddNode(Lens_Fresnel,1);

	AGeoAsphericDisk* disk_Lens = new AGeoAsphericDisk("disk_Fr", z1_Lens, curv1_Lens, z2_Lens, curv2_Lens, rmax_Lens, rmin_Lens); 
	ALens* Lens = new ALens("Lens_Fresnel", disk_Lens);   
	Lens->SetConstantRefractiveIndex(1.5);  
	world->AddNode(Lens,1);


	TGeoTube* focus = new TGeoTube("foc", 0*cm, 300*mm, 0.01*cm);
	TGeoTranslation* foctrans = new TGeoTranslation("foctrans", 0, 0,70);
	AFocalSurface* focal = new AFocalSurface("focal", focus);
	world->AddNode(focal,1,foctrans);

	TGeoTranslation* obstrans = new TGeoTranslation("obstrans", 0, 0,70.01);
	AObscuration* obs = new AObscuration("Obs", focus);
	world->AddNode(obs,1,obstrans);
   
	world->Draw("ogl");

/*double_t z_start=-20;
ARayArray* Rays;
   for(int i=1;i<1;i++)
    {
        double_t rad = 75/3;
        double_t xy = rad/sqrt(2);


        ARay* Ray1 = new ARay(0,546*nm,0,rad,z_start,0,0,0,1);
        ARay* Ray2 = new ARay(0,546*nm,0,-rad,z_start,0,0,0,1);
   	    ARay* Ray3 = new ARay(0,546*nm,rad,0,z_start,0,0,0,1);
        ARay* Ray4 = new ARay(0,546*nm,-rad,0,z_start,0,0,0,1);
        ARay* Ray5 = new ARay(0,546*nm,xy,xy,z_start,0,0,0,1);
        ARay* Ray6 = new ARay(0,546*nm,-xy,xy,z_start,0,0,0,1);
        ARay* Ray7 = new ARay(0,546*nm,-xy,-xy,z_start,0,0,0,1);
        ARay* Ray8 = new ARay(0,546*nm,xy,-xy,z_start,0,0,0,1);
       
        Rays->Add(Ray1);
        Rays->Add(Ray2);
        Rays->Add(Ray3);
        Rays->Add(Ray4);
        Rays->Add(Ray5);
        Rays->Add(Ray6);
        Rays->Add(Ray7);
        Rays->Add(Ray8);
    }
*/

    TGeoTranslation* phtrans = new TGeoTranslation("PhotonTrans", 0, 0,-20);
	ARayArray* Rays = ARayShooter::Circle(546*nm, 200*mm,4,9,0, phtrans);
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
