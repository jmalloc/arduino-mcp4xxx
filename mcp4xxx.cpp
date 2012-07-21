#include "mcp4xxx.h"

namespace icecave {
namespace arduino {

MCP4XXX::MCP4XXX(byte select_pin, Pot pot, MemorySize memory_size, WiperConfiguration config)
  : m_select_pin(select_pin)
  , m_pot_address(static_cast<Address>(pot))
  , m_max_value(memory_size + config) // Potentiometer configurations allow "memory_size + 1" values, for setting the "full-scale" wiper position.
{
  SPI.begin();
  pinMode(m_select_pin, OUTPUT);
  deselect();
}


word MCP4XXX::max_value(void) const
{
  return m_max_value;
}

bool MCP4XXX::increment(void)
{
  return transfer(m_pot_address, command_increment);
}

bool MCP4XXX::decrement(void)
{
  return transfer(m_pot_address, command_decrement);
}

bool MCP4XXX::set(word value)
{
  return transfer(m_pot_address, command_write, min(value, m_max_value));
}

bool MCP4XXX::get(word& value) const
{
  return transfer(m_pot_address, command_read, data_mask_word, value);
}

word MCP4XXX::get(void) const
{
  word result = 0xFFFF;
  get(result);
  return result;
}

void MCP4XXX::select(void) const
{
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  digitalWrite(m_select_pin, LOW);
}

void MCP4XXX::deselect(void) const
{
  digitalWrite(m_select_pin, HIGH);
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

} // end namespace arduino
} // end namespace icecave

