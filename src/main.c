#include "include.h"

int main(){

    printf("yo yo yo here's something funny for ya\nthis thing is a music player!");

    cbm_k_setnam("CD:ASSETS");
    cbm_k_setlfs(15,8,15);
    cbm_k_open();

    wavkit_init_engine();
    wavkit_setrate(PCM_8KHZ,0,0);
    wavkit_setfile(0,"TEST.WAV");

    while(1){
        waitvsync();
        poll_controller();
        VERA.display.border = 1;
        wavkit_tick();
        VERA.display.border = 0;
        //if()
    }
}