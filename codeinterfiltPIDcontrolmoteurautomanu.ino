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

//Filtre passe-bas
float capActuel_filt = 0; //Initialisation de l'angle de cap filtré
// Coefficient de filtrage (0.9 = fort lissage, 0.1 = faible lissage)
float alpha = 0.9  ;
float capActuel , ecart, commande, commandeAuto;    // variables du correcteur (nombres réels)
float capActuel_Old ; 

unsigned int Te, Tch;                 // variables pour la gestion du temps (nombres réels)
unsigned long DateEch, DateChgt;      // Dates (nombres réels)
int dt = 100;   // durée en millisecondes souhaitée entre deux cycles (mesure -> commande)

float capConsigne=90 ;                // variables pour générer la consigne (nombres réels)
float capPrecedent=capConsigne ;            //utilisée pour le ramp on setpoint
float kp =1  ; // Gain proportionnel. Préciser la valeur. Unités = %/°
float Ti = 50300 ; // Temps intégral. Préciser la valeur. Unités = millisecondes
float ui = 0  ; // valeur initiale de l'action intégrale. Préciser la valeur. Unités = %

// Paramètres de la rampe(ramp on setpoint)
float pente = 5.0;  // Vitesse de changement de consigne en °/s

//Auto ou Manu?
bool modeAuto= false ;   //booléen pour le mode Auto ou mode Manu

float alpha_pourcent =100;
float alpha_bin = 255;

//DFRobot_BMM150_SPI bmm150(/*cs = */BMM150_CS);

void setup() {
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

  //define pins
  pinMode(directionPin1, OUTPUT);
  pinMode(pwmPin1, OUTPUT);
  pinMode(brakePin1, OUTPUT);

  pinMode(directionPin2, OUTPUT);
  pinMode(pwmPin2, OUTPUT);
  pinMode(brakePin2, OUTPUT);

  directionState2 = !directionState2;//change direction sois de sens de rotation
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

    if (modeAuto== true) {                              //Mode AUTO
      //consigne en rampe(ramp on setpoint)
      if ( capConsigne > capPrecedent ) {
        capPrecedent += pente/(DateEch*60*1000) ;
        if ( capConsigne <= capPrecedent ) {
          capPrecedent = capConsigne;
        }
      }
        
      if ( capConsigne <= capPrecedent ) {
        capPrecedent -= pente/(DateEch*60*1000);
        if ( capConsigne >= capPrecedent ) {
          capPrecedent = capConsigne;
        }
      }

      ecart = fmod((capPrecedent - capActuel_filt + 180.0), 360.0) - 180.0;    //calcul de l'ecart entre le cap actuel et le cap consigne variant avec le ramp on setpoint
      
      if (ecart < -180.0){
        ecart += 360.0;
      }
      
      ui = ui + (kp*dt*ecart)/Ti    ;                   //calcul de la commande intégrale
      commandeAuto = kp*ecart + ui ;                    // calcul de la commande    //calcul de la commande PI
      if (commande > 100) 
      {
        commandeAuto = 100;
        ui = ui - (kp*dt*ecart)/Ti    ;              //windup
      }
      if (commandeAuto < - 100) 
      {
        commandeAuto = -100;
        ui = ui - (kp*dt*ecart)/Ti    ;
      }

      commande = commandeAuto * 255 / 100 ; 
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
    
    int alpha = constrain(abs(commande), 0, alpha_bin);

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
    
    analogWrite(pwmPin1, alpha);
    analogWrite(pwmPin2, alpha);  

    // Affichage des données
    Serial.print(capActuel);
    Serial.print(" ,");
    Serial.print(capActuel_filt);
    Serial.print(" ,");
    Serial.println(alpha);


  }
}
