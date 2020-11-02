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
 *      Utilisationd de la librairie DHT https://github.com/adafruit/DHT-sensor-library
 *      
 *  Fonctionalités
 *  ----------------------
 *  + Intéruption sur le vecteur 0 (pin 2 (D2))
 * 
 * 
 */
// ** Declaration des pins (a : analog, d: digital)
#define a_flame     A0  // ky-026 flame
#define d_flame     2

#define d_tempHum   5   // ky-015 Temperature humidité
#define DHTTYPE     DHT11// Pour la classe DHT 

#define a_buzzer    A1

#define led         13  // Led de l'arduino



// ** Librairies
#include <DHT.h>


// ** Variables globales 
static int  i_periode         = 5000;     // Temps du cycle 
static int  i_AlertePeriode   = 500;      // Temps du cycle en periode d'alerte
static bool b_Alerte            = false;    // Detection du feu : true 

DHT dht(d_tempHum,DHTTYPE);                        // Accesseur pour le capteur tempHum

// ** Entetes fonctions 
void alarme();
void flame_detected();
// ** Initiatilisation 
void setup (){

  pinMode(a_flame, INPUT);                    // Init detecteur Flame input
  pinMode(d_flame, INPUT);
  pinMode(d_tempHum, INPUT);                  //Init detecteur TempHum

  pinMode(led,OUTPUT);      
  pinMode(a_buzzer,OUTPUT);

  attachInterrupt(0, flame_detected, RISING);// interuption sur le vecteur 0 (pin 2(d2) au passage a létat haut
  Serial.begin (9600);                       // Debut du serial
}

// ** Boucle d'exec

void loop()
{ 
  byte buffer[128]={0};
  float f_infrarouge=0.0;
  float f_temperature=0.0;
  float f_humidite=0.0;

  if(b_Alerte)                // Etat d'alerte : feu détecté 
  {
    alarme();  // SIgnal Sonore
    b_Alerte = digitalRead(d_flame)==HIGH ? true:false; // desactivation de l'alarme si le feu n'est plus detecté
                                                        // TODO: remplacer par une interuption de l'alarme avec BP
  }

  f_infrarouge = analogRead(a_flame);
  
  dht.read(d_tempHum);
  f_temperature = dht.readTemperature();
  f_humidite    = dht.readHumidity();



  Serial.print("Print");
  Serial.print();

  delay(b_Alerte?i_AlertePeriode:i_periode); // en fonction de l'etat d'alerte, la periode varie


}


/**
 * @brief Signal d'alerte audio
 * @details [long description]
 */
void alarme() {
  
  Serial.println("Alerte Incendie !");
  
  unsigned char i;
  for (i = 0; i < 50; i++)
  {
    digitalWrite (a_buzzer, HIGH) ;
    delay (1) ;
    digitalWrite (a_buzzer, LOW) ;
    delay (1) ;
  }
  //Frequency 2
  for (i = 0; i < 100; i++)
  {
    digitalWrite (a_buzzer, HIGH) ;
    delay (2) ;
    digitalWrite (a_buzzer, LOW) ;
    delay (2) ;
  }

  
}

/**
 * @brief Fonction appelée sur l'interuption
 * @details Passage a l'etat d'alerte haut 
 */
void flame_detected() {
  b_Alerte = true;
  Serial.println("Interuption : Flame detectee");
}