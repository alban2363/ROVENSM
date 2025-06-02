/*mis à jour le 16/05/25

    Auteur : Antoine MARC 
    Projet : Réalisation d'un pilote auto
    Brique : mise en rotation de la maquette
    But    : rotation des 2 moteur en simultannés
    
    Construction du programme:    
          -on reprend le programme rotation_1moteur
       mais on modifie le programme pour ajouter un deuxieme moteur 
       le programe mets en rotation la maquette pendant 2s et s'arrête 
       pendant 1s 
       cependant on arrête le changement de sens de rotation a chaque 
       itération  
    
  Matériel:
    carte Arduino UNO
    carte Arduino Motor Shield REV 3
    Moteur FIT0521 DF25-370
    Batterie PS-1212

  Branchement:
    -la batterie: borne (-) sur la connexion GND et borne (+) sur la connexion VIN
      de la carte Arduino Motor Shield REV 3
    -le moteur 1 :borne (-) sur la connexion A- et borne (+) sur la connexion A+
      de la carte Arduino Motor Shield REV 3
    -le moteur 2 :borne (-) sur la connexion B- et borne (+) sur la connexion B+
      de la carte Arduino Motor Shield REV 3
*/

//Channel A, assignation des broches pour le moteur 1
int directionPin1 = 12;//assign direction sur la broche de sortie 12.
int pwmPin1 = 3;// assign PWM (work duty) sur la broche de sortie 3.
int brakePin1 = 9;//assign brake sur la broche de sortie 9.

//channel B, assignation pour le deuxieme moteur
int directionPin2 = 13;
int pwmPin2 = 11;
int brakePin2 = 8;

//boolean pour pouvoir changer de direction
bool directionState1;
bool directionState2;

void setup() {
  
//configurations des broches moteur 1 et 2
pinMode(directionPin1, OUTPUT);
pinMode(pwmPin1, OUTPUT);
pinMode(brakePin1, OUTPUT);

pinMode(directionPin2, OUTPUT);
pinMode(pwmPin2, OUTPUT);
pinMode(brakePin2, OUTPUT);

// initialisation des sens de rotation des 2 moteurs
if(directionState1 == false){
  digitalWrite(directionPin1, LOW);
}
else{
  digitalWrite(directionPin1, HIGH);
} 
if(directionState2 == false){
  digitalWrite(directionPin2, LOW);
}
else{
  digitalWrite(directionPin2, HIGH);
}

}

void loop() {

//désactivation des freins moteur pour les moteurs 1 et 2
digitalWrite(brakePin1, LOW);//
digitalWrite(brakePin2, LOW);

/*set work duty
35 valeur minimum(estimer) pour la mise 
en rotation pour vaincre l'inertie du systeme */
analogWrite(pwmPin1, 35);                        
analogWrite(pwmPin2, 35);

delay(2000); //durée de la rotation de 2s

//activation des freins moteurs pour les moteurs 1 et 2
digitalWrite(brakePin1, HIGH);
digitalWrite(brakePin2, HIGH);

//set work duty for the motor to 0 (off) motor 1
analogWrite(pwmPin1, 0);
analogWrite(pwmPin2, 0);

delay(1000); // durée de 1s pendant laquelle les moteurs sont arrêtés 
}
//------------------fin du programme----------------------------------------
