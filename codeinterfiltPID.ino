//Commentaire:Ce programme permet d'illustrer une loi de commande PI avec une consigne de cap 

//Branchement capteur
//Rouge sur 5V
//Noir sur GND
//SDA sur SDA
//SCL sur SCL

#include <bmm150_defs.h>
#include <DFRobot_BMM150.h>
#include "DFRobot_BMM150.h"
DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);
#define BMM150_CS D3

//Filtre passe-bas
float capActuel_filt = 0; //Initialisation de l'angle de cap filtré
// Coefficient de filtrage (0.9 = fort lissage, 0.1 = faible lissage)
float alpha = 0.9  ;
float capActuel , ecart, commande;    // variables du correcteur (nombres réels)
float capActuel_Old ; 

unsigned int Te, Tch;                 // variables pour la gestion du temps (nombres réels)
unsigned long DateEch, DateChgt;      // Dates (nombres réels)
int dt = 100;   // durée en millisecondes souhaitée entre deux cycles (mesure -> commande)

float capConsigne=90 ;                // variables pour générer la consigne (nombres réels)
float kp =1  ; // Gain proportionnel. Préciser la valeur. Unités = %/°
float Ti = 50300 ; // Temps intégral. Préciser la valeur. Unités = millisecondes
float ui = 0  ; // valeur initiale de l'action intégrale. Préciser la valeur. Unités = %

//DFRobot_BMM150_SPI bmm150(/*cs = */BMM150_CS);

void setup() 
{
  Serial.begin(115200);
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
  Te = millis() - DateEch;
  
  if ( Te >= dt) {  // toutes les (au moins) dt millisecondes exécuter les instructions suivantes   
    DateEch = millis();
    
    capActuel = bmm150.getCompassDegree();

    //Methode: filtre passe-bas
    // Application du filtre passe-bas
    capActuel_filt = alpha * capActuel_Old + (1 - alpha) * capActuel ;
    capActuel_Old = capActuel_filt ;

    // Implémentez ici la loi de commande PI
    ecart = fmod((capConsigne - capActuel_filt + 180.0), 360.0) - 180.0;    //calcul de l'ecart entre le cap actuel et le cap consigne 
      
    if (ecart < -180.0){
      ecart += 360.0;
    }
    
    ui = ui + (kp*dt*ecart)/Ti    ;                   //calcul de la commande intégrale
    commande = kp*ecart + ui ;                    // calcul de la commande    //calcul de la commande PI

    //Dispositif anti-windup
    if (commande > 100) {
      commande = 100;
      ui = ui - (kp*dt*ecart)/Ti    ;              //windup
    }
    if (commande < - 100) {
      commande = -100;
      ui = ui - (kp*dt*ecart)/Ti    ;
    }


    // Affichage des données
    Serial.print(capActuel);
    Serial.print(" ,");
    Serial.print(capActuel_filt);
    Serial.print(" ,");
    Serial.println(commande);
  }
}
