#include "lora.h"


static bool is_exist = false;
static bool is_join = false;
static char recv_buf[512];

static int at_send_check_response(const char *expected_ack, int timeout_ms, const char *command_format, ...) {
    int character;
    int index = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, command_format);
    Serial1.printf(command_format, args);
    Serial.printf(command_format, args);
    va_end(args);
    delay(200);
    int startMillis = millis();
    do {
        while (Serial.available() > 0) {
            character = Serial.read();
            recv_buf[index++] = character;
            Serial1.print((char)character);
            delay(2);
        }
        if (strstr(recv_buf, expected_ack) != NULL) {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    Serial1.println("Not ACK");
    return 0;
}

void initLoRa() {
    at_send_check_response("+FDEFAULT: OK", 1000, "AT+FDEFAULT\r\n");
    at_send_check_response("+RESET: OK", 1000, "AT+RESET\r\n");
    sleep(2);
    if (at_send_check_response("+AT: OK", 1000, "AT\r\n")) {
        is_exist = true;
        at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n");
        at_send_check_response("+DR: EU868", 1000, "AT+DR=EU868\r\n");
        at_send_check_response("+CLASS: A", 1000, "AT+CLASS=A\r\n");
        at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,0-2\r\n");
        at_send_check_response("+DELAY:", 1000, "AT+DELAY=JRX1,6000\r\n");
        at_send_check_response("+DELAY:", 1000, "AT+DELAY=JRX2,6000\r\n");
        at_send_check_response("", 1000, "AT+ID\r\n");

        if(get_mode_eco()){
            at_send_check_response("+DR:", 1000, "AT+DR=DR3\r\n");
            at_send_check_response("+POWER: ", 1000, "AT+POWER=14\r\n");
            }//Si sur batterie
        else{
            at_send_check_response("+DR:", 1000, "AT+DR=DR3\r\n");
            at_send_check_response("+POWER: ", 1000, "AT+POWER=22\r\n");
            }
        String appKey = get_APP_KEY();
        char commandBuffer[64];
        snprintf(commandBuffer, sizeof(commandBuffer), "AT+KEY=APPKEY,\"%s\"\r\n", appKey.c_str());
        at_send_check_response("+KEY: APPKEY", 1000,commandBuffer);
        is_join = false;
    } else {
        is_exist = false;
    }
}

bool isLoRaModuleAvailable() { return is_exist; }

bool isLoRaJoined() { return is_join; }

void joinLoRa() {
    for(int i=0; i<try_to_join ; i++){
        if (at_send_check_response("+JOIN: Network joined", 11000, "AT+JOIN\r\n")) {
            is_join = true;
            sendLoRaName(get_card_id(),get_brd_name());
            Serial1.println("ok");
            return;
        }
        else if (at_send_check_response("+JOIN: Joined already", 11000, "AT+JOIN\r\n")) {
            is_join = true;
            sendLoRaName(get_card_id(),get_brd_name());
            Serial1.println("ok1");
            return;
        } else {
            at_send_check_response("+JOIN: Join failed", 11000, "AT+JOIN=FORCE\r\n");
        }
    }
    is_join = false;
}

bool sleepLoRa(){
    if (at_send_check_response("+LOWPOWER: SLEEP", 5000,"AT+LOWPOWER\r\n")) {
        return true;
    }
    return false;
}
bool wakeLoRa(){
    if (at_send_check_response("+LOWPOWER: WAKEUP", 5000,"A\r\n")) {
        sleep(1);
        return true;
    }
    return false;
}
bool sendLoRaData(uint16_t card_id, float temp, float humi, float pressure, int particles[3], float vbat,float iaq) {
    char frame[115];
    int lenght = snprintf(frame, sizeof(frame), "AT+CMSG=\"1;%u;%.2f;%.2f;%.2f;%d;%d;%d;%.2f;%.2f\"\r\n", card_id, temp, humi, pressure, particles[0], particles[1], particles[2], vbat,iaq);
    Serial1.println(lenght);
    if (!at_send_check_response("Done", 50000, frame)) {
        return false;
        is_join=false;
    }
    else{
        return true;
    }
}
bool sendLoRaName(uint16_t card_id,String name_brd) {
    char frame[115];
    int lenght = snprintf(frame, sizeof(frame), "AT+CMSG=\"2;%u;%s\"\r\n", card_id,name_brd.c_str());
    Serial1.println(lenght);
    if (!at_send_check_response("Done", 50000, frame)) {
        return false;
        is_join=false;
    }
    else{
        return true;
    }
}
