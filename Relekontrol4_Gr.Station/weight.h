#ifndef WEIGHT_H
#define WEIGHT_H

// Weight function menu
void weightControl(int control);

// Log the weight data into the weight array
void weight_log_data();

// Spam log during ignition
void weight_spam_log();

// Empty the weight array
void weight_clear_data();

// Print data to screen
void weight_print_data();

// Send the array over xBee
void weight_send_data();

// Setup weight pins and tare
void weight_setup();

// Calibration function
void weight_calibrate();

// Return current weight
float weight_measure();

#endif /* WEIGHT_H */
