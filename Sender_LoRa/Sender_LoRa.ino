/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 *  Emeteur Tx 
 *  
 *  @brief Receuille des données et les envoie via le protocole LoRa
 *  
 *  Les capteurs utilisés
 *  ---------------------
 *  
 *  + ky-026 detecteur de flamme 
 *      OutPin(pates a gauche de haut en bas ) : Analog, Ground, V+  , Digital
 *      InPin                                    A0(A0)                2(D2)     
 *      
 *      Valeur de sortie                         Max 1024              Etat Haut quand flame detectée 
 *      
 *  + ky-015 Temp & hum 
 *      OutPin(pates a gauche de haut en bas ) : Analog, Ground, V+  
 *      InPin                                    5(D5) 
 *      
 *      Valeur de sortie: Trame de  5 bytes 
 *       
 *      
 *  Fonctionalités
 *  ----------------------
 *  + Intéruption sur le vecteur 0 (pin 2 (D2))
 * 
 *  + La trame {alerte, err, hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm} 8 Byte 
 *  + La trame : alerte feu détecté : 1 , 0 sinon
 *               err,   erreur parmis les mesures 
 *               hum_10, dizaine humidite
 *               hum_dec, 
 *               tmp_10, 
 *               tmp_dec, 
 *               infra, taux de rayonement infra 
 *               chksm cheksum 
 * 
 */
// ** Declaration des pins (a : analog, d: digital)
#define a_flame     A0  // ky-026 flame
#define d_flame     2

#define d_tempHum   5   // ky-015 Temperature humidité

#define a_buzzer    A1

#define led         13  // Led de l'arduino





// ** Variables globales 
static int  i_periode         = 5000;     // Temps du cycle 
static int  i_AlertePeriode   = 500;      // Temps du cycle en periode d'alerte
static bool b_Alerte            = false;    // Detection du feu : true 
byte humTmp[5];                      // tableau de stockage des mesures Hum/ tmp 

// ** Librairies
#include "misc.h"
#include "humTemp.h"
// ** Entetes fonctions 
void alarme();
void flame_detected();
// ** Initiatilisation 
void setup (){

  pinMode(a_flame, INPUT);                    // Init detecteur Flame input
  pinMode(d_flame, INPUT);
  pinMode(d_tempHum, OUTPUT);                  //Init detecteur TempHum

  pinMode(led,OUTPUT);      
  pinMode(a_buzzer,OUTPUT);

  attachInterrupt(0, flame_detected, RISING);// interuption sur le vecteur 0 (pin 2(d2) au passage a létat haut
  Serial.begin (9600);                       // Debut du serial
}

// ** Boucle d'exec

void loop()
{ 
  byte buffer[9]={0}; // Trame a *  + La trame {alerte, err, hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm} 8 Byte 


  if(b_Alerte)                // Etat d'alerte : feu détecté 
  {
    alarme();  // SIgnal Sonore
    b_Alerte = digitalRead(d_flame)==HIGH ? true:false; // desactivation de l'alarme si le feu n'est plus detecté
                                                        // TODO: remplacer par une interuption de l'alarme avec BP
  }
  

  buffer[0]=(char) b_Alerte ? 1:0;
  buffer[1]=(char) (humTemp() ? 0:1);// si pas d'erreurs err=0
  buffer[2]=(char)humTmp[0];
  buffer[3]=(char)humTmp[1];
  buffer[4]=(char)humTmp[2];
  buffer[5]=(char)humTmp[3];
  buffer[6]=(unsigned char)  256  - (256*analogRead(a_flame)/1024) ;
  for(int i=0 ; i<7; i++,buffer[7]+=buffer[i]);
  buffer[8]=(char)'\0';
  //Serial.println((char*)buffer);


  for(int cursor=0; cursor<9 ; cursor++){
    Serial.print((int)buffer[cursor]);
    Serial.print(',');
  }
Serial.println();

  //Serial.print(buffer);

  delay(b_Alerte?i_AlertePeriode:i_periode); // en fonction de l'etat d'alerte, la periode varie
}



/**
 * @brief Fonction appelée sur l'interuption
 * @details Passage a l'etat d'alerte haut 
 */
void flame_detected() {
  b_Alerte = true;
  Serial.println("Interuption : Flame detectee");
}