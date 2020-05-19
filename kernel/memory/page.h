#ifndef __SYS_PAGE_H
#define __SYS_PAGE_H

#define TOTAL_PAGE_FRAME 512

void * page_alloc ( );
void page_free ( void * addr );

#endif