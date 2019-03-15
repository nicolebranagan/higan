auto APU::read(uint16 address) -> uint8 {
  if(address >= 0x0000 && address <= 0x0fff) return ram.read(address);
  if(address == 0x8000) return cpu.io.apuPort;
  return 0x00;
}

auto APU::write(uint16 address, uint8 data) -> void {
  if(address >= 0x0000 && address <= 0x0fff) return ram.write(address, data);
  if(address == 0x4000) return psg.psgRight.write(data);
  if(address == 0x4001) return psg.psgLeft.write(data);
  if(address == 0x8000) return (void)(cpu.io.apuPort = data);
  if(address == 0xc000) return cpu.pin37 = 1;
}

auto APU::in(uint8 address) -> uint8 {
  return 0x00;
}

auto APU::out(uint8 address, uint8 data) -> void {
  //todo: unconfirmed
  irq.line = 0;
  cpu.pin37 = 0;
}