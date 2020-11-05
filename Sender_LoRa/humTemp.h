/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 * Fonctions gèrant le capteur d'humidite et de temperature
 * 
 * 
 * Vu la librairie DHT https://github.com/adafruit/DHT-sensor-library
 * Ne fonctionne pas parfaitement et contre productif pour récuperer les valeurs en char*
 */

byte read_data();
void probe_humTemp();

/**
 * @brief Fonction de mesure de la temperature et stockage dans le tableau global humTmp
 * @details [long description]
 * @return true si tout s'est bien passé 
 */
bool humTemp() {

  probe_humTemp(); // mesure

  byte checksum = humTmp[0] + humTmp[1] + humTmp[2] + humTmp[3];

  if (humTmp[4] != checksum)
    return false;

  return true;
}

byte read_data() {
  byte i = 0;
  byte result = 0;
  for (i = 0; i < 8; i++) {
    while (digitalRead(d_tempHum) == LOW); // debut de la transmission par un etat bas
    delayMicroseconds(30); //laisser le temps de changement de bit
    if (digitalRead(d_tempHum) == HIGH)
      result |= (1 << (8 - i)); //High in the former, low in the post
    while (digitalRead(d_tempHum) == HIGH); //d_tempHumhumTmpa '1', waiting for the next bit of reception
  }
  return result;
}

void probe_humTemp() {
  digitalWrite(d_tempHum, LOW); //Etat bas au bus pour initier signal => debut mesure de la sonde 
  delay(40); //un peu de temps pour laisser la sonde mesurer 

  digitalWrite(d_tempHum, HIGH); // Etat haut pour initier l'envoi des datas 
  delayMicroseconds(40); //Wait for DHT11 to respond

  pinMode(d_tempHum, INPUT); // on passe en mode reception 
  while (digitalRead(d_tempHum) == HIGH); // reception etat bas debut de trans;ition 
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;

  if (digitalRead(d_tempHum) == LOW)
    delayMicroseconds(80); //
  for (int i = 0; i < 5; i++)
    humTmp[i] = read_data();
  pinMode(d_tempHum, OUTPUT); // fin de reception, sonde desactivee
  digitalWrite(d_tempHum, HIGH); //After the completion of a release of data bus, waiting for the host to start the next signal

}