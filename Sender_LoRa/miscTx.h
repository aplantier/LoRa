/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 * Fonctions diverses pour Tx 
 */



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



 static void onTxDone(){

  Serial.print("ok :[");
  for(int cursor=0; cursor<9 ; cursor++){
    Serial.print(buffer[cursor],DEC);
    Serial.print(cursor==8?'\ ':',');
  }
  Serial.println(']');
}
