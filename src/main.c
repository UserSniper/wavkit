#include "include.h"



// example program for WAVKIT V1
int main(){
    cbm_k_clall();

    printf("yo yo yo here's something funny for ya\nthis thing is a music player!");


    cbm_k_setnam("CD:ASSETS");
    cbm_k_setlfs(15,8,15);
    cbm_k_open();
    cbm_k_setnam("U0>B\x01");
    cbm_k_open();


    wavkit_init_engine();

    wavkit_setvol(12);
    wavkit_setrate(PCM_8000HZ,0,0);
    wavkit_setfile("LMAO.WAV");
    wavkit_setloop(1);
    wavkit_play();


    while(1){
        waitvsync();
        poll_controller();
        VERA.display.border = 1;
        wavkit_tick();
        VERA.display.border = 0;

        if(pad_a_new){wavkit_restart();}
        if(pad_b_new){wavkit_stop();}
        if(pad_x_new){wavkit_play();}
    }
}