#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <stdarg.h>
#include "sd_env.h"
#define  try_to_join  5 //Nb. de tentative Join
void initLoRa();
bool isLoRaModuleAvailable();
bool isLoRaJoined();
void joinLoRa();
bool sleepLoRa();
bool wakeLoRa();
bool sendLoRaData(uint16_t card_id,float temp, float humi, float pressure, int particles[3], float vbat,float iaq);
bool sendLoRaName(uint16_t card_id,String name_brd);


#endif 