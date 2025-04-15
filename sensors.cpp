#include "sensors.h"


const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};
#define STATE_SAVE_PERIOD	UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;
bsec_virtual_sensor_t sensorList[13];


void initSensors() {
    pinMode(SENSOR_PIN, INPUT);
    bme_init();
    sps_init();
}
void bme_init(){
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1);
    iaqSensor.begin(BME68X_I2C_ADDR_LOW, Wire);
    loadState();
    bsec_virtual_sensor_t sensorList[13] = {
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_STABILIZATION_STATUS,
        BSEC_OUTPUT_RUN_IN_STATUS,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_GAS_PERCENTAGE
    };
    iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
    iaqSensor.run();
}
void sps_init() { // Initialisation du capteur SPS30
    pinMode(D9, OUTPUT);
    digitalWrite(D9, HIGH);
    sensirion_i2c_init();
    sps30_set_fan_auto_cleaning_interval_days(4);
    sps30_start_measurement();
}
void readBME680(float &temperature, float &humidity, float &pressure,float &iaq) {
    if (iaqSensor.run()) { // If new data is available
        iaq=iaqSensor.iaq;
        pressure=iaqSensor.pressure;
        temperature=iaqSensor.temperature;
        humidity=iaqSensor.humidity;
        updateState();
  } else {
  }
}
void loadState()
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
    }
    iaqSensor.setState(bsecState);
  } else {
    Serial1.println("Erasing EEPROM");
    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);
    EEPROM.commit();
  }
}

void updateState()
{
  bool update = false;
  if (stateUpdateCounter == 0) {
    /* First state update when IAQ accuracy is >= 3 */
    if (iaqSensor.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD minutes */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    iaqSensor.getState(bsecState);
    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
    }
    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
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
