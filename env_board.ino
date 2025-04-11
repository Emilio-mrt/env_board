#include <Arduino.h>
#include "sensors.h"
#include "lora.h"
#include "led.h"
#include "sd_env.h"

#define mS_TO_S_FACTOR 1000ULL
#define RXD2 16
#define TXD2 12
int cpt_name=0;
void sleepMode(int delay);
void setup() {
    Serial.begin(9600);
    Serial1.begin(9600,SERIAL_8N1, RXD2, TXD2);
    initSD();    
    initLED();
    if(!get_status_sd()){
        while(1){
        setLEDColor(128, 0, 128);
        }
    }
    initSensors();
    initLoRa();
}

void loop() {
    float temperature, humidity, pressure;
    int particleCounts[3];
    readBME680(temperature, humidity, pressure);
    measure_sps(particleCounts);

    if (isLoRaModuleAvailable()) {
        if (!isLoRaJoined()) {
            joinLoRa();
            setLEDColor(100,0,0);
        } else {
            if(sendLoRaData(get_card_id(), temperature, humidity, pressure, particleCounts, readBatteryVoltage())){
                clearLED();
                if (cpt_name>=10){
                    sendLoRaName(get_card_id(),get_brd_name());
                    cpt_name=0;
                }
                cpt_name++;
                if(get_mode_eco()){
                    sleepMode(get_delay_send());
                }
                else{
                    delay(get_delay_send());
                }
            }
        }
    } else {
        setLEDColor(100,0,0);
        delay(get_delay_send());
        initLoRa();
    }
}
void sleepMode(int delay_sleep){
    Serial1.println("start_to_sleep"); 
    if(sleepLoRa())
    {
     esp_sleep_enable_timer_wakeup(delay_sleep* mS_TO_S_FACTOR);      
     esp_light_sleep_start();
     wakeLoRa();
     Serial1.println("Wake up from light sleep");
    }
    else
    {
        setLEDColor(255,165,0);
        Serial1.println("unable to sleep LoRa");
        delay(delay_sleep);
    }
}
