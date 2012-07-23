# arduino-mcp4xxx
## Arduino library for controlling [Microchip's MCP4XXX](http://ww1.microchip.com/downloads/en/DeviceDoc/22060b.pdf) range of digital potentiometers.

The supported chips are configured as follows:

```
MCP 4 X Y Z
      | | |
      | | +- Z = Wiper Configuration (1 = Potentiometer, 2 = Rheostat).
      | |
      | +--- Y = Memory Type / Resolution (3 = 7-bit RAM, 4 = 7-bit EEPROM, 5 = 8-bit RAM, 6 = 8-bit EEPROM).
      |
      +----- X = Number of pots (1 or 2, quad-pot chips are not currenctly supported).
```

Note that the MCP41X1 chips multiplex the SDI and SDO on a single pin.
To use these chips with a standard SPI interface as on the Arduino you will need to
connect the SDI/SDO pin on the pot to the Arduino's MISO pin, then bridge the MISO pin to the MOSI pin
with a resistor (3k9 resistor seems to work well).

This class has only been tested with the MCP4151.
