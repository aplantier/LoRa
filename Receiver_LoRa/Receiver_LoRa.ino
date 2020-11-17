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
 *  + La trame {alerte, err,hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm} 8 Byte 
 *  + le separateur dans la trame est ';'
 *  + La trame : alerte feu détecté : 1 , 0 sinon
 *               err,   erreur parmis les mesures 
 *               hum_10, dizaine humidite
 *               hum_dec, 
 *               tmp_10, 
 *               tmp_dec, 
 *               infra, taux de rayonement infra 
 *               chksm cheksum 
 *      
 *  Dépendances
 *  ----------------------
 *  + miscRx.h : fonctions spéciales à Rx + declaration structure Rx représentant un noued 
 *  + communication.h: Fonctions de manipulation de framme communes a Tx 
 */

 #define PRINT_VERBOSE_SERIAL false

 #define led 13 // Led de l'arduino
// valeurs Par deff pour la config LoRa
#define txPower 20 // puissance de transmission incl. []
#define SprFac 12 // spreading Factor incl[7;12]
#define BandWidth 125E3
#define preamLengt 8 // preamble size
#define CANAL 0xA2 // canal de communication, idem pour sender

#define MAXREGISTRE 10 // le nombre d'appareils max identifiables sur l'appareil 
#define FRAMESIZE 256

// ** Configuration LoRa
int txPo = txPower; // puissance d'emission 
int spFa = SprFac; // Spreading Factor
long baWi = BandWidth;
int prLe = preamLengt;

// ** Variables Globales
byte buffer[FRAMESIZE] = {
  0
}; // tramme de buffing contenant la data netoyée 

static char logedIn = 0; // nombre d'appareils Tx enregistrés

// ** Librairies
#include <SPI.h>

#include <LoRa.h>

#include "miscRx.h"

#include "communication.h"

struct Rx registre[MAXREGISTRE]; // les Tx enregistrés sur l'appareil 

// ** Entetes fonctions 
void enregistrementTx();

void update(byte * buffer, int );

// ** Initiatilisation 
void setup() {

  Serial.begin(9600); // Debut du serial

  Serial.println("Lora Receiver");
  if (!LoRa.begin(915E6)) {
    Serial.println("/!\\Lora echec");
  }


  LoRa.setTxPower(txPo, 1);
  LoRa.setSpreadingFactor(spFa);
  LoRa.setSignalBandwidth(baWi);
  LoRa.setPreambleLength(prLe);
  LoRa.setSyncWord(CANAL);
  LoRa.enableCrc();
  LoRa.receive();

}

// ** Boucle d'exec
volatile int timer =450;
void loop() {
  if(timer++==9500)
  {
    #if PRINT_VERBOSE_SERIAL

    Serial.print("Noeud Colecteur [ Tx conectés: ");
    Serial.print(logedIn, DEC);
    Serial.print(" / ");
    Serial.print(MAXREGISTRE, DEC);
    Serial.println(" ] ");
    #endif

    timer =0;
  }



  int pcktSize=LoRa.parsePacket();
  int frameSize=0;
  if ( pcktSize ) { // received a packet

    getFramme(buffer, &frameSize,pcktSize );// recupération de la tramme 

    if ( ( * buffer > logedIn ) ||  ( * buffer < 0) )  // id incohérent
    {

      Serial.println("Tentative de conection invalide");


     return; // si un appareil avec un id incohérent,

   }
    if ( * buffer == 0) // trame recu d'un appareil non apairé 
    {
      if ( * (buffer + 1) == 0 ) // demande de handCheck 
      {
        byte answer[3] = {
          0,  // idTx
          0,  // key
          '\0'
        };
        if(* (buffer + 2)<=logedIn)
        {

         answer[0] = * (buffer + 2);  // idTx
         answer[1] = * (buffer + 3);  // key
         answer[2] = '\0';
         
       }
       else{
         answer[0] = * (buffer + 2);  // idTx
         answer[1] = 0;  // key
         answer[2] = '\0';
       }
       sendFrame(answer, 5);
     }
      else // enregistrement d'un nouvel appareil  avec key
      {        
      enregistrementTx( *(buffer + 1)); // enregistrement de l'appareil avec la clé 
      return;
    }
  }
    update(buffer, frameSize); // LoRa.packetRssi(); 

  }
}

/**
 * @brief Enregistrement d'un nouvel Tx
 * @details Lorsque un apparteil essaie de s'authentifier, il envoie une clé. En reponse Rx envoie une tramme avec comme entete 
 * + La clée précédement recue 
 * + L'identifiant proposé 
 * Puis Rx se met en ecoute avec un nombre de tentative limité 
 * @param a_key clé d'authentification.
 */

 void enregistrementTx(byte a_key) {


  #if PRINT_VERBOSE_SERIAL
  Serial.println("[ Tentative d'apairement :");
  #endif


  if(!registre[logedIn+1].actif)
  {

    byte answer[5] = {
      a_key,
      ++logedIn,
      '\0'
    };
    registre[logedIn].actif=true;
    sendFrame(answer, 5);
  }
}

void update(byte * buffer, int a_frameSize){
  Serial.println("COUCOU");
  registre[logedIn].reportCount++;

  char outputFrame [256]{0};

  for(int i =0 ; i<a_frameSize; i++)
  {
    Serial.print((char)*(i+buffer),DEC);
    Serial.print(',');
  }

  Serial.println("\n\r");
}
