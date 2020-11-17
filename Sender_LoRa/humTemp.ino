void humTemp(void){
  
probe_humTemp();
  char  pc_humTempTrame[16]={0};
  byte checksum = bytab_tempHum[0] + bytab_tempHum[1] + bytab_tempHum[2] + bytab_tempHum[3];
  if (bytab_tempHum[4] != checksum)
    sprintf(pc_humTempTrame,"[H:err][T:err]"); 
  else 
    sprintf(pc_humTempTrame,"[H%d.%d][T%d.%d]",bytab_tempHum[0],bytab_tempHum[1],bytab_tempHum[2],bytab_tempHum[3]);
  Serial.println(pc_humTempTrame);
     
}

byte read_data()
{
  byte i = 0;
  byte result = 0;
  for (i = 0; i < 8; i++) {
      while (digitalRead(dp_tempHum) == LOW); // wait 50us
      delayMicroseconds(30); //The duration of the high level is judged to determine whether the dp_tempHumbytab_tempHuma is '0' or '1'
      if (digitalRead(dp_tempHum) == HIGH)
        result |= (1 << (8 - i)); //High in the former, low in the post
    while (digitalRead(dp_tempHum) == HIGH); //dp_tempHumbytab_tempHuma '1', waiting for the next bit of reception
    }
  return result;
}

void probe_humTemp(){
  digitalWrite(dp_tempHum, LOW); //Etat bas au bus pour initier signal => debut mesure de la sonde 
  delay(30); //un peu de temps pour laisser la sonde mesurer 
  
  digitalWrite(dp_tempHum, HIGH);// Etat haut pour initier l'envoi des datas 
  delayMicroseconds(40); //Wait for DHT11 to respond
  
  pinMode(dp_tempHum, INPUT);// on passe en mode reception 
  while(digitalRead(dp_tempHum) == HIGH); // reception etat bas debut de trans;ition 
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;
  
  if(digitalRead(dp_tempHum) == LOW)
    delayMicroseconds(80); //
  for(int i = 0; i < 5; i++) 
    bytab_tempHum[i] = read_data();
  pinMode(dp_tempHum, OUTPUT); // fin de reception, sonde desactivee
  digitalWrite(dp_tempHum, HIGH); //After the completion of a release of data bus, waiting for the host to start the next signal

}
