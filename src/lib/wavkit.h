typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

#define PCM_48KHZ 126
#define PCM_32KHZ 84
#define PCM_16KHZ 42
#define PCM_8KHZ 21

#define PCM_44100HZ 116
#define PCM_22050HZ 58
#define PCM_11025HZ 29

void wavkit_init_engine();
void wavkit_setvol(const u8 volume);
void wavkit_setrate(const u8 samplerate, const bool stereo, const bool bitdepth);
void wavkit_setfile(const u8 prio, const char * file);
void wavkit_tick();