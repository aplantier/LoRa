/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 *  Recepteur Rx 
 *  
 *  @brief Recoit des trames de données 
 *  
 *  Les capteurs utilisés
 *  ---------------------
      
 *  + ky-015 Temp & hum 
 *      OutPin(pates a gauche de haut en bas ) : Analog, Ground, V+  
 *      InPin                                    5(D5) 
 *      
 *      Valeur de sortie: Trame de  5 bytes 
 *       
 *      
 *  Fonctionalités
 *  ----------------------
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


 #define led         13  // Led de l'arduino
// valeurs Par deff pour la config LoRa
#define txPower     20 // puissance de transmission incl. []
#define SprFac      12 // spreading Factor incl[7;12]
#define BandWidth   125E3
#define preamLengt      8   // preamble size
#define CANAL       0xA2

#define MAXREGISTRE 10
#define FRAMESIZE   256


// ** Configuration LoRa
int txPo=txPower;           // puissance d'emission 
int spFa=SprFac;                // Spreading Factor
long baWi=BandWidth;
int prLe = preamLengt;

// ** Variables Globales
byte buffer[FRAMESIZE]={0};  // tramme de buffing pour la reception 
int registre [MAXREGISTRE]={0};// les Tx enregistrés sur l'appareil 
static int logedIn = 0 ;      // nombre d'appareils Tx enregistrés

// ** Librairies
#include <SPI.h>
#include <LoRa.h>

// ** Entetes fonctions 
void enregistrementTx();

// ** Initiatilisation 
void setup (){

  Serial.begin (9600);  // Debut du serial


  Serial.println("Lora Receiver");
  if(!LoRa.begin(915E6)){
    Serial.println("/!\\Lora echec");
  }
  LoRa.setTxPower(txPo,1);
  LoRa.setSpreadingFactor(spFa);
  LoRa.setSignalBandwidth(baWi);
  LoRa.setPreambleLength(prLe);
  LoRa.setSyncWord(CANAL);
  LoRa.enableCrc();
  LoRa.receive();

  
}

// ** Boucle d'exec

void loop()
{ 

  if (getFramme(LoRa.parsePacket())) {// received a packet
     
     if(buffer[0]==0) enregistrementTx();

    
  }
}

/**
 * @brief Fonction de reception de tramme 
 * @details Fonction d'aquisition d'une frame
 * @return bool false si erreur dans la lecture de frame 
 */
bool getFramme(int a_i_framSize){
  if(a_i_framSize==0) return false;
  Serial.print("Paquet recu taille: ");
  Serial.println(a_i_framSize);

  int cursor=0;


  while(LoRa.available()){
    buffer[cursor++]=(char)LoRa.read();
  }
  return true;

  }


