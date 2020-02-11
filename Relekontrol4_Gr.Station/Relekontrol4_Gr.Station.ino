#include "fueling.h"
#include "weight.h"
#include "xbee_GS.h"
#define LOCAL 1 // Set this to 1 if testing without xbee

void setup(){
    setupPins();
    Serial.begin(9600);
    xbee_Serial_init();
    while(!Serial);
    Serial.println("Ready to send data");
    delay(2000);
    xbee_transmit("Ready to receive data");
    Serial.println("Sending: Ready to receive data");
    xbee_transmit("Ready to receive data");
    Serial.println("Sending: Ready to receive data");
    delay(500);
    weight_setup();
    weight_calibrate();

    //xbee_menu_functions();
}

void loop() {
    //char control = xbee_receive();
    char control = ' ';
    if (LOCAL) {
      control = Serial.read();
    }
    else {
      control = xbee_receive();
    }

    valveControl(control);
    weightControl(control);
    ignitionControl(control);
    statusCheck(control);
    delay(20);
}
