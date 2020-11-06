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
static int logedIn = 0; // nombre d'appareils Tx enregistrés

// ** Librairies
#include <SPI.h>

#include <LoRa.h>

#include "miscRx.h"

#include "communication.h"

struct Rx registre[MAXREGISTRE]; // les Tx enregistrés sur l'appareil 

// ** Entetes fonctions 
void enregistrementTx();

void update(byte * buffer);

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
      Serial.print("Noeud Colecteur\n[ Nombre Tx conectés:");
      Serial.println(logedIn, DEC);
      Serial.print("");
        timer =0;
  }

    

  int pcktSize=LoRa.parsePacket();
  if ( pcktSize ) { // received a packet

    getFramme(buffer, FRAMESIZE,pcktSize );
    if ( * buffer == 0) // trame recu d'un appareil non apairé 
    {
      enregistrementTx( *(buffer + 1)); // enregistrement de l'appareil avec la clé 
      return;
    }
    if ( ( * buffer > logedIn ) ||  ( * buffer < 0) )
    {
      Serial.println("Tentative de conection invalide");
     return; // si un appareil avec un id incohérent,

    }


  
    update(buffer); // LoRa.packetRssi(); 

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
  Serial.println("--- [ Tentative d'apairement détecté");
  byte answer[5] = {
    a_key,
    logedIn + 1,
    '\0'
  };
  sendFrame(answer, 5);
   LoRa.receive();
   // on se met en recepteur pour recevoir la réponse
  int timout = 10000;
  while (timout-- != 0) { // Tentatives de conections = au nombre de device non authentifié
    
    
    if (LoRa.parsePacket() ) {
     
        getFramme(buffer, FRAMESIZE, LoRa.available());
        Serial.print("--- [ Apairement : ");
      if (( * buffer == logedIn + 1) && (a_key == * (1 + buffer)))
      {

      logedIn++;
      registre[logedIn].actif = false;// l'appareil est enregistré mais ne sera activé que à la première reception
      registre[logedIn].reportCount = 0;
      Serial.print("=  Reussi avec :");
      Serial.println(logedIn);
      return;
      }
      else 
      Serial.println(" échoué");

    }
    delay(2000);
    Serial.println(" OUverture LoRa echoué");

    

  }
}