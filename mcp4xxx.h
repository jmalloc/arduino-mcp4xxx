#ifndef __ICECAVE_ARDUINO_MCP4XXX
#define __ICECAVE_ARDUINO_MCP4XXX

#include <Arduino.h>
#include <SPI.h>

namespace icecave {
namespace arduino {

/**
 * Controls Microchip's MCP4XXX range of digital potentiometers.
 * Data Sheet: http://ww1.microchip.com/downloads/en/DeviceDoc/22060b.pdf
 *
 * Please see README.md for more information.
 */
class MCP4XXX
{
  public:

    // Which pot to use for dual devices (MCP42XX).
    enum Pot
    {
        pot_0 = B00
      , pot_1 = B01
    };

    // The resolution of the device's resistor ladder.
    enum Resolution
    {
        res_7bit = 127
      , res_8bit = 255
    };

    // Whether the device is a rheostat (MCP4XX2), or potentiometer (MCP4XX1).
    enum WiperConfiguration
    {
        rheostat = 0
      , potentiometer = 1
    };

    /**
     * select_pin  - The pin to use for the SPI slave select signal, defaults to the main slave select pin on your Arduino.
     * pot         - For the 2-pot variants (MCP42XX), the potentiometer to control. Must be pot_0 for MCP41XX chips.
     * resolution  - res_7bit for MCP4X3X and MCP4X4X, res_8bit for MCP4X5X and MCP4X6X.
     */
    MCP4XXX(byte select_pin = SS, Pot pot = pot_0, Resolution resolution = res_8bit, WiperConfiguration config = potentiometer);

    /**
     * Retrieve the maximum value allowed for the wiper position.
     *
     * The maximum value depends on the device's resolution and wiper configuration.
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

    /**
     * Set the status of terminal "A".
     *
     * This method will always fail on rheostat (MCP4XX2) devices.
     * Use set_terminal_b_status() instead.
     *
     * connected - TRUE to connect, FALSE to disconnect.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_terminal_a_status(bool connected);

    /**
     * Get the status of terminal "A".
     *
     * This method will always fail on rheostat (MCP4XX2) devices.
     * Use get_terminal_b_status() instead.
     *
     * connected - Assigned TRUE if connected, FALSE if disconnected.
     *
     * Returns TRUE on success; otherwise FALSE.
     */
    bool get_terminal_a_status(bool& connected) const;

    /**
     * Get the status of terminal "A".
     *
     * This method will always fail on rheostat (MCP4XX2) devices.
     * Use get_terminal_b_status() instead.
     *
     * Returns TRUE if the command is successful AND the terminal is connected; otherwise FALSE.
     */
    bool get_terminal_a_status(void) const;

    /**
     * Set the status of terminal "B".
     *
     * connected - TRUE to connect, FALSE to disconnect.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_terminal_b_status(bool connected);

    /**
     * Get the status of terminal "B".
     *
     * connected - Assigned TRUE if connected, FALSE if disconnected.
     *
     * Returns TRUE on success; otherwise FALSE.
     */
    bool get_terminal_b_status(bool& connected) const;

    /**
     * Get the status of terminal "B".
     *
     * Returns TRUE if the command is successful AND the terminal is connected; otherwise FALSE.
     */
    bool get_terminal_b_status(void) const;

    /**
     * Set the status of the wiper.
     *
     * connected - TRUE to connect, FALSE to disconnect.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_wiper_status(bool connected);

    /**
     * Get the status of the wiper.
     *
     * connected - Assigned TRUE if connected, FALSE if disconnected.
     *
     * Returns TRUE on success; otherwise FALSE.
     */
    bool get_wiper_status(bool& connected) const;

    /**
     * Get the status of the wiper.
     *
     * Returns TRUE if the command is successful AND the wiper is connected; otherwise FALSE.
     */
    bool get_wiper_status(void) const;

    /**
     * Set the software shutdown status.
     *
     * shutdown - TRUE to shutdown, FALSE to enable.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_shutdown_status(bool shutdown);

    /**
     * Get the software shutdown status.
     *
     * Note that although the shutdown behaviour is overridden by the hardware SHDN pin (if present),
     * this method will still return the software setting. To get the hardware status use get_hardware_shutdown_status().
     *
     * shutdown - Assigned TRUE if shutdown, FALSE if enabled.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool get_shutdown_status(bool& shutdown) const;

    /**
     * Get the software shutdown status.
     *
     * Note that although the shutdown behaviour is overridden by the hardware SHDN pin (if present),
     * this method will still return the software setting. To get the hardware status use get_hardware_shutdown_status().
     *
     * Returns TRUE if the command is successful AND the device is in a software shutdown state.
     */
    bool get_shutdown_status(void) const;

    /**
    * Get the status of the hardware shutdown bin, if present.
     *
     * shutdown - Assigned TRUE if shutdown, FALSE if enabled.
     *
     * Returns TRUE on success; otherwise FALSE.
     */
    bool get_hardware_shutdown_status(bool& shutdown) const;

    /**
     * Get the status of the hardware shutdown bin, if present.
     *
     * Returns TRUE if the command is successful AND the device is shutdown; otherwise FALSE.
     */
    bool get_hardware_shutdown_status(void) const;

  private:
    const static byte address_mask   = B11110000;
    const static byte command_mask   = B00001100;
    const static byte cmderr_mask    = B00000010;
    const static byte data_mask      = B00000001;
    const static word data_mask_word = 0x01FF;

    const static byte tcon_shutdown_mask = B1000;
    const static byte tcon_term_a_mask   = B0100;
    const static byte tcon_term_b_mask   = B0001;
    const static byte tcon_wiper_mask    = B0010;

    const static word status_shutdown_mask = B10;

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

    /**
     * Set the value of the TCON register.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_tcon(byte value);

    /**
     * Set the value of a bit within the TCON register.
     *
     * mask - One of the TCON bit masks.
     * value - The value of the TCON bit for the given mask.
     *
     * Note that the mask is automatically shifted to the correct position based on m_pot.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool set_tcon(byte mask, bool value);

    /**
     * Get the value of the TCON register.
     *
     * value - Assigned the value of the TCON register.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool get_tcon(byte& value) const;

    /**
     * Get the value of a bit within the TCON register.
     *
     * mask - One of the TCON bit masks.
     * value - Assigned the value of the TCON bit for the given mask.
     *
     * Note that the mask is automatically shifted to the correct position based on m_pot.
     *
     * Returns TRUE on success; otherwise, FALSE.
     */
    bool get_tcon(byte mask, bool& value) const;

    byte m_select_pin;
    Pot m_pot;
    word m_max_value;
    mutable unsigned int m_select_nesting;
};

} // end namespace arduino
} // end namespace icecave
#endif // __ICECAVE_ARDUINO_MCP4XXX
