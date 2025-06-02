/*mis à jour le 16/05/25

  Auteur : Antoine MARC 
  Projet : Réalisation d'un pilote auto
  Brique : mise en rotation de la maquette
  But    : Mise en rotation d'un seul moteur via le code constructeur.

  onstruction du programme:
            Dans un premier temps, le code définit un état initial pour le moteur. 
            Puis on met en rotation le moteur pendant 2 secondes.
            Ensuite, on arrête la rotation du moteur pendant 2 secondes.
            Enfin, la boucle se répète, mais a chaque itération, le sens de rotation est inverser.
    
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
*/
//Channel A, assignation des broches
int directionPin1 = 12;//assign direction sur la broche de sortie 12.
int pwmPin1 = 3;// assign PWM (work duty) sur la broche de sortie 3.
int brakePin1 = 9;//assign brake sur la broche de sortie 9.

//boolean pour pouvoir changer de direction
bool directionState1;

void setup() {
  
//define pins
pinMode(directionPin1, OUTPUT);
pinMode(pwmPin1, OUTPUT);
pinMode(brakePin1, OUTPUT);

// initialisation des sens de rotation des 2 moteurs
if(directionState1 == false){
  digitalWrite(directionPin1, LOW);
}
else{
  digitalWrite(directionPin1, HIGH);
} 
}

void loop() {

directionState1 = !directionState1;//change le sens de rotation

//désactivation du frein moteur pour le moteur
digitalWrite(brakePin1, LOW);

/*set work duty
35 valeur minimum(estimer) pour la mise 
en rotation pour vaincre l'inertie du systeme */
analogWrite(pwmPin1, 35);

delay(2000);//durée de la rotation de 2s

//activation du frein moteur 
digitalWrite(brakePin1, HIGH);

//set work duty for the motor to 0 (off)
analogWrite(pwmPin1, 0);

delay(2000);//durée de 2s pendant laquelle le moteur est arrêté
}
//----------------fin du programme----------------------------------------
