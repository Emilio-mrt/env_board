#include "sensors.h"

DFRobot_BME680_I2C bme(0x76);

void initSensors() {
    pinMode(SENSOR_PIN, INPUT);
    if(bme.begin()){Serial1.println("Bme initialisé");}
    else{Serial1.println("Bme non initialisé");}
    bme.startConvert();
    delay(1000);
    bme.update();
    sps_init();

}
void sps_init() { // Initialisation du capteur SPS30
    pinMode(D9, OUTPUT);
    digitalWrite(D9, HIGH);
    sensirion_i2c_init();
    sps30_set_fan_auto_cleaning_interval_days(4);
    sps30_start_measurement();
}
void readBME680(float &temperature, float &humidity, float &pressure) {
    bme.startConvert();
    delay(1000);
    bme.update();
    temperature = bme.readTemperature() / 100;
    humidity = bme.readHumidity() / 1000;
    pressure = bme.readPressure() / 100;
}

void measure_sps(int particleCounts[3]) {
    if(get_mode_eco()){
        sps30_wake_up();
        sleep(1);
    }
    struct sps30_measurement m;
    if (sps30_read_measurement(&m) < 0) {
        particleCounts[0] = particleCounts[1] = particleCounts[2] = 0;
    } else {
        particleCounts[0] = (m.nc_10p0 - m.nc_0p5) * 10000;
        particleCounts[1] = (m.nc_10p0 - m.nc_1p0) * 10000;
        particleCounts[2] = (m.nc_10p0 - m.nc_4p0) * 10000;
    }
    if(get_mode_eco()){
    sps30_sleep();
    }
}

float readBatteryVoltage() {
    return analogRead(SENSOR_PIN) * ((get_v_batt_nom())/ 4096) * 2;
}
