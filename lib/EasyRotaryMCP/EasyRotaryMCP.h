#ifndef easy_rotary
#define easy_rotary

#include <Arduino.h>
#include <RotaryEncOverMCP.h>

struct RotaryData{
    public:
    bool clockwise;
    int id;
    bool changed;
};


class EasyRotary{

public :
EasyRotary();
RotaryData checkInterrupt();
void startup(void (*function)(bool,int));
bool getClockwise();
int getId();

private:

};
#endif