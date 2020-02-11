#ifndef FUELING_H
#define FUELING_H


// Configure pinModes
void setupPins();

// 'F', 'D', 'I' for fill, dump, air; smaller case to close valves
void valveControl(char valve);

// 'A', 'O' for arm and fire; smaller case to stop
void ignitionControl(char order);

// Ignition function with weight measurement
void ignite_n_weigh();

// Check = 'S' to print out states;
void statusCheck(char check);


#endif /* FUELING_H */
