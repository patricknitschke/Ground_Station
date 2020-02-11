#include "weight.h"
#include "arduino.h"
#include "xbee_GS.h"
#include "HX711.h"
#define DOUT 13 // PURPLE WIRE
#define CLK 4 // GREEN WIRE
#define LOCAL 1 // Set to 1 if debugging local

#define W_LOG_SIZE 30

// Timestamp[0], weight[0], ..., Timestamp[N], weight[N]
float weight_array_data[W_LOG_SIZE] = {0};

HX711 scale;
float calibration_factor = 20400; // Should work for us
constexpr int cal_factor_change = 50;


void weightControl(int control) {
    if (control == 'W') {
        weight_log_data();
        weight_send_data();
        weight_clear_data();
    }
    if (control == 'L') {
        weight_spam_log();
    }
}

void weight_spam_log() {
    xbee_weight_spam(weight_measure());
}

void weight_log_data() {
    for (int i = 0; i < W_LOG_SIZE; i++) {
        weight_array_data[i++] = millis();
        weight_array_data[i] = weight_measure();
        delay(20);
    }
}

void weight_send_data() {
    xbee_transmit("-----WEIGHT LOG-----\n");
    Serial.println("-----WEIGHT LOG-----\n");
    if (LOCAL) {
        for (int i = 0; i < W_LOG_SIZE; i++) {
        Serial.print("Timestamp: ");
        Serial.print(weight_array_data[i]);
        delay(25);
        Serial.print("\tWeight: ");
        Serial.println(weight_array_data[++i]);
        delay(25);
        }
    } else {
        xbee_transmit_T(weight_array_data, W_LOG_SIZE);
    }
    xbee_transmit("-----END OF LOG-----\n");
    Serial.println("-----END OF LOG-----\n");
}

void weight_print_data() {
    xbee_transmit("-----WEIGHT LOG-----\n");
    Serial.println("-----WEIGHT LOG-----\n");
    for (int i = 0; i < W_LOG_SIZE; i++) {
        xbee_transmit(weight_array_data[i]);
        Serial.println(weight_array_data[i]);
        xbee_transmit('\t');
        Serial.println('\t');
        xbee_transmit(weight_array_data[++i]);
        Serial.println(weight_array_data[i]);
        xbee_transmit(" kg\n");
        Serial.println(" kg\n");
        delay(50);
    }
    xbee_transmit("-----END OF LOG-----\n");
    Serial.println("-----END OF LOG-----\n");
}

void weight_clear_data() {
    for (int i = 0; i < W_LOG_SIZE; i++) {
		    weight_array_data[i] = 0;
	}
}

void weight_setup() {
    delay(20);
    xbee_transmit("READY TO CALIBRATE WEIGHT CELLS");
    Serial.println("READY TO CALIBRATE WEIGHT CELLS");
    xbee_transmit("Press + or a to increase calibration factor");
    Serial.println("Press + or a to increase calibration factor");
    delay(20);
    xbee_transmit("Press - or z to decrease calibration factor");
    Serial.println("Press - or z to decrease calibration factor");
    xbee_transmit("Press XXXXX to end calibration and begin testing");
    Serial.println("Press XXXXX to end calibration and begin testing");
    delay(500);

    scale.begin(DOUT, CLK);
    scale.set_scale();
    scale.tare(); //Reset the scale to 0

    long zero_factor = scale.read_average(); //Get a baseline reading
    xbee_transmit("Zero factor:", (float)zero_factor); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor);
}

void weight_calibrate() {
    int calibrating = 1;
    while (calibrating) {
        static int counter = 0;
        if (++counter > 15) {
            scale.set_scale(calibration_factor);
            delay(20);
            //xbee_transmit("Reading:", weight, "kg");
            Serial.print("Reading: ");
            Serial.print(weight_measure());
            Serial.println(" kg");
            delay(20);
            xbee_transmit("Cal. factor:", calibration_factor);
            Serial.println("Cal. factor: ");
            Serial.println(calibration_factor);
            counter = 0;
        }
        char check;
        if (!LOCAL) {
            check = xbee_receive();
        } else {
            check = Serial.read();
        }
        if (check == '+' || check == 'a') {
            calibration_factor += cal_factor_change;
        }
        else if (check == '-' || check == 'z') {
            calibration_factor -= cal_factor_change;
        }
        else if (check == 'X') {
            calibrating = 0;
            break;
        }
        delay(20);
    }
    xbee_transmit("Calibration complete.");
    Serial.println("Calibration complete.");
}

float weight_measure() {
    return scale.get_units();
}
