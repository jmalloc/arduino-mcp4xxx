#include "mcp4xxx.h"

namespace icecave {
namespace arduino {

MCP4XXX::MCP4XXX(byte select_pin, Pot pot, Resolution resolution, WiperConfiguration config)
  : m_select_pin(select_pin)
  , m_pot(pot)
  , m_max_value(resolution + config) // Potentiometer configurations allow "resolution + 1" values, for setting the "full-scale" wiper position.
  , m_select_nesting(0)
{
  SPI.begin();
  pinMode(m_select_pin, OUTPUT);
  digitalWrite(m_select_pin, HIGH);
}


word MCP4XXX::max_value(void) const
{
  return m_max_value;
}

bool MCP4XXX::increment(void)
{
  return transfer(static_cast<Address>(m_pot), command_increment);
}

bool MCP4XXX::decrement(void)
{
  return transfer(static_cast<Address>(m_pot), command_decrement);
}

bool MCP4XXX::set(word value)
{
  return transfer(static_cast<Address>(m_pot), command_write, min(value, m_max_value));
}

bool MCP4XXX::get(word& value) const
{
  return transfer(static_cast<Address>(m_pot), command_read, data_mask_word, value);
}

word MCP4XXX::get(void) const
{
  word result = 0xFFFF;
  get(result);
  return result;
}

bool MCP4XXX::set_terminal_a_status(bool connected)
{
  return set_tcon(tcon_term_a_mask, connected);
}

bool MCP4XXX::get_terminal_a_status(bool& connected) const
{
  return get_tcon(tcon_term_a_mask, connected);
}

bool MCP4XXX::get_terminal_a_status(void) const
{
  bool connected = false;
  return get_terminal_a_status(connected)
      && connected;
}

bool MCP4XXX::set_terminal_b_status(bool connected)
{
  return set_tcon(tcon_term_b_mask, connected);
}

bool MCP4XXX::get_terminal_b_status(bool& connected) const
{
  return get_tcon(tcon_term_b_mask, connected);
}

bool MCP4XXX::get_terminal_b_status(void) const
{
  bool connected = false;
  return get_terminal_b_status(connected)
      && connected;
}

bool MCP4XXX::set_wiper_status(bool connected)
{
  return set_tcon(tcon_wiper_mask, connected);
}

bool MCP4XXX::get_wiper_status(bool& connected) const
{
  return get_tcon(tcon_wiper_mask, connected);
}

bool MCP4XXX::get_wiper_status(void) const
{
  bool connected = false;
  return get_wiper_status(connected)
      && connected;
}

bool MCP4XXX::set_shutdown_status(bool shutdown)
{
  return set_tcon(tcon_shutdown_mask, !shutdown);
}

bool MCP4XXX::get_shutdown_status(bool& shutdown) const
{
  bool enabled;
  if (get_tcon(tcon_shutdown_mask, enabled))
  {
    shutdown = !enabled;
    return true;
  }
  return false;
}

bool MCP4XXX::get_shutdown_status(void) const
{
  bool shutdown = false;
  return get_shutdown_status(shutdown)
      && shutdown;
}

bool MCP4XXX::get_hardware_shutdown_status(bool& shutdown) const
{
  word result;
  if (transfer(address_status, command_read, data_mask_word, result))
  {
    shutdown = result & status_shutdown_mask;
    return true;
  }
  return false;
}

bool MCP4XXX::get_hardware_shutdown_status(void) const
{
  bool shutdown = false;
  return get_hardware_shutdown_status(shutdown)
      && shutdown;
}

void MCP4XXX::select(void) const
{
  if (++m_select_nesting == 1)
  {
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    digitalWrite(m_select_pin, LOW);
  }
}

void MCP4XXX::deselect(void) const
{
  if (--m_select_nesting == 0)
  {
    digitalWrite(m_select_pin, HIGH);
  }
}

byte MCP4XXX::build_command(Address address, Command command) const
{
  return ((address << 4) & address_mask)
       | ((command << 2) & command_mask)
       | cmderr_mask;
}

word MCP4XXX::build_command(Address address, Command command, word data) const
{
  return (build_command(address, command) << 8)
       | (data & data_mask_word);
}

bool MCP4XXX::transfer(Address address, Command command) const
{
  select();
  byte result = SPI.transfer(build_command(address, command));
  deselect();
  return result & cmderr_mask;
}

bool MCP4XXX::transfer(Address address, Command command, word data) const
{
  select();

  word cmd = build_command(address, command, data);
  bool valid = SPI.transfer(highByte(cmd)) & cmderr_mask;

  if (valid)
  {
    SPI.transfer(lowByte(cmd));
  }

  deselect();
  return valid;
}

bool MCP4XXX::transfer(Address address, Command command, word data, word& result) const
{
  select();

  word cmd = build_command(address, command, data);
  byte high_byte = SPI.transfer(highByte(cmd));
  bool valid = high_byte & cmderr_mask;

  if (valid)
  {
    result = ((high_byte & data_mask) << 8)
           | SPI.transfer(lowByte(cmd));
  }

  deselect();
  return valid;
}

bool MCP4XXX::set_tcon(byte value)
{
  // Note that the 9th (reserved) bit is always set to 1.
  return transfer(address_tcon, command_write, 0x100 | value);
}

bool MCP4XXX::set_tcon(byte mask, bool value)
{
  select();

  // The values for pot #1 are 4 bits higher in the TCON register.
  if (m_pot == pot_1)
    mask <<= 4;

  byte tcon;
  bool success = get_tcon(tcon);

  if (success)
  {
    if (value)
    {
      success = set_tcon(tcon | mask);
    }
    else
    {
      success = set_tcon(tcon & ~mask);
    }
  }

  deselect();
  return success;
}

bool MCP4XXX::get_tcon(byte& value) const
{
  word result;
  if (transfer(address_tcon, command_read, data_mask_word, result))
  {
    value = lowByte(result);
    return true;
  }
  return false;
}

bool MCP4XXX::get_tcon(byte mask, bool& value) const
{
  byte tcon;
  if (get_tcon(tcon))
  {
    // The values for pot #1 are 4 bits higher in the TCON register.
    if (m_pot == pot_1)
      mask <<= 4;

    value = tcon & mask;
    return true;
  }
  return false;
}

} // end namespace arduino
} // end namespace icecave
