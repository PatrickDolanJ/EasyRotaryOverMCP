#ifndef easy_rotary
#define easy_rotary

#include <Arduino.h>
#include <RotaryEncOverMCP.h>

class EasyRotary{

public :
EasyRotary();
bool checkInterrupt();
void startup();
bool getClockwise();
int getId();

private:

};
#endif