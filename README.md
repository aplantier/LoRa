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

### Les pins utilisées pour les capteurs : 

``` 

ATMega328p       Capteurs

                   Ky-026(flamme)
 A0          <----> entrée analogique ( Out [0;1024] )
 2(D2)       <----> entrée Digitale ( etat haut quand flamme détectée )
                 |     vecteur 1 
                 |_>   ! _ INTERUPTION SUR LE pASSAGE A L'ETAT HAUT _ ! 

                   Ky-015(Temperature & humidite)
 5(D5)          <----> entrée Digitale ( envoie une tramme de 5 byte donnat                 les mesures + checksum ) 

                    Ky-06(Buzzer)
A1          <----> Sortie Analogique 



```

### Les variables 
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

### les trammes

**Tx** envoie deux types de trammes, chaques trammes a une taille fixe de 10 bytes lorsque ces trammes seront envoyées via le protocole LoRa, chaque bytes de ces trammes seront séparés par un bytes contenant **";"** en guise de séparateur de données.

Attention, les donnée son envoyées Bytes après Bytes par le protocole LoRa mais un chiffre a plusieurs unités n'est pas encodé sur un seul byte. 

Ex: 123, devient 1, 2 , 3. 

De plus un nombre négatif est casté avec un décalage

Ex : -11 devient 245( 256 - 11  ) et donc 2, 4, 5 



####  Tramme de mesures 

Parlons en détail de **buffer** et **FRAMESIZE** : buffer contient la trame type envoyée via le protocole LoRa et contenant les données mesurées par Tx. 
Sa taille est fixée (**FRAMESIZE**) les mesures décimales ont une précision de E-2 fixe.




A savoir que le **LoRa** transmet les trames bytes après bytes ( non-signés ). Le choix de trame que j'ai choisis est le suivant 

![Trame](/src/tramme.png)

+ **IdTx** identifiant de l'appareil O quand non identifié 
+ **Etat** Etat de l'appareil (normal/urgence-> feu)
+ **erreur** Le capteur TempHum peut avoir des ratés cependant les valeures sont soouvent cohérente. J'ai décidé de les garder a chaque fois avec ce code d'erreur qui me permettra de faire un post traitement sur ces données 
+ **Humidité** sur 2 bytes le premier pour la partie entière la seconde pour la partie decimale 
+ **Température** sur 2 bytes le premier pour la partie entière la seconde pour la partie decimale 
+ **Rayonnement infrarouge** en %, plus le seuil est haut plus le rayonnement est fort 
+ **Checksum**  somme des valeurs de la tramme dans un byte non signé 
+ **byte de fin de ligne** 



####  Tramme d'authentification

Lorsque l'appareil n'est pas identifié, envoie une trame d'authentification.

+ **IdTx** identifiant de l'appareil O car non identifié. C'est ce qui lancera la procédure d'authentification auprès de Rx 
+ **Key/randNumber** byte aléatoire générée avec random(125)
+ **Checksum**  somme des valeurs de la tramme dans un byte non signé 
+ **byte de fin de ligne** 

Le reste des byte est à 0 

### Les méthodes 

####  void flame_detected() 
Méthode appelée lors d'une interuption sur la pin _D2_ . 
L'etat d'alerte est déclaré ```bool b_Alerte = true``` 

####  void buildFrame(bool auth = false) 
Méthode construction des trammes à envoyer. Ces tramme sont construite dans le buffer. 

l'argument permet de choisir quelle tramme construire. 

Cette fonction appelle les methodes permettant d'efectuer les mesures. (cf _humTmp()_ dans humTmp.h)


####  void enregistrementRx() 
Cette methode permet de s'authentifier auprès de Rx. L'algo est le suivant: 

```
PRE CONDITIONS : 
Pas déja identifié -> idTx=0 
LoRa en mode emission
-----
DEBUT

key <-= une clé aléatoire
trame <-= format Trame authentification utilisant key
envoie (trame)

passe en mode reception 

tentative <-= 10 
timeout <-= 250 

boucle : tant que le nombre de t'entative >0 AND timeaout !=0 
        SI Frame Recue ET key ==trame recue [1]
            idTx <-= trame recue [2]
            FIN
        SINON tentative -- 
    timeout--
AUthentification écouée 
FIN
```

### void loop()
La methode loop boucle sur l'arduino. 

en pseudo code 

```
boucle 
    Si Etat d'alerte 
        Faire soner l'alarme 
        faire tramme d'alerte
        Envoyer Tramme d'alerte
        FIN
    Si appareil pas encore enregistré
        enregistrement()
        FIN
    Mesures des capteurs
    Construction de tramme
    envoi de la trame 
    attendre ( laps de temps)
    FIN
```



## Le module recepteur : Receiver/ Rx 

## Le fonctions communes à Tx/Rx: envoie & reception de trames 

## Le Software en python 

## liens Utiles 


TOOOOODOOOOOOOOOOOOOOOOO
=========================

+ Finir redaction 
+ check icrem id register in reeiver 
+ Faire Code python pour récupérer data  + qualité transmition 
+ voir pour du sql pour la bdd ? 
+ plot , moyenne glissante, envoie mail 
+ ajouter la posibilité de détecter une alerte incendie même pour un noeud non identifié 
+ compte du nombre de framme recu par chaques Tx + moyenne reception 