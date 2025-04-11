#include "sd_env.h"

// Variables globales pour les configurations
int card_id = 1000;
String brd_name = "";
float v_batt_nom = 0;
String APP_KEY = "A7C8DD6949694FCD3D4792918F9F6791";
bool mode_eco = 1;
int delay_send = 10000;
bool is_SD_exist = true;
// getteur 
int get_card_id(){
  return card_id;
}
String get_brd_name(){
  return brd_name;
}
float get_v_batt_nom(){
  return v_batt_nom;
}
String get_APP_KEY(){
  return APP_KEY;
}
bool get_mode_eco(){
  return mode_eco;
}
int get_delay_send(){
  return delay_send;
}
bool get_status_sd(){
  return is_SD_exist;
}

// Fonction pour extraire une valeur de configuration
String getConfigValue(File configFile, const String& key) {
  configFile.seek(0); // Remettre le curseur au début du fichier
  String line;
  while (configFile.available()) {
    line = configFile.readStringUntil('\n');
    int index = line.indexOf(key + "=");
    if (index != -1) {
      int startIndex = index + key.length() + 1;
      int endIndex = line.indexOf('\n', startIndex);
      if (endIndex == -1) {
        endIndex = line.length();
      }
      return line.substring(startIndex, endIndex);
    }
  }
  return ""; // Clé non trouvée
}

void initSD() {
  if (SD.begin()) {
    Serial1.println("SD connected");
    if (SD.exists("/conf.txt")) {
      File configFile = SD.open("/conf.txt");
      if (configFile) {
        // Lire les valeurs de configuration
        String cardIdStr = getConfigValue(configFile, "card_id");
        String name = getConfigValue(configFile, "name");
        String vBatterieStr = getConfigValue(configFile, "V_batterie");
        String appKey = getConfigValue(configFile, "APPKEY");
        String modeEcoStr = getConfigValue(configFile, "mode_eco");
        String timeDelaySendStr = getConfigValue(configFile, "time_delay_send");

        // Convertir et assigner les valeurs aux variables globales
        card_id = cardIdStr.toInt();
        brd_name = name;
        v_batt_nom = vBatterieStr.toFloat();
        APP_KEY = appKey;
        mode_eco = modeEcoStr.toInt();
        delay_send = timeDelaySendStr.toInt();

        // Fermer le fichier
        configFile.close();

        // Afficher les valeurs récupérées
        Serial1.println("Valeurs de configuration récupérées :");
        Serial1.println("card_id: " + cardIdStr);
        Serial1.println("name: " + brd_name);
        Serial1.println("bat: " + vBatterieStr);
        Serial1.print("V_batterie: ");
        Serial1.println(v_batt_nom);
        Serial1.println("APPKEY: " + appKey);
        Serial1.println("mode_eco: " + modeEcoStr);
        Serial1.println("time_delay_send: " + timeDelaySendStr);

      } else {
        Serial1.println("Erreur lors de l'ouverture du fichier conf.txt.");
        is_SD_exist=false;

      }
    } else {
      File file = SD.open("/conf.txt", FILE_WRITE);
      if (file) {
        file.print(CONFIG_STRING);

        file.close();
      } else {
        Serial1.println("Failed to open file for writing");
        is_SD_exist=false;

      }
    }
  } else {
    Serial1.println("No SD card");
    is_SD_exist=false;

  }
}
