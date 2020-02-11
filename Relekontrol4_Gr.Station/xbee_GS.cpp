#include "arduino.h"
#include <SoftwareSerial.h>
#include "xbee_GS.h"

#define LENGTH 100

bool started= false;    //True: Message is strated
bool ended  = false;    //True: Message is finished
char incomingByte;     //Variable to store the incoming byte
char msg[LENGTH];       //Message - array from 0 to LENGTH
byte index;             //Index of array
char letter;
String cool_word = "";
char return_char = '\0';

SoftwareSerial xbee_Serial(2, 3); //RX, TX

void xbee_Serial_init() {
  xbee_Serial.begin(9600);
}

int xbee_Serial_available() {
  return xbee_Serial.available();
}

void xbee_menu_functions(){
    delay(20);
    xbee_transmit('<');
    xbee_transmit("VALVE COMMANDS:");
    xbee_transmit("F/f for fill, D/d for dump, I/i for airing.");
    delay(20);
    xbee_transmit("WEIGHT COMMANDS:");
    xbee_transmit("W to weigh, L to cont. log weight.");
    delay(20);
    xbee_transmit("IGNITION COMMANDS:");
    xbee_transmit("A/a for arming, O/o for firing.");
    delay(20);
    xbee_transmit(">");
}

char xbee_receive() {
    while (xbee_Serial.available() > 0) {
        incomingByte = xbee_Serial.read();
        if(incomingByte == '<') {
            started = true;
            index = 0;
        }
        else if(incomingByte == '>') {
            ended = true;
            break;
        }
        else {
            if(index < LENGTH) {
                msg[index] = incomingByte;
                cool_word += msg[index];
                index++;
            }
        }
    }
    if (started && ended) {
        Serial.print("Received: ");
        Serial.println(cool_word); //Use this to debug, typeof(cool_word) == String
        if (msg[0] != ' ') {
            return_char = msg[0];
        }
        cool_word = "";
        index = 0;
        msg[index] = '\0';
        started = false;
        ended = false;
        //xbee_transmit("returning: ");
        //xbee_transmit(return_char);
        return return_char;
    }
    else {
        return '\0';
    }
}

void xbee_transmit(char* cool_word) {
    xbee_Serial.print('<');
    xbee_Serial.print(cool_word);
    xbee_Serial.println('>');
    delay(40);
}

void xbee_transmit(int value) {
    xbee_Serial.print('<');
    xbee_Serial.print(value);
    xbee_Serial.println('>');
    delay(40);
}
void xbee_transmit_T(float* weight_array, int arr_size) {
    xbee_Serial.print('<');
    for (int i = 0; i < arr_size; i++) {
        xbee_Serial.print("Timestamp: ");
        xbee_Serial.print(weight_array[i]);
        delay(25);
        xbee_Serial.print("\tWeight: ");
        xbee_Serial.println(weight_array[++i]);
        delay(25);
    }
    xbee_Serial.print('>');
    delay(20);
}

void xbee_weight_spam(float weight) {
    xbee_Serial.print('<');
    xbee_Serial.print("Timestamp: ");
    xbee_Serial.print(millis());
    delay(25);
    xbee_Serial.print('\t');
    xbee_Serial.print(weight);
    xbee_Serial.print(" kg");
    delay(25);
    xbee_Serial.println('>');
}

void xbee_transmit(String cool_word, int value) {
    xbee_Serial.print('<');
    xbee_Serial.print(cool_word);
    xbee_Serial.print(" ");
    xbee_Serial.print(value);
    xbee_Serial.println('>');
    delay(40);
}

void xbee_transmit(String cool_word, float value) {
    xbee_Serial.print('<');
    xbee_Serial.print(cool_word);
    xbee_Serial.print(" ");
    xbee_Serial.print(value);
    xbee_Serial.println('>');
    delay(40);
}

void xbee_transmit(String cool_word, float value, String another_word) {
    xbee_Serial.print('<');
    xbee_Serial.print(cool_word);
    xbee_Serial.print(" ");
    xbee_Serial.print(value);
    xbee_Serial.print(" ");
    xbee_Serial.print(another_word);
    xbee_Serial.println('>');
    delay(40);
}

void xbee_transmit(float value, String cool_word) {
    xbee_Serial.print('<');
    xbee_Serial.print(value);
    xbee_Serial.print(" ");
    xbee_Serial.print(cool_word);
    xbee_Serial.println('>');
    delay(40);
}
