.section ".text"   // declare the name of section
_start:            // linker will entry from here
  wfe              // wait for event. If event register is set, it will be clear; If event register is not set, let kernel enter low-power standby mode.
  b _start         // branch to label "_start"
