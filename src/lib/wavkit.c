#include <cx16.h>
#include <cbm.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define sb4(a) *((unsigned char*)&a+3)
#define sb3(a) *((unsigned char*)&a+2)
#define msb(a) *((unsigned char*)&a+1)
#define lsb(a) *((unsigned char*)&a)

#define byte(a) ((unsigned char*)a)
#define short(a) ((unsigned short*)a)


#define PCM_PROPERTIES (VERA.audio.control & 0x30) // stereo/16bit

#define PCM_STEREO (VERA.audio.control & 0x10)
#define PCM_16BIT (VERA.audio.control & 0x20)
#define PCM_FIFO_EMPTY (VERA.audio.control & 0x40)
#define PCM_FIFO_FULL (VERA.audio.control & 0x80)




typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

struct wav_header {
    char ChunkID[4];                // "RIFF"
    unsigned long ChunkSize;        // file size (32-bit integer)
    char Format[4];                 // "WAVE"
    char Subchunk1ID[4];            // "fmt "
    unsigned long Subchunk1Size;    // length of format data
    unsigned short AudioFormat;     // type of format; 1 is PCM
    unsigned short NumChannels;     // channel count
    unsigned long SampleRate;       // sample rate
    unsigned long ByteRate;         // (sample rate * bits per sample * channels) / 8
    unsigned short BlockAlign;      // (bits per sample * Channels) / 8
    unsigned short BitsPerSample;   // bits per sample
    char Subchunk2ID[4];            // "data"
    unsigned long Subchunk2Size;    // length of audio data
};

u8 wavkit_global_atten = 15;
//u32 wavkit_tmp, wavkit_tmp2;
u16 wavkit_nextValueL, wavkit_nextValueR;

struct wavkit_ch {
    //u8 atten;           // (SAMPLE >> atten)
    u8 rate;
    bool stereo;
    bool sixteenbit;
    u32 pos;            // position in file - 44 bytes
    bool loop;          // loop the channel? (true = yes)
    bool playing;
    
    char filename[64];

    u8 buffer[256];

    u16 nextValueL;
    u16 nextValueR;

    //struct wav_header header[WAVKIT_CHANNEL_CNT]; // header for the loaded .wav
} wavkit_ch;

// sets default values. call wavkit_setrate() after this.
void wavkit_init_engine(){
    wavkit_global_atten = 0;
    wavkit_ch.rate = 0;
    wavkit_ch.stereo = 0;
    wavkit_ch.sixteenbit = 0;
    wavkit_ch.loop = 0;
    wavkit_ch.playing = 0;
}


void wavkit_setvol(const u8 volume){
    // cap the volume
    if(volume > 15) wavkit_global_atten = 15;
    else wavkit_global_atten = volume;

    // write in the volume without changing stereo/16bit mode
    unsigned char tmp = (VERA.audio.control & 0x30);
    VERA.audio.control = (tmp + wavkit_global_atten);
}


void wavkit_setrate(const u8 samplerate, const bool stereo, const bool bitdepth){
    wavkit_ch.stereo = stereo;
    wavkit_ch.sixteenbit = bitdepth;

    VERA.audio.control = ((bitdepth<<5)|(stereo<<4)|(wavkit_global_atten));
    wavkit_ch.rate = (samplerate & 0x7f); // no 128, sorry
    VERA.audio.rate = 0;
}

// opens the wav file and extracts the header.
void wavkit_setfile(const char * file){
    strcpy(wavkit_ch.filename, file);

    cbm_k_setnam(wavkit_ch.filename);
    cbm_k_setlfs(10, 8, 2);
    cbm_k_open();

    //cbm_read(10, &wavkit_ch.buffer, 44);
    cx16_k_macptr(44,0,&wavkit_ch.buffer[0]);
    //for(u8 i = 0; i < 44; i++){
    //    cbm_k_acptr();
    //}
    
}


void wavkit_setloop(const bool loop){
    wavkit_ch.loop = loop;
}


void wavkit_fetchnext(){
    if(lsb(wavkit_ch.pos) == 0){
        cx16_k_macptr(255,0,&wavkit_ch.buffer);
        wavkit_ch.buffer[255] = cbm_k_acptr();
        
        
        if ((cbm_k_readst() & 0b01000000)){ // end of file
            cx16_k_memory_fill(wavkit_ch.buffer,256,0x80);
            VERA.audio.rate = 0;
            wavkit_ch.playing = 0;
            VERA.audio.control = 0b10000000;
            cbm_k_close(10);
            if((wavkit_ch.loop && (wavkit_ch.pos != 0)) || (wavkit_ch.pos == 0)){ // && (!)
                wavkit_setfile(wavkit_ch.filename);
                wavkit_setrate(wavkit_ch.rate, wavkit_ch.stereo, wavkit_ch.sixteenbit);
                VERA.audio.rate = wavkit_ch.rate;
                wavkit_ch.playing = 1;
            }
        }
        
    }

    
    //switch(PCM_PROPERTIES){
    //    default: // 8bit mono
            lsb(wavkit_nextValueL) = wavkit_ch.buffer[lsb(wavkit_ch.pos)];
            wavkit_ch.pos++;
            
    //        break;
        
    //    case 0x10: // 16bit mono
    //        wavkit_nextValueL = wavkit_ch.buffer[lsb(wavkit_ch.pos)];
    //        wavkit_ch.pos+=2;
    //        
    //        break;

        /*
        case 0x20: // 8bit stereo
            lsb(wavkit_ch.nextValueL[i]) = cbm_k_acptr();
            //msb(wavkit_ch.nextValueL[i]) = 0;
            lsb(wavkit_ch.nextValueR[i]) = cbm_k_acptr();
            //msb(wavkit_ch.nextValueR[i]) = 0;

            wavkit_tmp += lsb(wavkit_ch.nextValueL[i]);
            wavkit_tmp2 += lsb(wavkit_ch.nextValueR[i]);
            break;

        case 0x30: // 16bit stereo
            cx16_k_macptr(2,0,&wavkit_ch.nextValueL[i]);
            //lsb(wavkit_ch.nextValueL[i]) = cbm_k_acptr();
            //msb(wavkit_ch.nextValueL[i]) = cbm_k_acptr();
            //lsb(wavkit_ch.nextValueR[i]) = cbm_k_acptr();
            //msb(wavkit_ch.nextValueR[i]) = cbm_k_acptr();

            wavkit_tmp += wavkit_ch.nextValueL[i];
            wavkit_tmp2 += wavkit_ch.nextValueR[i];
            break;
        */
    //}
    
}


void wavkit_writenext(){
    //switch(PCM_PROPERTIES){
        //default:
            VERA.audio.data = lsb(wavkit_nextValueL);
        //    break;
        //case 0x10: // 16bit mono
        //    VERA.audio.data = lsb(wavkit_nextValueL);
        //    VERA.audio.data = msb(wavkit_nextValueL);
        //    break;
    //}
}


void wavkit_tick(){
    if(wavkit_ch.playing == 0) return;

    cbm_k_chkin(10);
    cbm_k_readst();
    //if ((cbm_k_readst() & 0b01000000)) { 
    //    return; // end of file
    //} 
    while(!PCM_FIFO_FULL){
        wavkit_fetchnext();
        wavkit_writenext();
    }   
}


void wavkit_play(){
    VERA.audio.rate = wavkit_ch.rate;
    wavkit_ch.playing = 1;
}


void wavkit_stop(){
    VERA.audio.rate = 0;
    wavkit_ch.playing = 0;
}


void wavkit_restart(){
    VERA.audio.rate = 0;
    wavkit_ch.playing = 0;
    wavkit_ch.pos = 0;
    //VERA.audio.control = 0b10000000;
    cbm_k_close(10);
    wavkit_setfile(wavkit_ch.filename);
    wavkit_setrate(wavkit_ch.rate, wavkit_ch.stereo, wavkit_ch.sixteenbit);
}