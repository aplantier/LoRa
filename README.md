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

Voici le schéma général du projet.

![Projet Général](/src/schem_General.png)

Le fonctionnement général est le suivant. 

Le module Recepteur **Rx/receiver** est conecté par le port série à l'ordinateur sur lequel tourne le programme en python. 

Le module recepteur **Rx** est en écoute sur un canal du protocol LoRa. 
Lorsque un module emetteur **Tx** se connecte sur un canal, il s'authentifie auprès de **Rx**, dès lors, il peut lui envoyer les données qu'il collecte avec ses capteurs. 

Ces données sont traitées par le software pour pouvoir les exploiter: elles sont datées, structurée et stockées. Le software permet par exemple de voir un graphique de l'évolution de la temperature actualisé dynamiquement, ou encore de faire des calculs : moyenne glissante, moyenne par jours ... etc 

Il existe également un mode d'urgence. Si un des modules ky-026 détecte une flamme, une interuption est générée sur le module émetteur qui se met donc en état d'alerte. la fréquence d'envoi des tramme est accéléré pour évaluer le danger, une alarme se met à sonner sur **Tx** et **Rx** et un email est envoyé pour avertir du dangers. 



envoie une tramme de demande d'authentification.(Cf. tramme Plus bas ) 
A sa reception, **Rx** 

 

### Les modules Emeteurs : Sender/ TX 

### Le module recepteur : Receiver/ Rx 

### Le Software en python 



##liens Utiles 

