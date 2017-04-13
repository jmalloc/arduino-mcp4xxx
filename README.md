# arduino-mcp4xxx

Arduino library for controlling [Microchip's MCP4XXX](http://ww1.microchip.com/downloads/en/DeviceDoc/22060b.pdf) range of SPI controlled digital potentiometers.

## Supported Devices

The supported chips are designated as follows:

```
MCP 4 X Y Z
      │ │ │
      │ │ └── (Z) Wiper Configuration
      │ │         1 = Potentiometer, 2 = Rheostat
      │ │
      │ └──── (Y) Memory Type / Resolution
      │           3 = 7-bit RAM, 4 = 7-bit EEPROM, 5 = 8-bit RAM, 6 = 8-bit EEPROM
      │
      └────── (X) Number of pots
                  1 or 2
```

This encompasses: MCP4131, MCP4151, MCP4231, MCP4251, MCP4132, MCP4152, MCP4232 and MCP4252.

### SPI connection on MCP4X1X

Note that the MCP41X1 chips multiplex SDI and SDO on a single pin. To use these chips with a standard SPI interface as on the Arduino you will need to
connect the shared SDI/SDO pin on the pot to the Arduino's MISO pin, then bridge the MISO pin to the MOSI pin with a resistor (3k9 resistor seems to work well).

## Installation

 * Download the library (a list of available versions is [here](https://github.com/jmalloc/arduino-mcp4xxx/releases))
 * Exit the Arduino IDE
 * Copy `mcp4xxx.h` and `mcp4xxx.cpp` files to a folder named `mcp4xxx` in your Arduino libraries folder
 * Restart the Arduino IDE, the library should now be available by including `mcp4xxx.h` as per the example below

More information is available under the section titled "Manual Installation" on the [libraries page](http://arduino.cc/en/Guide/Libraries) of the official Arduino website.
 
## Example

```c++
#include <SPI.h>
#include <mcp4xxx.h>

using namespace icecave::arduino;

MCP4XXX* pot;

void setup()
{
    // Construct an instance of the MCP4XXX to manage the digipot.
    // The first parameter is the pin number to use for 'chip select' (CS), if you are
    // using the default SPI CS pin for your Arduino you can simply omit this parameter.
    pot = new MCP4XXX(49);
}

void loop()
{
    // Move the wiper to the lowest value
    pot->set(0);

    // Move the wiper to the highest value
    pot->set(pot->max_value());

    // Increment the wiper position by one
    pot->increment();

    // Decrement the wiper position by one
    pot->decrement();
}
```

The `MCP4XXX` class supports the full range of commands available on the MCP4XXX range of devices,
for more information please see the API documentation in the [header file](mcp4xxx.h).

## Known Issues / Caveats

* Does not support entire 4XXX range, notably absent is support for I2C and quad-pot devices.
* Only tested with MCP4151, which does not have a hardware shutdown pin.
