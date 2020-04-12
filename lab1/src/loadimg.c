#include "io.h"

void loadimg(){
    println("please input image now...");
    if(expect("LOADIMG|")){
        puts("IMG PREFIX VERIFIED!!");
        int size = get_int(0);
        char *load_addr = (char*)0x80000;
        println("recving ", size, " of bytes...");
        while(size--){
            *load_addr++ = getchar();
            println(size, " bytes remain...");
        }
        if(expect("|GMIDAOL"))
            puts("LOAD IMG DONE!!");
        else{ puts("IMG POSTFIX FAILED"); }
    }
    else{ puts("IMG PREFIX FAILED"); }
}
