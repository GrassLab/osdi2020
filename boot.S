.section ".text"    // start of the text segment
_start:		    // entry point for the linker
  /* 
   * enter into low power standby mode
   * it makes the cpu enter the STANDBYWFE state, which suspends execution
   * unless an event is signaled by te SEV instruction or an interrupt happends
   * this is a better alternative than using a simple NOP operation
   * because the latter would use all the cpu ressources for the infinite loop
   *
   * source:  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15473.html
   */
  wfe
  b _start	    // branch back into _start to make the cpu idle forever
