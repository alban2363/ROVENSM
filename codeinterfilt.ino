  //Auteur : Malo Selosse 
  //Projet : pilote automatique 
  //Commentaire:Ce programme permet d'extraire les valeurs non filtrées du capteur magnétomètre et de les filtrer

  //Branchement capteur 
  //Noir dur GND 
  //Rouge sur 5V 
  //SDA sur SDA 
  //SCL sur SCL 

  #include <bmm150_defs.h>
  #include <DFRobot_BMM150.h>
  #include "DFRobot_BMM150.h"
  DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);
  #define BMM150_CS D3
  
  //Filtre passe-bas
  float capActuel_filt = 0 ; // Angle de cap filtré
  float capActuel , ecart, commande;          // variables du correcteur (nombres réels) 
  float capActuel_Old ;
  float alpha = 0.9  ; // Coefficient de filtrage (0.9 = fort lissage, 0.1 = faible lissage)
  
  unsigned int Te, Tch;                       // variables pour la gestion du temps (nombres réels)
  unsigned long DateEch, DateChgt;            // Dates (nombres réels)
  int dt =100 ;                                 // durée en millisecondes souhaitée entre deux cycles (mesure -> commande), période d'échantillonage
  
  //DFRobot_BMM150_SPI bmm150(/*cs = */BMM150_CS);
  
  void setup() 
  {
    Serial.begin(115200);//transmission des données 
    while(!Serial);
    while(bmm150.begin()){
      Serial.println("bmm150 init failed, Please try again!");
      delay(1000);
    } Serial.println("bmm150 init success!");
  
    bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);//Normal mode
  
    bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);//High-accuracy mode
  
    bmm150.setRate(BMM150_DATA_RATE_10HZ);//10HZ
  
    bmm150.setMeasurementXYZ();//Default config of X-, Y-, and Z-axis
  
  }
  
  void loop()
  {
    Te = millis() - DateEch;                    //temps d'échantillonnage
    
    if ( Te >= dt) {                            // toutes les (au moins) dt millisecondes exécuter les instructions suivantes   
      DateEch = millis();                       //date ech: temps actuel       
      
      capActuel = bmm150.getCompassDegree();    //donne le cap actuel de la maquette en degrès sur le nord magnétique, stock les données du capteurs dans la variable capActuel
  
      //Methode: filtre passe-bas
      // Application du filtre passe-bas
      capActuel_filt = alpha * capActuel_Old + (1 - alpha) * capActuel ;//formule de réccurence du filtre passe bas
      capActuel_Old = capActuel_filt ; //capActuel_Old stock la valeur caActuel_filt pour la boucle d'après 
   
    //Affichage sur le moniteur série
      Serial.print(capActuel); //affichage sur le port série du capActuel
      Serial.print(" ,");
      Serial.println(capActuel_filt); //affichage sur le port série du capActuel_filt
    }
  }
