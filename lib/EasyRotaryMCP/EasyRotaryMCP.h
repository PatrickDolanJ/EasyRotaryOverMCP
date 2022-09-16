#ifndef easy_rotary
#define easy_rotary

#include <Arduino.h>
#include <RotaryEncOverMCP.h>

class EasyRotary{
public :
EasyRotary();
void checkInterrupt();
void startup();
private:
};
#endif