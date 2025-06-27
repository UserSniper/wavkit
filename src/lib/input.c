unsigned char tmp1, tmp2;

unsigned short pad_old; // previous frame's input
unsigned short pad;     // current frame's input
unsigned short pad_new; // just-pressed inputs (single-frame)

#define msb(a) *((unsigned char*)&a+1)
#define lsb(a) *((unsigned char*)&a)

#define pad_b       (lsb(pad) & 0x80)
#define pad_y       (lsb(pad) & 0x40)
#define pad_select  (lsb(pad) & 0x20)
#define pad_start   (lsb(pad) & 0x10)
#define pad_up      (lsb(pad) & 0x08)
#define pad_down    (lsb(pad) & 0x04)
#define pad_left    (lsb(pad) & 0x02)
#define pad_right   (lsb(pad) & 0x01)

#define pad_a       (msb(pad) & 0x80)
#define pad_x       (msb(pad) & 0x40)
#define pad_l       (msb(pad) & 0x20)
#define pad_r       (msb(pad) & 0x10)


#define pad_b_new       (lsb(pad_new) & 0x80)
#define pad_y_new       (lsb(pad_new) & 0x40)
#define pad_select_new  (lsb(pad_new) & 0x20)
#define pad_start_new   (lsb(pad_new) & 0x10)
#define pad_up_new      (lsb(pad_new) & 0x08)
#define pad_down_new    (lsb(pad_new) & 0x04)
#define pad_left_new    (lsb(pad_new) & 0x02)
#define pad_right_new   (lsb(pad_new) & 0x01)

#define pad_a_new       (msb(pad_new) & 0x80)
#define pad_x_new       (msb(pad_new) & 0x40)
#define pad_l_new       (msb(pad_new) & 0x20)
#define pad_r_new       (msb(pad_new) & 0x10)


#define xstr(s) str(s)
#define str(s) #s


struct __mouse {
    unsigned short x;
    unsigned short y;
};
#define mousePosZP (*(volatile struct __mouse *)0x02)
// see pad vars above

struct __zp mouse {
    unsigned short x;
    unsigned short y;
    unsigned char button;
      signed char scroll;

    unsigned char button_old;
    unsigned char button_new;
} mouse;

#define mouse_left (mouse.button & 0x01)
#define mouse_right (mouse.button & 0x02)
#define mouse_middle (mouse.button & 0x04)
#define mouse_button4 (mouse.button & 0x10)
#define mouse_button5 (mouse.button & 0x20)

#define mouse_left_new (mouse.button_new & 0x01)
#define mouse_right_new (mouse.button_new & 0x02)
#define mouse_middle_new (mouse.button_new & 0x04)
#define mouse_button4_new (mouse.button_new & 0x10)
#define mouse_button5_new (mouse.button_new & 0x20)

void poll_controller(){
    mouse.button_old = mouse.button;

    //mouse.button = cx16_k_mouse_get(&mousePosZP);
    __attribute__((leaf)) __asm__ volatile(
        "ldx #$02 \n"
        "jsr $ff6b \n"
        "sta $06\n"
        "stx $07\n"
    ); // call "mouse_get" kernal function*/
    mouse.button = ((unsigned char*)0x06);
    mouse.scroll = ((unsigned char*)0x07);

    mouse.x = mousePosZP.x;
    mouse.y = mousePosZP.y;
    
    //mouse = TMP;

    mouse.button_new = (mouse.button_old ^ mouse.button) & mouse.button;



    pad_old = pad;
    pad = pad_new;// this line is solely here for the linker
    
    __asm__("lda #0"); // Check joystick 0 
    __asm__("jsr $FF56");  // Call "joystick_get" Kernal function
    
    // Get the status bytes from the A and X registers
    __asm__("eor #%11111111"); // negate low byte
    __asm__("sta "xstr(pad));
    __asm__("txa");
    //__asm__("eor #%11111111"); // negate high byte
    __asm__("sta "xstr(pad)"+1");
    pad ^= 0xff00;
    

    //if(mouse_left)lsb(pad)^=0x80; // toggle B if left mouse held
    //if(mouse_left)lsb(pad)^=0x80; // toggle B if left mouse held

    // pad_old: previous frame's inputs
    // pad:     current frame's inputs
    // pad_new: new inputs (first frame pressed)
    pad_new = (pad_old ^ pad) & pad;
}


unsigned char mouse_in_window(
    const unsigned short x,
    const unsigned short y,
    const unsigned char width,
    const unsigned char height){

    if(mouse.x < x) goto nope;
    if(mouse.y < y) goto nope;
    if(mouse.x < x+width){
        if(mouse.y < y+height){
            return 1;
        }
    }

    nope: 
    return 0;
}