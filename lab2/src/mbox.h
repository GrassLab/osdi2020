#include "gpio.h"

/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

/*channel*/
#define MBOX_CH_PROP    8

int mbox_call(unsigned char ch);

