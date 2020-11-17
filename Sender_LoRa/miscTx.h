/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 * Fonctions diverses pour Tx 
 */

enum typeTrame {
  donnees = 0,
  authentification = 1,
  handcheck = 2
};

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
 * @brief Fonction d'affichage des dernières mesures
 */
void afficheMesure(){

  Serial.print("[ Appareil : ");
  Serial.print(idTx);
  Serial.print(" ] Etat d'alerte : ");
  Serial.print(buffer[1]?"oui":"non");
  Serial.print("---------\nMesures (possibilité d'erreur :");
  Serial.print(buffer[2]?"oui":"non");
  Serial.print("\n\t|_> Humiditée : ");
  Serial.print(buffer[3],DEC);
  Serial.print(",");
  Serial.print(buffer[4],DEC);

  Serial.print("%\n\t|_> Temperature : ");
  Serial.print(buffer[5],DEC);
  Serial.print(",");
  Serial.print(buffer[6],DEC);

  Serial.print("°\n\t|_> Spectre infrarouge : ");
  Serial.print(buffer[7],DEC);
  Serial.println("%");
}

