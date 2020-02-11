#ifndef XBEE_GS_H
#define XBEE_GS_H

void xbee_Serial_init();

// Sends a menu of functions to user
void xbee_menu_functions();

// Returns a the first letter recieved from a message, otherwise '\0' if nothing recieved
char xbee_receive();

// Sends word through xbee
void xbee_transmit(char* cool_word);

// Check if recieved a letter
int xbee_Serial_available();

// Sends value through xbee
void xbee_transmit(int value);

// Sends weight array through xbee
void xbee_transmit_T(float* weight_array, int arr_size);

// Spams the log during ignition
void xbee_weight_spam(float weight);

// Overloading transmit
void xbee_transmit(String cool_word, int value);
void xbee_transmit(String cool_word, float value);
void xbee_transmit(String cool_word, float value, String another_word);
void xbee_transmit(float value, String cool_word);

#endif /* XBEE_GS_H */
