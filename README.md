Antoine PLANTIER
LP SIGD IOTIA
Module Objets Conectés 
---

# Projet d'Objets Conectés : Réseau de capteurs Communication LoRa

## Introduction

### Résumé
Dans le cadre du Module d'objets conectés, nous avions a faire un projet metant en oeuvre le protocole de communication **LoRa**. J'ai choisis de mettre en place un réseau de communication assurant un relevé de mesures et leurs utilisations dans un programe python.

Le projet consiste en un noeud collecteur _(Rx/receviver)_ qui receptionne les données envoyées par différents noeuds emeteurs _(Tx/sender)_. Ces données sont traitées et stoquées par un programme en python. Celui ci offre différentes actions de traitement et d'interactions. 

### Materiel 

Pour ce projet nous travaillons avec des arduino intégrant des modules **LoRa**

[Cf Page Web du module](https://github.com/FabienFerrero/UCA_Board)
+ Les Modules Arduino utilisés 
https://www.arduino.cc/en/uploads/Main/Arduino-Pro-Mini-schematic.pdf
https://cdn.sparkfun.com/assets/c/6/2/2/1/ProMini8MHzv2.pdf

Nous utilison également des capteurs : 

+ ky-015 temperature & humidite

https://tkkrlab.nl/wiki/Arduino_KY-015_Temperature_and_humidity_sensor_module
https://arduinomodules.info/ky-015-temperature-humidity-sensor-module/

+ ky-026 detecteur de flamme 

Voltage : 3.3 - 5.5V
Spectre infrarouge détecté :    760 nm to 1100 nm

https://arduinomodules.info/ky-026-flame-sensor-module/
https://www.thegeekpub.com/wiki/sensor-wiki-ky-026-flame-ir-sensor/
https://www.arduinoforbeginners.com/ky-026-arduino-flame-ir-sensor/

### Langages

+ C
+ Python
+ SQL


## Fonctionnement général 



Le module Recepteur **Rx/receiver** est conecté par le port série à l'ordinateur sur lequel tourne le programme en python. 

Le module recepteur **Rx** est en écoute sur un canal du protocol LoRa. 
Lorsque un module emetteur **Tx** se connecte sur un canal, il s'authentifie auprès de **Rx**, dès lors, il peut lui envoyer les données qu'il collecte avec ses capteurs. 

Ces données sont traitées par le software pour pouvoir les exploiter: elles sont datées, structurée et stockées. Le software permet par exemple de voir un graphique de l'évolution de la temperature actualisé dynamiquement, ou encore de faire des calculs : moyenne glissante, moyenne par jours ... etc 

Il existe également un mode d'urgence. Si un des modules ky-026 détecte une flamme, une interuption est générée sur le module émetteur qui se met donc en état d'alerte. la fréquence d'envoi des tramme est accéléré pour évaluer le danger, une alarme se met à sonner sur **Tx** et **Rx** et un email est envoyé pour avertir du dangers. 


Voici le schéma général du projet.

![Projet Général](/src/schem_General.png)

## Configuration LoRa des modules 

```c++
LoRa.begin(915E6); // parametre réseau pour l'europe 
LoRa.setTxPower(txPo, 1);// puissance d'émission à 20 ; la pin de sortie de l'emeteur
LoRa.setSpreadingFactor(spFa); // le sf ici gardé a 12
LoRa.setSignalBandwidth(baWi);// bande passante du signal 125E3
LoRa.setPreambleLength(prLe); // la longueur du preambules
LoRa.setSyncWord(CANAL);// le canal de communication ici 0xA2
LoRa.enableCrc(); //activation du crc
```

Pins utilisés par le module LoRa

```
ATMega328p       LoRa RFM95W 
                   Module
 D8          <----> RST
 MISO  (D12) <----> MISO
 MOSI  (D11) <----> MOSI
 SCK   (D13) <----> CLK
 SS    (D10) <----> SEL (Chip Select)
 D3          <----> DIO0
 D7          <----> DIO1
 D6          <----> DIO2
 3.3V        <----> Vcc
 
```


## Les modules Emeteurs : Sender/ TX 

+ Les pins utilisées pour les capteurs : 

``` 

ATMega328p       Capteurs

                   Ky-026(flamme)
 A0          <----> entrée analogique ( Out [0;1024] )
 2(D2)       <----> entrée Digitale ( etat haut quand flamme détectée )

                   Ky-015(Temperature & humidite)
 5(D5)          <----> entrée Digitale ( envoie une tramme de 5 byte donnat                 les mesures + checksum ) 

                    Ky-06(Buzzer)
A1          <----> Sortie Analogique 
```

+ Les variables 
``` c++

// ** Variables globales 
static int i_periode = 5000; // Temps du cycle 
static int i_AlertePeriode = 500; // Temps du cycle en periode d'alerte
static bool b_Alerte = false; // Detection du feu : true 
byte humTmp[5]; // tableau de stockage des mesures Humidite & temperature
byte idTx; //identifiant de l'appareil s'il nest pas enregistré dans tx =0 
byte randNumber; // variable pour stocker une clée générrée aléatoirement avec random() 
#define FRAMESIZE 10 // taille de la framme 
byte buffer[FRAMESIZE] = { // buffer de communication pour les données
  0
};
```

Parlons en détail de **buffer** et **FRAMESIZE** : buffer contient la trame type envoyée via le protocole LoRa et contenant les données mesurées par Tx. 

A savoir que le **LoRa** transmet les trames bytes après bytes ( non-signés ). Le choix de trame que j'ai choisis est le suivant 

![Projet Général](/src/trame.png)

## Le module recepteur : Receiver/ Rx 

## Le Software en python 



##liens Utiles 

