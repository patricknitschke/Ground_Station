#include "Arduino.h"
#include "xbee_GS.h"
#include "weight.h"
#include "fueling.h"

#define outDumpPin 5
#define outFillPin 6
#define outArmPin 7
#define outPyroPin 8
#define outOxPin 9
#define outAirPin 10

int armingStatus = 0;
int fillStatus = 0;
int dumpStatus = 0;
int airingStatus = 1;

#define wait 1000
#define oxdelay 500

void setupPins() {
    pinMode(outDumpPin, OUTPUT);
    digitalWrite(outDumpPin, HIGH);
    pinMode(outFillPin, OUTPUT);
    digitalWrite(outFillPin, HIGH);
    pinMode(outArmPin, OUTPUT);
    digitalWrite(outArmPin, HIGH);
    pinMode(outPyroPin, OUTPUT);
    digitalWrite(outPyroPin, HIGH);
    pinMode(outOxPin, OUTPUT);
    digitalWrite(outOxPin, HIGH);
    pinMode(outAirPin, OUTPUT);
    digitalWrite(outAirPin, HIGH);
}

void valveControl(char valve){
  	switch(valve){
	case 'D':
		if(fillStatus == 0){
			digitalWrite(outDumpPin, LOW);
			dumpStatus = 1;
			xbee_transmit("Dump valve is now OPEN");
			Serial.println("Dump valve is now OPEN");
		} else {
			xbee_transmit("Filling valve must be closed while dumping.");
			Serial.println("Filling valve must be closed while dumping.");
		}
		break;
    case 'd':
		digitalWrite(outDumpPin, HIGH);
		dumpStatus = 0;
		xbee_transmit("Dump valve is now CLOSED");
		Serial.println("Dump valve is now CLOSED");
		break;
    case 'F':
		if(dumpStatus == 0){
			digitalWrite(outFillPin, LOW);
			fillStatus = 1;
			xbee_transmit("Fill valve is now OPEN");
			Serial.println("Fill valve is now OPEN");
		} else {
			xbee_transmit("Dump valve must be closed while filling.");
			Serial.println("Dump valve must be closed while filling.");
		}
		break;
    case 'f':
		digitalWrite(outFillPin, HIGH);
		fillStatus = 0;
		xbee_transmit("Fill valve is now CLOSED");
		Serial.println("Fill valve is now CLOSED");
		break;
    case 'I':
		digitalWrite(outAirPin, LOW);
		airingStatus = 0;
		xbee_transmit("Airing valve is now CLOSED");
		Serial.println("Airing valve is now CLOSED");
		break;
    case 'i':
		digitalWrite(outAirPin, HIGH);
		airingStatus = 1;
		xbee_transmit("Airing valve is now OPEN");
		Serial.println("Airing valve is now OPEN");
		break;
	}
}

void ignitionControl(char order){
    switch(order){
	case 'A':
		if((dumpStatus == 0) && (fillStatus == 0) && (airingStatus == 0)){
			xbee_transmit("Caution: Rocket is about to arm. Confirm with Y/N");
			Serial.println("Caution: Rocket is about to arm. Confirm with Y/N");
			char confirmArm = ' ';
			while(true){
				delay(20);
				confirmArm = xbee_receive();
				if(confirmArm == 'Y'){
					digitalWrite(outArmPin, LOW);
					armingStatus = 1;
					xbee_transmit("WARNING! WARNING! WARNING!");
					Serial.println("WARNING! WARNING! WARNING!");
					xbee_transmit("ROCKET IS NOW ARMED AND READY TO FIRE. COMMAND a TO DISARM");
					Serial.println("ROCKET IS NOW ARMED AND READY TO FIRE. COMMAND a TO DISARM");
					break;
				}
				else if (confirmArm == 'N') {
						xbee_transmit("Arming cancelled");
						Serial.println("Arming cancelled");
					break;
				}
				else if (xbee_Serial_available() > 0){
					xbee_transmit("To Arm: Type 'Y', otherwise 'N'");
					Serial.println("To Arm: Type 'Y', otherwise 'N'");
				}
			}
		}
  	    else {
  	    xbee_transmit("No valves open when arming!");
  	    Serial.println("No valves open when arming!");
  	  	}
		break;
	case 'a':
		digitalWrite(outArmPin, HIGH);
		armingStatus = 0;
		xbee_transmit("Rocket is not armed, and will not fire");
		Serial.println("Rocket is not armed, and will not fire");
		break;
	case 'O':
		if(armingStatus == 1){
			xbee_transmit("Rocket will fire in 5 seconds... COMMAND o TO CANCEL");
			Serial.println("Rocket will fire in 5 seconds... COMMAND o TO CANCEL");
      ignite_n_weigh();
		}
		break;
	case 'o':
		digitalWrite(outPyroPin, HIGH);
		digitalWrite(outOxPin, HIGH);
		digitalWrite(outArmPin, HIGH);
		xbee_transmit("Pyro, arm and ox disabled");
		Serial.println("Pyro, arm and ox disabled");
	break;
	}
}

void ignite_n_weigh() {
	delay(1 * wait);
	Serial.println("Beginning weight log... (Send X to end logging)");
	float startTime = millis()/1000.0;
	float runTime = 0;
	int pyro_ignited = 0;
	int ox_activated = 0;
	static int counter = 0;
	while (runTime < 20) {
		char command = xbee_receive();
		runTime = (millis()/1000.0 - startTime);

		if (command == 'o' || command == 'X') {
			xbee_transmit("Pyro, arm and ox disabled.");
			Serial.println("Pyro, arm and ox disabled.");
			xbee_transmit("Weight logging ended.");
			Serial.println("Weight logging ended.");
			break;
		}
		if (runTime >= 4 && !pyro_ignited) {
			digitalWrite(outPyroPin, LOW);//IGNITES PYRO
			delay(20);
			xbee_transmit("----------IGNITING PYRO-----------");
			Serial.println("----------IGNITING PYRO-----------");
			xbee_transmit("----------IGNITING PYRO-----------");
			Serial.println("----------IGNITING PYRO-----------");
			xbee_transmit("----------IGNITING PYRO-----------");
			Serial.println("----------IGNITING PYRO-----------");
			delay(20);
			pyro_ignited = 1;
		}
		if ((runTime >= 4 + oxdelay/1000.0) && !ox_activated) {
			digitalWrite(outOxPin, LOW); //ACTIVATES RUN VALVE
			delay(20);
			xbee_transmit("----------OPENING RUN VALVE-----------");
			Serial.println("----------OPENING RUN VALVE-----------");
			xbee_transmit("----------OPENING RUN VALVE-----------");
			Serial.println("----------OPENING RUN VALVE-----------");
			xbee_transmit("----------OPENING RUN VALVE-----------");
			Serial.println("----------OPENING RUN VALVE-----------");
			delay(20);
			ox_activated = 1;
		}
		weight_spam_log();
	}
	xbee_transmit("Rocket has fired.");
	Serial.println("Rocket has fired.");
}

void statusCheck(char check){
    if (check == 'S'){
		delay(20);
        xbee_transmit("\nCurrent status is:\n");
        Serial.println("\nCurrent status is:\n");
        xbee_transmit("Dumpstatus is: ", dumpStatus);
        Serial.print("Dumpstatus is: ");
        Serial.println(dumpStatus);
		delay(20);
        xbee_transmit("Airing status is: ", airingStatus);
        Serial.println("Airing status is: ");
        Serial.println(airingStatus);

        xbee_transmit("Filling status is: ", fillStatus);
        Serial.print("Filling status is: ");
		Serial.println(fillStatus);
		delay(20);
        xbee_transmit("Arming status is: ", armingStatus);
        Serial.print("Arming status is: ");
        Serial.println(armingStatus);
		delay(20);
    }
}
