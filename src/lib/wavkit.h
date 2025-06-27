typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

#define PCM_48000HZ 126
#define PCM_32000HZ 84
#define PCM_16000HZ 42
#define PCM_8000HZ 21

#define PCM_44100HZ 116
#define PCM_22050HZ 58
#define PCM_11025HZ 29

void wavkit_init_engine();
void wavkit_setvol(const u8 volume);
void wavkit_setrate(const u8 samplerate, const bool stereo, const bool bitdepth);
void wavkit_setfile(const char * file);
void wavkit_setloop(const bool loop);
void wavkit_tick();

void wavkit_play();
void wavkit_stop();
void wavkit_restart();