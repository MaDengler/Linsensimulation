//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"
//#include "AGeoAsphericDisc.h"

void Winkelverteilung()
{
const double cm = AOpticsManager::cm();
const double mm = AOpticsManager::mm();
const double nm = AOpticsManager::nm();


double_t n = 1.5;						//Brechungsindex

//Parameter erst Linse AL2520-A

double_t d1 = 25*mm;

double_t z1_1 = 0*mm;
double_t curv1_1 = 1/(15.54*mm);
double_t k1 = -1.35;
double_t coefficients[8]={0,2.3618134E-2,-1.1303079E-3,-1.1113906E-4,-2.3981714E-5,3.035791E-6,1.3660815E-7,-1.8881587E-8};

double_t z1_2 = z1_1 + 7.6*mm;
double_t curv1_2 = 0;


//Parameter zweite Linse LB1437-A

double_t d2 = 25.4*mm;

double_t z2_1 = 5*cm;
double_t curv2_1 = 1/(102.4*mm);

double_t z2_2 = z2_1 + 3.1*mm;
double_t curv2_2 = -1/(102.4*mm);
 


	AOpticsManager* manager = new AOpticsManager("manager", "My manager");
	TGeoBBox* box = new TGeoBBox(15*cm, 15*cm, 25*cm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	manager->SetNsegments(100);
	
	AGeoAsphericDisk* disk1 = new AGeoAsphericDisk("disk1", z1_1, curv1_1, z1_2, -curv1_2, 0.5*d1, 0*cm); 
	disk1->SetConicConstants(k1,0);
	disk1->SetPolynomials(8,coefficients,0,0);
    disk1->InspectShape();
	ALens* Lens1 = new ALens("Lens1", disk1);   
	Lens1->SetConstantRefractiveIndex(1.5);  
	world->AddNode(Lens1,1);

	AGeoAsphericDisk* disk2 = new AGeoAsphericDisk("disk2", z2_1, curv2_1, z2_2, curv2_2, 0.5*d2, 0*cm); 
	ALens* Lens2 = new ALens("Lens2", disk2);   
	Lens2->SetConstantRefractiveIndex(1.5);  
	world->AddNode(Lens2,1);


	TGeoTube* focus = new TGeoTube("foc", 0*cm, 15.0*mm, 0.01*cm);
	TGeoTranslation* foctrans = new TGeoTranslation("foctrans", 0, 0,5*cm);
	AFocalSurface* focal = new AFocalSurface("focal", focus);
	world->AddNode(focal,1,foctrans);

	TGeoTranslation* obstrans = new TGeoTranslation("obstrans", 0, 0,5.01*cm);
	AObscuration* obs = new AObscuration("Obs", focus);
	world->AddNode(obs,1,obstrans);
   
	world->Draw("ogl");

    double Winkel_in = 60;    
    double Winkel_out;
    double Winkelvert;
    double PhStart;    
    int Winkelverteilung[11][120]={{0},{0}};


for(int x=0; x <= 0; x++)

{  
     
    PhStart = -1.26/tan((3.14159*Winkel_in)/360);
    TGeoTranslation* phtrans = new TGeoTranslation("PhotonTrans", 0, 0,PhStart+5.4*mm);
	ARayArray* Rays = ARayShooter::RandomCone(546*nm,1.26,-PhStart,30,0, phtrans);
	manager->TraceNonSequential(Rays);	
    

	TObjArray* Focused = Rays -> GetFocused();
	TObjArray* Exited = Rays -> GetExited();
	TObjArray* Suspended = Rays -> GetSuspended();

//	std::cout << "Focused: \t" << (Focused -> GetLast())+1 << std::endl; 
//  std::cout << "Exited: \t" << (Exited -> GetLast())+1  << std::endl;
//	std::cout << "Suspended: \t" << (Suspended -> GetLast())+1 << "\n" << std::endl;
    
    
	
    for(int n=0; n<=(Focused->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Focused)[n];
		Double_t Direction[3];
        RayN -> GetDirection(Direction); 
        Winkel_out = (360/(2*3.14159))*acos(Direction[2]/(sqrt(pow(Direction[0],2)+pow(Direction[1],2)+pow(Direction[2],2))));

        for(int i=0;i<120;i++)
        {
            if((Winkel_out<i/20.0))
            {
                Winkelverteilung[x][i-1] += 1;
                break;
            }
        }

        cout << Direction[0]<<"\t" << Direction[1]<<"\t" << Direction[2]<<"\t"<<Winkel_out<<"\t" << endl;
		
        TPolyLine3D* polN = RayN -> MakePolyLine3D();
	    polN -> SetLineColor(2);
		polN -> Draw("ogl");
       
	}   

    Winkel_in += 1;


/*   	for(int n=0;n<=(Exited->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Exited)[n];
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(2);
		polN -> Draw("ogl");
	}
*/
}   

/*std::ofstream out("Winkelverteilung.txt");

for(int n=0; n<120; n++)
{
    out <<n/20.00 << "\t";

    for(int i=0; i<=10; i++)
    {
        out << Winkelverteilung[i][n] << "\t\t";
    }
    
    out << endl;
}

out.close();*/
}
