//#include "TGeoBBox.h"
//#include "AOpticsManager.h"
//#include "TEllipse.h"
//#include "TCanvas.h"
//#include "ALens.h"
//#include "TGeoSphere.h"
//#include "AGlassCatalog.h"
//#include "ARayShooter.h"
//#include "AGeoAsphericDisc.h"

void WinkelverteilungAbstand()
{
const double cm = AOpticsManager::cm();
const double mm = AOpticsManager::mm();
const double nm = AOpticsManager::nm();


double_t n = 1.5;						//Brechungsindex

//Parameter erst Linse AL2520-A

double_t d1 = 25*mm;

double_t z1 = 0*mm;
double_t curv1 = 1/(15.54*mm);
double_t k1 = -1.35;
double_t coefficients[8]={0,-2.3618134E-2,1.1303079E-3,1.1113906E-4,2.3981714E-5,-3.035791E-6,-1.3660815E-7,1.8881587E-8};

double_t z2 = z1 + 7.6*mm;
double_t curv2 = 0;
	




	AOpticsManager* manager = new AOpticsManager("manager", "My manager");
	TGeoBBox* box = new TGeoBBox(3*cm, 3*cm, 6*cm);
	AOpticalComponent* world = new AOpticalComponent("world", box);
	
	manager->SetTopVolume(world);
	manager->SetNsegments(100);
	
	AGeoAsphericDisk* disk1 = new AGeoAsphericDisk("disk1", z1, curv2, z2, -curv1, 0.5*d1, 0*cm); 
	disk1->SetConicConstants(0,k1);
	disk1->SetPolynomials(0,0,8,coefficients);
    disk1->InspectShape();
	ALens* Lens1 = new ALens("Lens1", disk1);   
	Lens1->SetConstantRefractiveIndex(1.79);  
	world->AddNode(Lens1,1);

	/*AGeoAsphericDisk* disk2 = new AGeoAsphericDisk("disk2", z2_1, curv2_1, z2_2, curv2_2, 0.5*d2, 0*cm); 
	ALens* Lens2 = new ALens("Lens2", disk2);   
	Lens2->SetConstantRefractiveIndex(1.5);  
	world->AddNode(Lens2,1);*/


	TGeoTube* focus = new TGeoTube("foc", 0*cm, 23.0*mm, 0.01*cm);
	TGeoTranslation* foctrans = new TGeoTranslation("foctrans", 0, 0,3*cm);
	AFocalSurface* focal = new AFocalSurface("focal", focus);
	world->AddNode(focal,1,foctrans);

	TGeoTranslation* obstrans = new TGeoTranslation("obstrans", 0, 0,3.01*cm);
	AObscuration* obs = new AObscuration("Obs", focus);
	world->AddNode(obs,1,obstrans);
   
	world->Draw("ogl");


    double AbstandMin=12*mm;    
    double AbstandMax=17*mm;
    double Abstand=AbstandMin;
    double AbstandStep= 1*mm;
    int AbstandZahl = (AbstandMax-AbstandMin)/AbstandStep+1;
    
    double Winkel_max =10;
    double Winkel_out;
    
    int div = 500;
    int divn;    
    int Winkelverteilung[15][500]={{0},{0}};


for(int x=0; x < AbstandZahl; x++)

{  
   
    TGeoTranslation* phtrans = new TGeoTranslation("PhotonTrans", 0, 0,-Abstand);
	ARayArray* Rays = ARayShooter::RandomCone(546*nm,1.15,Abstand,1000000,0, phtrans);
	manager->DisableFresnelReflection(1);
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
        
        if(Winkel_out<Winkel_max) 
        { 
            divn = round(Winkel_out*div/Winkel_max);
            Winkelverteilung[x][divn] +=1;
        }
        
       
		
        //TPolyLine3D* polN = RayN -> MakePolyLine3D();
	    //polN -> SetLineColor(1);
        //if(Winkel_out>0.&&Winkel_out<0.87)
        
            //polN -> Draw("ogl");
       
	}   

    


   /*for(int n=0;n<=(Exited->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Exited)[n];
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(2);
		//polN -> Draw("ogl");
	}
   for(int n=0;n<=(Suspended->GetLast()); n++) 
	{   
		ARay* RayN = (ARay*)(*Suspended)[n];
		TPolyLine3D* polN = RayN -> MakePolyLine3D();
		polN -> SetLineColor(2);
   
		//polN -> Draw("ogl");
	}*/
    cout<<Suspended->GetLast()<<endl<<Exited->GetLast()<<endl<<Focused->GetLast()<<endl;
    Abstand=Abstand+AbstandStep;
    
}   

std::ofstream out("Winkelverteilung.txt");

for(int n=0; n<div; n++)
{
    out <<n*Winkel_max/div << "\t";

    for(int i=0; i<AbstandZahl; i++)
    {
        out << Winkelverteilung[i][n] << "\t\t";
    }
    
    out << endl;
}

out.close();
}
