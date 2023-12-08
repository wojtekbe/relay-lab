#include <ArduinoJson.h>

#define MAX_CMD_LEN 100
#define NUM_RELAYS 4
String cmd;

StaticJsonDocument<200> req;
StaticJsonDocument<200> rep;

DeserializationError json_err;

enum relay_state {OPEN = LOW, CLOSE = HIGH};

struct relay_info {
   relay_state st;
   int pin;
   String name;
};

static struct relay_info relays[] = {
    // we want relays to be indexed from 1 so do not use this one
    {
        .st = OPEN,
        .pin = 666,
    },
    // Channel 1
    {
        .st = OPEN,
        .pin = 7,
        .name = "relay1"
    },
    // Channel 2
    {
        .st = OPEN,
        .pin = 6,
        .name = "relay2"
    },
    // Channel 3
    {
        .st = OPEN,
        .pin = 5,
        .name = "relay3"
    },
    // Channel 4
    {
        .st = OPEN,
        .pin = 4,
        .name = "relay4"
    },
};


void relay_set() {
    for (int i = 1; i <= NUM_RELAYS; i++) {
        digitalWrite(relays[i].pin, relays[i].st);
    }

}

void setup() {
    Serial.begin(9600);
    while(!Serial);

    for (int i = 1; i <= NUM_RELAYS; i++) {
        pinMode(relays[i].pin, OUTPUT);
        relays[i].st = OPEN;
    }
    relay_set();

}

relay_state val2state(int v) {
    if(v != 0)
        return CLOSE;
    else
        return OPEN;
}

void loop() {
    if (Serial.available()) {
        cmd = Serial.readStringUntil('\n');

        json_err = deserializeJson(req, cmd);
        if(json_err) {
            rep["status"] = "fail";
            goto reply;
        }

        String key = "relay";

        for (int i = 1; i <= NUM_RELAYS; i++) {
            String k = key + i;
            if(req.containsKey(k))
                relays[i].st = val2state(req[k]);
        }
        relay_set();

        rep["status"] = "ok";
reply:
        for (int i = 1; i <= NUM_RELAYS; i++) {
            String k = key + i;
            rep[k] = relays[i].st;
        }
        serializeJson(rep, Serial);
        Serial.println();

        req.clear();
        rep.clear();
    }
}
