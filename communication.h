/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 * Fonctions comunes Rx Tx pour la reception et l'envoie de tramme
 * 
 */

// ** SIgnatures 

byte byteTabToByte(byte * a_byteTabToParse, int dataSize);
static void printFrame(byte * a_frame, int a_frameSize);
void sendFrame(byte * a_frame, int a_frameSize);
bool getFramme(byte * a_outputFrame, int a_i_outputFrameSize, int a_i_framSize);

static void printFrame(byte * a_frame, int a_frameSize) {

  Serial.print("[");
  for (int cursor = 0; cursor < a_frameSize; cursor++) {
    Serial.print((char) * (a_frame + cursor), DEC);
    Serial.print(cursor == a_frameSize - 1 ? '\ ' : ',');
  }
  Serial.println(']');
}

void sendFrame(byte * a_frame, int a_frameSize) {
  Serial.print("--- [ Envoi trame: ");
  printFrame(a_frame, a_frameSize);
  LoRa.beginPacket();
  for (int cursor = 0; cursor < a_frameSize; cursor++) {
    if ((char) * (a_frame + cursor) < 0) { // si la data est négative, on evoie le '-' a part 
      LoRa.print('-');
      *(a_frame + cursor) *= -1; // on prends la partie entière
    }
    LoRa.print( * (a_frame + cursor)); // modif 
    LoRa.print(';');
  }
  LoRa.endPacket();
  LoRa.sleep();
  return;

}

/**
 * @brief Fonction de reception de tramme 
 * @details Fonction d'aquisition d'une frame
 * 
 * Une trame recue est formatée avec des séparateurs ';' 
 * le protocole LoRa décompose les données : 123 devient 1, 2 ,3
 * 
 * il faut donc reformater la trame 
 * 
 * Pre condition, pas de trame qui commencent par un separateur!
 * @return bool false si erreur dans la lecture de frame 
 */

bool getFramme(byte * a_outputFrame, int a_i_outputFrameSize, int a_i_framSize) {
  if (a_i_framSize == 0) return false;

  int cursor = 0; // curseur de remplissage de la tramme finale

  byte bytesData[5] = {
    0
  }; // buffer pour les bytes à retraiter  séparés par des séparateurs 
  int bytesDataCount = 0; // compteurs du nombres d'unités ( nombre de char entre les séparateurs)

  while (LoRa.available()) // tant que de la data est dispo 
  {
    bytesData[bytesDataCount] = LoRa.read();

    if (bytesData[bytesDataCount] == ';') // si separateur
    {
      a_outputFrame[cursor++] = byteTabToByte(bytesData, bytesDataCount); // transformation du tableau de byte en un char-> stockage dans le buffer
      bytesDataCount = 0; // réinitialisation 
    } else bytesDataCount++;
  }
  Serial.print("--- [ Reception trame: ");
  printFrame(buffer, cursor);
  return true;

}

/**
 * @brief Conversion d'une suite de byte en un byte 
 * @details Le module LoRa décompose les données en byte il faut donc les rassembler 
 * 
 * @param a_byteTabToParse contien les byte a merge
 * @param dataSize le nombre d'éléments a merge
 * 
 * @return [description]
 */
byte byteTabToByte(byte * a_byteTabToParse, int dataSize) {

  byte zero = (char)
  '0';
  char result = 0;
  int puissance = dataSize - 1;
  bool negatif = false; // si un - est dans la chaine (PRE: en tete )
  for (int i = 0; i < dataSize; i++) {

    if ( * (a_byteTabToParse + i) == '-') {
      negatif = true;
      puissance--;
      continue;
    }
    result += (( * (a_byteTabToParse + i) - zero)) * pow(10, puissance--); // on monte en puissance les unités en fction de leurs place
  }
  return (byte) negatif ? -result : result; // renvoie d'un byte signé  ( char )
}