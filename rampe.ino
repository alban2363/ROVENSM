//Ce programme effectue la ramp on setpoint

float capActuel , ecart,ecart1, commande;          // variables du correcteur (nombres réels)
int capConsigne=90 ;                      // variables pour générer la consigne (nombres réels)
int capPrecedent=0;            //utilisée pour le ramp on setpoint
unsigned int Te, Tch;                       // variables pour la gestion du temps (nombres réels)
unsigned long DateEch, DateChgt;            // Dates (nombres réels)
int dt =100;                                 // durée en millisecondes souhaitée entre deux cycles (mesure -> commande)
float pente=10;         //pente de la consigne 10°/s
bool modeAuto= true ;   //booléen pour le mode Auto ou mode Manu
void setup() {

  Serial.begin(115200);

}


void loop(){

  Te = millis() - DateEch;                    //temps d'échantillonnage
  
  if ( Te >= dt) {                            // toutes les (au moins) dt millisecondes exécuter les instructions suivantes   
    DateEch = millis();                       //date ech: temps actuel       
    
    //consigne en rampe
    if (modeAuto== true) {                              //Mode AUTO
      
      if ( capConsigne > capPrecedent ) {           //sens de la giration
        capPrecedent += pente*dt/1000 ;           //la consigne de cap évolue
        if ( capConsigne <= capPrecedent ) {     //une fois la valeur atteinte la consigne reste constante
          capPrecedent = capConsigne;              
        
          
        }
      }
        
      if ( capConsigne <= capPrecedent ) {      //sens de la giration
        capPrecedent -= pente*dt/1000;          //la consigne de cap évolue
        if ( capConsigne >= capPrecedent ) {      //une fois la valeur atteinte la consigne reste constante
          capPrecedent = capConsigne;
          //Serial.print(capConsigne);
          //Serial.println( capPrecedent );
        }
      }
    }
    Serial.println(capConsigne);Serial.print(",");    //on regarde l'échelon de consigne
    
    Serial.println(capPrecedent);                 //on regarde la rampe
  
  }
}
