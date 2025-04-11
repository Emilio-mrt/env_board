#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "sensors.h"
#include <Wire.h>
#include "DFRobot_BME680_I2C.h"
#include <sps30.h>
#include "sd_env.h"

#define SENSOR_PIN A2

void initSensors();
void sps_init();
void readBME680(float &temperature, float &humidity, float &pressure);
void measure_sps(int particleCounts[3]);
float readBatteryVoltage();

#endif