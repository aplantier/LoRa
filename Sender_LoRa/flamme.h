/**
 * Antoine PLANTIER 
 * Projet Lora
 * =================
 * 
 * @brief Fonctions d'utilisation du capteur de flamme
 * 
 *  + ky-026 detecteur de flamme 
 *      OutPin(pates a gauche de haut en bas ) : Analog, Ground, V+  , Digital
 *      InPin                                    A0(A0)                2(D2)     
 *      
 *      Valeur de sortie                         Max 1024              Etat Haut quand flame detectée 
 *   
 * 
 */
 

 /**
  * @brief Retoune la valeur d'infrarouge mesurée
  * @details 
  * @return [description]
  */
 float getInfrarouge(char * ){
 	float f_infrarouge_mesure = analogRead(a_flame)

 }