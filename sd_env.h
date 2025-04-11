#ifndef SD_ENV_H 
#define SD_ENV_H

#include <Arduino.h>
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "led.h"

#define PATH "/conf.txt"
#define CONFIG_STRING \
"###############################################################################\n" \
"# Configuration du fichier conf.txt\n" \
"# Ce fichier contient les paramètres de configuration\n" \
"###############################################################################\n" \
"# Identifiant de la carte (entier)\n" \
"card_id=1000\n" \
"\n" \
"# Nom de l'application (chaîne de caractères)\n" \
"name=envbrd\n" \
"\n" \
"# Tension de la batterie (entier)\n" \
"# 0 si pas de batterie\n" \
"V_batterie=0\n" \
"\n" \
"# Clé d'application (chaîne de caractères)\n" \
"APPKEY=A7C8DD6949694FCD3D4792918F9F6791\n" \
"\n" \
"# Mode éco (entier : 0 ou 1)\n" \
"mode_eco=false\n" \
"\n" \
"# Délai d'envoi (entier, en millisecondes)\n" \
"time_delay_send=10000"


void initSD();
String getConfigValue(File configFile, const String& key);
int get_card_id();
String get_brd_name();
float get_v_batt_nom();
String get_APP_KEY();
bool get_mode_eco();
int get_delay_send();
bool get_status_sd();

#endif