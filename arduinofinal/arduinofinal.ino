//Commentaire:

//Branchement capteur
//Rouge sur 5V
//Noir sur GND
//SDA sur SDA
//SCL sur SCL

#include <bmm150_defs.h>
#include <DFRobot_BMM150.h>
#include <math.h>

#include "DFRobot_BMM150.h"

// Définition des broches du Motor Shield Rev3
// channel A
int directionPin1 = 12;
int pwmPin1 = 3;
int brakePin1 = 9;

// channel B
int directionPin2 = 13;
int pwmPin2 = 11;
int brakePin2 = 8;

//boolean to switch direction
bool directionState1 = false ;
bool directionState2 = false ;

DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

//Moyenne mobile
const int N = 20;                  // Taille de la fenêtre
float angles[N];                  // Tableau pour stocker les dernières valeurs
int index = 0;                    // Position actuelle dans le tableau
float total = 0;                  // Somme des valeurs
float capActuel_filt = 0;

float capActuel , ecart,ecart1, commande;          // variables du correcteur (nombres réels)
float capConsigne=90 ;                      // variables pour générer la consigne (nombres réels)
float capPrecedent=capConsigne ;            //utilisée pour le ramp on setpoint
unsigned int Te, Tch;                       // variables pour la gestion du temps (nombres réels)
unsigned long DateEch, DateChgt;            // Dates (nombres réels)
int dt =100;                                 // durée en millisecondes souhaitée entre deux cycles (mesure -> commande)
 
float kp =0.3;         // Gain proportionnel. Préciser la valeur
float Ti =2000  ;       // Temps intégral. Préciser la valeur. Unités ? millisecondes
float ui = 0  ;         // valeur initiale de l'action intégrale. Préciser la valeur. Unités ?

float pente=10;         //pente de la consigne 10°/s
bool modeAuto= false ;   //booléen pour le mode Auto ou mode Manu

float alpha_pourcent =39.21;
float alpha_bin = 100;


#define BMM150_CS D3

//DFRobot_BMM150_SPI bmm150(/*cs = */BMM150_CS);

void setup() {
  Serial.begin(115200);
  while(!Serial);{
  while(bmm150.begin()){
    Serial.println("bmm150 init failed, Please try again!");
    delay(1000);
  } Serial.println("bmm150 init success!");

  bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);//Normal mode

  bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY) ;//High-accuracy mode

  bmm150.setRate(BMM150_DATA_RATE_10HZ) ;//10HZ

  bmm150.setMeasurementXYZ();//Default config of X-, Y-, and Z-axis
  
  }
  
//  for (int i = 0; i < N; i++) {
//    angles[i] = 0;
//  }

  //define pins
  pinMode(directionPin1, OUTPUT);
  pinMode(pwmPin1, OUTPUT);
  pinMode(brakePin1, OUTPUT);

  pinMode(directionPin2, OUTPUT);
  pinMode(pwmPin2, OUTPUT);
  pinMode(brakePin2, OUTPUT);
}

void loop(){

  if (Serial.available() >= 3 ) 
    {
    byte identifiant = Serial.read();
    byte lowByte = Serial.read();
    byte highByte = Serial.read();

    uint16_t valeur_int = (highByte << 8) | lowByte ;

    float valeur = valeur_int / 100.0 ; 

    if (identifiant == 1)
      {
        capConsigne = valeur ;
      }
    if (identifiant == 2)
      {
        modeAuto = true ;
      }
    if (identifiant == 3)
      {
        modeAuto = false ;
      }
    if (identifiant == 4)
      {
       kp =valeur;
      }
    if (identifiant == 5)
      {
       Ti= valeur*1000;
      }
    if (identifiant == 6)
      {
        alpha_pourcent = valeur;
        alpha_bin =alpha_pourcent*(255/100);
      }
  }

  Te = millis() - DateEch;                    //temps d'échantillonnage
  
  if ( Te >= dt) {                            // toutes les (au moins) dt millisecondes exécuter les instructions suivantes   
    DateEch = millis();                       //date ech: temps actuel       
    
    capActuel = bmm150.getCompassDegree();    //donne le cap actuel de la maquette en degrès sur le nord magnétique

    

//    total -= angles[index];              // Enlève l'ancienne valeur
//    angles[index] = capActuel;      // Remplace par la nouvelle
//    total += capActuel;             // Ajoute la nouvelle valeur
//
//    index = (index + 1) % N;             // Passe à la prochaine case
//
//    capActuel_filt = total / N;                 // Calcule la moyenne
    capActuel_filt = capActuel;
    
    //consigne en rampe
    if (modeAuto== true) {                              //Mode AUTO
      //consigne en rampe(ramp on setpoint)
      if ( capConsigne > capPrecedent ) {
        capPrecedent += pente/(DateEch*60*1000) ; // a quoi correspond 60 ? 
        if ( capConsigne <= capPrecedent ) {
          capPrecedent = capConsigne;
          //Serial.println(capConsigne);
          //Serial.print(",");
          //Serial.println(capPrecedent);
        }
      }
        
      if ( capConsigne <= capPrecedent ) {
        capPrecedent -= pente/(DateEch*60*1000);
        if ( capConsigne >= capPrecedent ) {
          capPrecedent = capConsigne;
          //Serial.print(capConsigne);
          //Serial.println( capPrecedent );
        }
      }

      // Implémentez ici la loi de commande PI
      //ecart = capActuel_filt - capPrecedent   ;   //supprimer cette ligne ? 

      ecart1 = fmod((capPrecedent - capActuel_filt + 180.0), 360.0) - 180.0;    //calcul de l'ecart entre le cap actuel et le cap consigne variant avec le ramp on setpoint
      
      if (ecart1 < -180.0){
        ecart1 += 360.0;
      }
      
      ui = ui + (kp*dt*ecart1)/Ti    ;                   //calcul de la commande intégrale
      commande = kp*ecart1 + ui ;                    // calcul de la commande    //calcul de la commande PI
      if (commande > alpha_bin) 
      {
        commande = alpha_bin;
        ui = ui - (kp*dt*ecart1)/Ti    ;              //windup
      }
      if (commande < - alpha_bin) 
      {
        commande = -alpha_bin;
        ui = ui - (kp*dt*ecart1)/Ti    ;
      }
      
      
     
    }
    if (modeAuto ==false){                            //Mode MANU
      float val ;                           
      //float commandeManu ;                   //commande du mode en manu
      val = analogRead(3);                   //calcul de la commande en MANU grace au potentiometre qui pourrait varier entre -255 et +255
      commande = map(val,0,1023,-255,255) ;

      // dispositif auto <--> manu sans à coup
      capConsigne = capActuel_filt;
      capPrecedent= capActuel_filt;
      ui = ( float ) commande;
    }

    // Ajustement du rapport cyclique du hacheur (0 => 0 ; 255 =>100%) gauche et droite selon l’erreur de cap
    
    int alpha = constrain(abs(commande), 0, alpha_bin); // On prend la valeur absolue car Analogwrite ne sait gérer que des nombres positifs
    //int alpha = abs(commande); // on peut enlever cette ligne 
    digitalWrite(brakePin1, LOW); // Désactiver le frein
    digitalWrite(brakePin2, LOW); // Désactiver le frein
    
    // Inversion des directions pour une rotation sur place
    if (commande > 0) {
      digitalWrite(directionPin1, HIGH); // Moteur 1 avant
      digitalWrite(directionPin2, HIGH);  // Moteur 2 arrière
    } else {
      digitalWrite(directionPin1, LOW);  // Moteur 1 arrière
      digitalWrite(directionPin2, LOW); // Moteur 2 avant
    }
    
    analogWrite(pwmPin1, alpha); //envoie un signal PWM dont le rapport cyclique --> alpha, sur la broche pwmPin1 (moteur 1 )
    analogWrite(pwmPin2, alpha); //envoie un signal PWM dont le rapport cyclique --> alpha, sur la broche pwmPin2 (deuxième moteur)

    // Affichage des données
    //Serial.print("Cap Actuel : ");
    //Serial.print(capActuel);
    //Serial.print("Cap Actuel : ");
    //Serial.print(",");
    Serial.print(capActuel_filt);
    Serial.print(",");
    Serial.print(capConsigne);
    Serial.print(",");
    Serial.print(kp);
    Serial.print(",");
    Serial.print(Ti/1000);
    Serial.print(",");
    Serial.println(alpha_pourcent);
    //Serial.println(alpha);
    //Serial.print(" | Cap Cible : "); 
    //Serial.print(","); Serial.println(ecart1);
    //Serial.print(","); Serial.print(ecart1);
    //Serial.print(","); Serial.println(commande);
    //Serial.println( commande );

  }
}
