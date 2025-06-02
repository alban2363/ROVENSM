//Ce programme sert à calculer la valeur d'angle de cap avec l'aide du magnétomètre

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

unsigned int Te, Tch;                       // variables pour la gestion du temps (nombres réels)
unsigned long DateEch, DateChgt;            // Dates (nombres réels)
int dt =100;                                 // durée en millisecondes souhaitée entre deux cycles (mesure -> commande)

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
  Te = millis() - DateEch ;

  if ( Te >= dt ){
    DateEch = millis() ; 
  
    float capActuel = bmm150.getCompassDegree();
    Serial.println(capActuel);
  
  }
}
