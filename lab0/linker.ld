/* linker script
It's used to describe how the sections in the input files should be mapped into the output file, and to control the memory layout of the output file. 
*/

SECTIONS           /* use keyword "SECTIONS" to declare SECTIONS */
{
  . = 0x80000;     /* "." is location counter, which means we move location counter to 0x80000 */
  .text :          /* define an output section ".text" */
  { 
  	*(.text)       /* "*" is a wildcard, put all ".text" input sections in all input files in this place */
  } 
}