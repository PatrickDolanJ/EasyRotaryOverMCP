

/*Do not try and download these two libraries: <Adafruit_MCP23017.h><Rotary.h> 
 * Instead, download the zip from: https://github.com/maxgerhardt/rotary-encoder-over-mcp23017 
 * The libraries are in there and I think he modified them.  I had to move the libraries from his
 * "lib" folder into arduino library folder and then put each on in a folder in order for Arduino to
 * see them.  
 * 
 * Another important thing is that if you are using just rotary encoders and not ones mounted to a breakout 
 * board, you MUST use 10k pulldown resistors on the encoders outside leads.  the encoders center lead 
 * is tied to 5VDC.  
 * 
 * One last thing.  The pin numbering stuff below is very confusing unless you know to look at: 
 * https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/ Pull this up then stare at the 
 * code and itll make sense
 * 
 * The default address as to be 0x20 although I don't see it anywhere


*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>
#include <EasyRotaryMCP.h>

#if defined(ESP32) || defined(ESP8266)
#define INTERRUPT_FUNC_ATTRIB IRAM_ATTR
#else
#define INTERRUPT_FUNC_ATTRIB  
#endif

/* Our I2C MCP23017 GPIO expanders */
Adafruit_MCP23017 mcp;

typedef struct RotaryData Struct;

RotaryData data;

// bool EasyRotary::getClockwise(){
//     return data.clockwise;
// }

// int EasyRotary::getId(){
//     return data.id;
// }


//Array of pointers of all MCPs if there is more than one
Adafruit_MCP23017* allMCPs[] = { &mcp };
constexpr int numMCPs = (int)(sizeof(allMCPs) / sizeof(*allMCPs));
/* function prototypes */
void intCallBack();
void cleanInterrupts();
void handleInterrupt();
void RotaryEncoderChanged(bool clockwise, int id);


/* the INT pin of the MCP can only be connected to
 * an interrupt capable pin on the Arduino, either
 * D3 or D2.
 * */
byte arduinoIntPin = 2;




/* variable to indicate that an interrupt has occured */
volatile boolean awakenByInterrupt = false;
//function pointer
typedef void (*rotaryActionFunc)(bool clockwise, int id);
rotaryActionFunc ActionFunction = nullptr;
void ThisIsAFunction(bool,int);

//function prototypes placeholder

/* Array of all rotary encoders and their pins */
RotaryEncOverMCP rotaryEncoders[] = {
    RotaryEncOverMCP(&mcp, 0, 1, &ThisIsAFunction, 1),// physical pins 21 and 22  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 2, 3, &ThisIsAFunction, 2),// physical pins 23 and 24  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 4, 5, &ThisIsAFunction, 3),// physical pins 25 and 26  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 6, 7, &ThisIsAFunction, 4),// physical pins 27 and 28  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 8, 9, &ThisIsAFunction, 5),// physical pins 1 and 2  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 10, 11, &ThisIsAFunction, 6),// physical pins 3 and 4  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    RotaryEncOverMCP(&mcp, 12, 13, &ThisIsAFunction, 8)// physical pins 7 and 8  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
    
};
constexpr int numEncoders = (int)(sizeof(rotaryEncoders)/sizeof(*rotaryEncoders));
void ThisIsAFunction(bool clockwise,int id){
    ActionFunction(clockwise, id);
}


// void RotaryEncoderChanged(bool clockwise, int id) {
    
//     //   Serial.println("Encoder " + String(id) + ": "
//     //           + (clockwise ? String("clockwise") : String("counter-clock-wise")));
//             data.clockwise = clockwise;
//             data.id = id; 
//             //Serial.println(String(data.id) + String(data.clockwise));
// }

void EasyRotary::startup(void (*function)(bool clockwise,int id)){
    ActionFunction = function;
    pinMode(arduinoIntPin,INPUT);
    mcp.begin();      // use default address 0
    mcp.readINTCAPAB(); //read this so that the interrupt is cleared

    //initialize all rotary encoders

    //Setup interrupts, OR INTA, INTB together on both ports.
    //thus we will receive an interrupt if something happened on
    //port A or B with only a single INT connection.
    //mcp.setupInterrupts(true,false,LOW);

    //Initialize input encoders (pin mode, interrupt)
    for(int i=0; i < numEncoders; i++) {
        rotaryEncoders[i].init();
    }
    attachInterrupt(digitalPinToInterrupt(arduinoIntPin), intCallBack, FALLING);
}

EasyRotary::EasyRotary(){
    Serial.println("MCP23017 Interrupt");
}

// The int handler will just signal that the int has happened
// we will do the work from the main loop.
void INTERRUPT_FUNC_ATTRIB intCallBack() {
    awakenByInterrupt=true;
}

RotaryData EasyRotary::checkInterrupt() {
    if(awakenByInterrupt) {
        // disable interrupts while handling them.
        detachInterrupt(digitalPinToInterrupt(arduinoIntPin));
        handleInterrupt();
        attachInterrupt(digitalPinToInterrupt(arduinoIntPin),intCallBack,FALLING);
    }
    data.changed = awakenByInterrupt;
    return data;
}

void handleInterrupt(){
    //Read the entire state when the interrupt occurred

    //An interrupt occurred on some MCP object.
    //since all of them are ORed together, we don't
    //know exactly which one has fired.
    //just read all of them, pre-emptively.

    for(int j = 0; j < numMCPs; j++) {
        uint16_t gpioAB = allMCPs[j]->readINTCAPAB();
        // we need to read GPIOAB to clear the interrupt actually.
        volatile uint16_t dummy = allMCPs[j]->readGPIOAB();
        for (int i=0; i < numEncoders; i++) {
            //only feed this in the encoder if this
            //is coming froma the correct MCP
            if(rotaryEncoders[i].getMCP() == allMCPs[j])
                rotaryEncoders[i].feedInput(gpioAB);
        }
    }
    cleanInterrupts();
}

// handy for interrupts triggered by buttons
// normally signal a few due to bouncing issues
void cleanInterrupts(){
#ifdef __AVR__
    EIFR=0x01;
#endif
    awakenByInterrupt=false;
}
