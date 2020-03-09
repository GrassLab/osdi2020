.section ".text"  // Indicate the following code in placed in .text section
_start:
  /* wfe: wait-for-event,
   *   let cpu enter the low-power standby state,
   *   the difference with wfi is the wfe enter the low-power standby state is depended on event
   *   register if the event-register is 1, then it will clear 0, and enter the low-power standby
   *   state until the wfe wakeup events(e.g. SEV) happen.
   * pesudo code:
   *    while (!event_has_occurred) {}
   */
  wfe
  b _start  // branch to _start
