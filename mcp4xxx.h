#ifndef __ICECAVE_ARDUINO_MCP4XXX
#define __ICECAVE_ARDUINO_MCP4XXX

#include <Arduino.h>
#include <SPI.h>

namespace icecave {
namespace arduino {

/**
 * This class controls Microchip's MCP4XXX range of digital potentiometers.
 * http://ww1.microchip.com/downloads/en/DeviceDoc/22060b.pdf
 *
 * The supported chips are configured as follows:
 *
 * MCP 4 X Y Z
 *       | | |
 *       | | +- Z = Wiper Configuration (1 = Potentiometer, 2 = Rheostat).
 *       | |
 *       | +--- Y = Memory Type (3 = 7-bit RAM, 4 = 7-bit EEPROM, 5 = 8-bit RAM, 6 = 8-bit EEPROM).
 *       |
 *       +----- X = Number of pots (1 or 2).
 *
 * Note that the MCP41X1 chips multiplex the SDI and SDO on a single pin.
 * To use these chips with a standard SPI interface as on the Arduino you will need to 
 * connect the SDI/SDO pin on the pot to the Arduino's MISO pin, then bridge the MISO pin to the MOSI pin
 * with a resistor (3k9 resistor seems to work well).
 *
 * This class has only been tested with the MCP4151.
 */
class MCP4XXX
{
  public:
    enum Pot
    {
      pot_0 = B00,
      pot_1 = B01
    };
    
    enum MemorySize
    {
      memory_7bit = 127,
      memory_8bit = 255
    };
    
    enum WiperConfiguration
    {
      rheostat = 0,
      potentiometer = 1
    };

    /**
     * select_pin  - The pin to use for the SPI slave select signal, defaults to the main slave select pin on your Arduino.
     * pot         - For the 2-pot variants (MCP42XX), the potentiometer to control. Must be pot_0 for MCP41XX chips.
     * memory_size - memory_7bit for MCP4X3X and MCP4X4X, memory_8bit for MCP4X5X and MCP4X6X.
     */
    MCP4XXX(byte select_pin = SS, Pot pot = pot_0, MemorySize memory_size = memory_8bit, WiperConfiguration config = potentiometer);
    
    /**
     * Retrieve the maximum value allowed for the wiper position.
     *
     * The maximum value depends on the device's memory type and wiper configuration.
     *
     * 7-bit devices have a maximum value of 127 for rheostats and 128 for potentiometers.
     * 8-bit devices have a maximum value of 255 for rheostats and 256 for potentiometers.
     *
     * The higher value on potentiometers (MCP4XX1) facilitates direct connection of the wiper to the "A" terminal (also known as "full-scale").
     * Confusingly the rheostat devices (MCP4XX2) have only a wiper pin and "B" terminal pin, (not "A").
     */
    word max_value(void) const;

    /**
     * Increase the wiper position by 1.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool increment(void);

    /**
     * Decrease the wiper position by 1.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool decrement(void);

    /**
     * Set the wiper position.
     *
     * value - The new wiper position (must be between 0 and max_value()).
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set(word value);

    /**
     * Get the wiper position.
     *
     * value - Assigned the value of the current wiper position when retrieved successfully.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool get(word& value) const;

    /**
     * Get the wiper position.
     *
     * Returns the current wiper position, a value of 0xFFFF indicates a failure.
     */
    word get(void) const;

  private:
    const static byte address_mask       = B11110000;
    const static byte command_mask       = B00001100;
    const static byte cmderr_mask        = B00000010;
    const static byte data_mask          = B00000001;
    const static word data_mask_word     = 0x01FF;

    enum Address
    {
        address_pot0_wiper = B0000
      , address_pot1_wiper = B0001
      , address_tcon       = B0100
      , address_status     = B0101
    };
    
    enum Command
    {
        command_write      = B00
      , command_read       = B11
      , command_increment  = B01
      , command_decrement  = B10
    };

    /**
     * Select this device for SPI communication
     *
     * Configures SPI for communication with MCP devices, and sends slave-select pin LOW.
     */
    void select(void) const;

    /**
     * Cease SPI communications with this device.
     *
     * Sends the slave-select pin HIGH.
     */
    void deselect(void) const;

    /**
     * Build an 8-bit command.
     */
    byte build_command(Address address, Command command) const;

    /**
     * Build a 16-bit command.
     */
    word build_command(Address address, Command command, word data) const;

    /**
     * Transfer an 8-bit command.
     */    
    bool transfer(Address address, Command command) const;
    
    /**
     * Transfer a 16-bit command.
     *
     * Result is TRUE if address/command combination is valid; otherwise, FALSE.
     */
    bool transfer(Address address, Command command, word data) const;
    
    /**
     * Transfer a 16-bit command, and read the response.
     *
     * Result is TRUE if address/command combination is valid; otherwise, FALSE.
     *
     * The result argument is populated with the 9-bit response only if return value is TRUE.
     */
    bool transfer(Address address, Command command, word data, word& result) const;

    byte m_select_pin;
    Address m_pot_address;
    word m_max_value;
};

} // end namespace arduino
} // end namespace icecave
#endif // __ICECAVE_ARDUINO_MCP4XXX
