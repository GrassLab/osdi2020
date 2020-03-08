.section ".text" // define this section as ".text"
_start:          // label _start
  wfe            // "wait for event", let ARM core enter low power mode and wait for lock
  b _start       // jump to _start label
