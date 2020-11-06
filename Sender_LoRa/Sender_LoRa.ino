/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 *  Emeteur Tx 
 *  
 *  @brief Colecte des données grace à deux capteur : Temperature, humidite, rayonement infrarouge. Un surveillance particulière
 *  des flammes est implémentées avec un état d'urgence sur intéruption si une flamme est  détecté. Le module essaie régulièrement
 *  de s'authentifier auprès d'un recepteur. S'il est authentifié, il envoie ses données via le protocole LoRa. 
 *  
 *  
 *  
 *  
 *  
 *  
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
 *  + La trame {ID, Etat, err, hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm, \0} 10 Byte 
 *  + La trame : ID emeteur si non authentifié : 0 
 *               Etat Alerte : 1 , 0 sinon
 *               err,   erreur parmis les mesures : 1
 *               hum_10, dizaine humidite
 *               hum_dec, 
 *               tmp_10, 
 *               tmp_dec, 
 *               infra, taux de rayonement infra 
 *               chksm cheksum 
 *               \0 bit de fin de trame 
 *               
 *  Configuration LoRa
 *  ----------------------
 *  Canal : 0xA2
 *  
 */
// ** Declaration des pins (a : analog, d: digital)
#define a_flame A0 // ky-026 flame
#define d_flame 2

#define d_tempHum 5 // ky-015 Temperature humidité

#define a_buzzer A1

#define led 13 // Led de l'arduino

// ** valeurs Par deff pour la config LoRa
#define txPower 20 // puissance de transmission incl. []
#define SprFac 12 // spreading Factor incl[7;12]
#define BandWidth 125E3
#define preamLengt 8 // preamble size
#define CANAL 0xA2 //le canal de communication

#define FRAMESIZE 10 // taille de la framme 

// ** Variables globales 
static int i_periode = 5000; // Temps du cycle 
static int i_AlertePeriode = 500; // Temps du cycle en periode d'alerte
static bool b_Alerte = false; // Detection du feu : true 
byte humTmp[5]; // tableau de stockage des mesures Hum/ tmp 
byte idTx; //identifiant de l'appareil s'il nest pas enregistré dans tx =0 
byte randNumber; // clé pour l'identifiant 
byte buffer[FRAMESIZE] = { // buffer de communication pour les données
  0
};

// param LoRa
int txPo = txPower; // puissance d'emission 
int spFa = SprFac; // Spreading Factor
long baWi = BandWidth;
int prLe = preamLengt; // taille du preambule 

// ** Librairies
#include <SPI.h>

#include <LoRa.h>

#include "miscTx.h"

#include "humTemp.h"

#include "communication.h"


// ** Entetes fonctions 
void alarme();
void flame_detected();
void buildFrame(bool auth = false);
void sendFrame();
void enregistrementRx();

// ** Initiatilisation 
void setup() {

  pinMode(a_flame, INPUT); // Init detecteur Flame input
  pinMode(d_flame, INPUT);
  pinMode(d_tempHum, OUTPUT); //Init detecteur TempHum

  pinMode(led, OUTPUT); //led de la maquette 
  pinMode(a_buzzer, OUTPUT); // le buzzer 

  LoRa.setPins(10, 9, -1); // desactivation de d2 pour utiliser l'interuption 
  attachInterrupt(digitalPinToInterrupt(d_flame), flame_detected, RISING); // interuption sur le vecteur 0 (pin 2(d2) au passage a létat haut
  Serial.begin(9600); // Debut du serial

  //LoRa.onTxDone(onTxDone); // Je ne comprends pas comment set le call back ....
  Serial.println("Lora Sender");
  if (!LoRa.begin(915E6)) {
    Serial.println("/!\\Lora echec");
  }
  LoRa.setTxPower(txPo, 1);
  LoRa.setSpreadingFactor(spFa);
  LoRa.setSignalBandwidth(baWi);
  LoRa.setPreambleLength(prLe);
  LoRa.setSyncWord(CANAL);
  LoRa.enableCrc(); //activation du crc
  idTx = 0;

  // la trame a envoyer

}

// ** Boucle d'exec

void loop() {
  Serial.print("ID tx : ");
  Serial.println(idTx);
  if (idTx == 0) // pas encore authentifié 
  {
    enregistrementRx(); //
    return; // si pas enregistré, pas de suite 

  }
  if (b_Alerte) // Etat d'alerte : feu détecté 
  {
    alarme(); // SIgnal Sonore
    b_Alerte = digitalRead(d_flame) == HIGH ? true : false; // desactivation de l'alarme si le feu n'est plus detecté
    buildFrame();
    sendFrame(buffer, FRAMESIZE);
    return;
    // TODO: remplacer par une interuption de l'alarme avec BP
  }


  buildFrame();
  sendFrame(buffer, FRAMESIZE);
  afficheMesure();

  //Serial.print(buffer);

  delay(b_Alerte ? i_AlertePeriode : i_periode); // en fonction de l'etat d'alerte, la periode varie
}

/**
 * @brief Fonction appelée sur l'interuption
 * @details Passage a l'etat d'alerte haut 
 */
void flame_detected() {
  b_Alerte = true;
  Serial.println("Interuption : Flame detectee");
}

/**
 * @brief Construction de la tramme a envoyer
 * @details construit la tramme 
 * tramme donnée : {ID, Etat, err, hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm, \0}
 * tramme authentification : {ID, Etat, err, hum_10, hum_dec, tmp_10, tmp_dec, infra, chksm, \0}
 */
void buildFrame(bool auth = false) {
  if (auth) { // trame d'authentification
    Serial.println(" Construction tramme authentification");

    for (int buffCursor = 0; buffCursor < FRAMESIZE; buffCursor++) {
      switch (buffCursor) {
      case 0:
        buffer[buffCursor] = (byte) idTx;
        break;

      case 1:
        buffer[buffCursor] = (byte) randNumber;
        break;

      case FRAMESIZE - 1:
        buffer[buffCursor] = (byte)
        '\0';
        break;

      case FRAMESIZE - 2:
        buffer[buffCursor] = 0;
        for (int i = 0; i < FRAMESIZE - 2; i++)
          buffer[buffCursor] += (char) buffer[i];
        break;

      default:
        buffer[buffCursor] = (byte) - 11;
      }
    }
    Serial.println("Affichage de la trame d'authentification");
    printFrame(buffer, FRAMESIZE);

    return; // RTant que Rx n'est pas authentifié on sort. 
  }
  for (int buffCursor = 0; buffCursor < FRAMESIZE; buffCursor++) {
    switch (buffCursor) {
    case 0:
      buffer[buffCursor] = (char) idTx; // identifiant de Tx  
      break;

    case 1:
      buffer[buffCursor] = (char) b_Alerte ? 1 : 0; // 
      break;

    case 2:
      buffer[buffCursor] = (char)(humTemp() ? 0 : 1); // si pas d'erreurs err=0       
      break;

    case 3:
      buffer[buffCursor] = (char) humTmp[0]; //Humidité entier
      break;

    case 4:
      buffer[buffCursor] = (char) humTmp[1]; //Humidité decimal      
      break;

    case 5:
      buffer[buffCursor] = (char) humTmp[2]; // Temp entier    
      break;

    case 6:
      buffer[buffCursor] = (char) humTmp[3]; // Temp decimal     
      break;

    case 7:
      buffer[buffCursor] = (unsigned char) 256 - (256 * analogRead(a_flame) / 1024); // Valeur Infra
      break;

    case FRAMESIZE - 1:
      buffer[buffCursor] = (char)
      '\0';
      break;

    case FRAMESIZE - 2: { // Calcul du checksum 
      buffer[buffCursor] = 0;
      for (int i = 0; i < FRAMESIZE - 2; i++)
        buffer[buffCursor] += (char) buffer[i];
    }
    break;

    default:
      buffer[buffCursor] = (char) 0; // remplissage a zero de la frame pour les indexes non renseignés
    }
  }
}

/**
 * @brief Fonction d'enregistrement de Tx auprès de Rx 
 * @details Tx par defaut ne connait pas son Tx. Pour pouvoir lui envoyer des donnés, il doit être authentifié.
 *          pour cela il passe en emetteur et envoie sa clé d'auth puis se met en recepteur. Tant qu'une trame de validation
 *          de l'enregistrement n'arrive pas, il continue d'essayer. 
 */
void enregistrementRx() {

  byte receptFrame[50] = {
    0
  };
  randNumber = (char) random(125); // clé d'authentification
  buildFrame(true); // construction de la tramme d'authentification
  sendFrame(buffer, FRAMESIZE);
  LoRa.receive();

  int tentatives = 0; //
  int timeout = 250;
  while (tentatives < 10 && timeout != 0) {

    if (LoRa.available()) {
      Serial.print("Tentative apairrement :");
      Serial.print(tentatives + 1, DEC);
      Serial.print("/10");

      if (getFramme(receptFrame, 50, LoRa.parsePacket()) && ( * receptFrame == randNumber)) {
        idTx = * (1 + receptFrame);
        Serial.print("Appairement avec Rx réussi id :");
        Serial.println(idTx);
        return;
      }
      else 
        tentatives++;
    }

  }
  Serial.print("Impossible de joindre Rx");
  return;
}