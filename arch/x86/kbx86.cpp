#include "kbx86.h"
#include "port.h"
#include "idt.h"
#include "video.h"
#include "mutex.hh"
#include "kb/layout.h"

#define KB_IRQ_NUMBER 33

KBX86::KBX86() : KB()
{
    IDT::get()->installRoutine(KB_IRQ_NUMBER, KBX86::handler);
}

/* FIXME General names, now mapped from finnish layout
 */
enum KEYCODE_INDEX {
    KEY_NONE = 0,
    KEY_ESC = 0x01,

    KEY_1 = 0x02,
    KEY_2 = 0x03,
    KEY_3 = 0x04,
    KEY_4 = 0x05,
    KEY_5 = 0x06,
    KEY_6 = 0x07,
    KEY_7 = 0x08,
    KEY_8 = 0x09,
    KEY_9 = 0x0A,
    KEY_0 = 0x0B,

    KEY_PLUS = 0x0C,
    KEY_QUOTE = 0x0D,

    KEY_BACKSPACE = 0x0E,
    KEY_TAB = 0x0F,

    KEY_Q = 0x10,
    KEY_W = 0x11,
    KEY_E = 0x12,
    KEY_R = 0x13,
    KEY_T = 0x14,
    KEY_Y = 0x15,
    KEY_U = 0x16,
    KEY_I = 0x17,
    KEY_O = 0x18,
    KEY_P = 0x19,

    KEY_ORING = 0x1A,
    KEY_CARET = 0x1B,

    KEY_ENTER = 0x1C,
    KEY_CTRL = 0x1D,

    KEY_A = 0x1E,
    KEY_S = 0x1F,
    KEY_D = 0x20,
    KEY_F = 0x21,
    KEY_G = 0x22,
    KEY_H = 0x23,
    KEY_J = 0x24,
    KEY_K = 0x25,
    KEY_L = 0x26,

    KEY_OE = 0x27,
    KEY_AE = 0x28,

    KEY_HALF = 0x29,

    KEY_LSHIFT = 0x2A,

    KEY_Z = 0x2C,
    KEY_X = 0x2D,
    KEY_C = 0x2E,
    KEY_V = 0x2F,
    KEY_B = 0x30,
    KEY_N = 0x31,
    KEY_M = 0x32,

    KEY_COMMA = 0x33,
    KEY_DOT = 0x34,
    KEY_DASH = 0x35,
    KEY_RSHIFT = 0x36,
    KEY_MUL = 0x37,
    KEY_ALT = 0x38,

    KEY_SPACE = 0x39,
    KEY_CAPS_LOCK = 0x3A,
    KEY_F1 = 0x3B,
    KEY_F2 = 0x3C,
    KEY_F3 = 0x3D,
    KEY_F4 = 0x3E,
    KEY_F5 = 0x3F,
    KEY_F6 = 0x40,
    KEY_F7 = 0x41,
    KEY_F8 = 0x42,
    KEY_F9 = 0x43,
    KEY_F10 = 0x44,

    KEY_PAD_7 = 0x47,
    KEY_PAD_8 = 0x48,
    KEY_PAD_9 = 0x49,

    KEY_PAD_MINUS = 0x4A,

    KEY_PAD_4 = 0x4B,
    KEY_PAD_5 = 0x4C,
    KEY_PAD_6 = 0x4D,

    KEY_PAD_PLUS = 0x4E,

    KEY_PAD_1 = 0x4F,
    KEY_PAD_2 = 0x50,
    KEY_PAD_3 = 0x51,

    KEY_DEL = 0x53,
    KEY_LESS_THAN = 0x56,
    KEY_F11 = 0x57,
    KEY_F12 = 0x58,

    KEY_WIN = 0x5B,
    KEY_RIGHT_MODIFIER = 0x60,
    KEY_RELEASE = 0x80
};

#define MOD_SHIFT (1 << 1)
#define MOD_CTRL  (1 << 2)
#define MOD_ALT   (1 << 3)
#define MOD_ALTGR (1 << 4)
#define MOD_RIGHT (1 << 5)

static const char **layout = nullptr;
static const char **layout_upcase = nullptr;
static unsigned char mods = 0;
static enum layout_code layout_code = LAYOUT_DEFAULT;
//static enum layout_code layout_code = LAYOUT_FI;

// Have to be power of 2
#define KEY_BUFFER_SIZE (1 << 5)
#define KEY_BUFFER_MASK (KEY_BUFFER_SIZE - 1)
static const char *keybuffer[KEY_BUFFER_SIZE] = { 0 };
static unsigned int keybuffer_write = 0;
static unsigned int keybuffer_read = 0;
static Spinlock keybuffer_read_lock;

bool KBX86::hasKey()
{
    return (keybuffer_write != keybuffer_read);
}

const char *KBX86::getKey()
{
    keybuffer_read_lock.lock();
    if (keybuffer_write == keybuffer_read) {
        keybuffer_read_lock.unlock();
        return nullptr;
    }

    const char *res = keybuffer[keybuffer_read];
    keybuffer_read = (keybuffer_read + 1) & KEY_BUFFER_MASK;

    keybuffer_read_lock.unlock();
    return res;
}

void KBX86::run(Regs *r)
{
    (void)r;
    unsigned char code = Port::in(0x60);

    (void)code;
    if (!layout) {
        /* Get default layout */
        layout = get_layout(layout_code);
        layout_upcase = get_layout_upcase(layout_code);
    }
    // TODO Post code to queue
    if ((code & KEY_RELEASE) != 0) {
        unsigned char bcode = code & ~KEY_RELEASE;

        if (bcode == KEY_LSHIFT)
            mods &= ~MOD_SHIFT;
        if (bcode == KEY_RSHIFT)
            mods &= ~MOD_SHIFT;
        if (bcode == KEY_CTRL)
            mods &= ~MOD_CTRL;
        if (bcode == KEY_ALT) {
            if (mods & MOD_RIGHT)
                mods &= ~MOD_ALTGR;
            else
                mods &= ~MOD_ALT;
        }
        if (bcode & KEY_RIGHT_MODIFIER)
            mods &= ~MOD_RIGHT;

    } else {
        unsigned char bcode = code;

        if (bcode & KEY_RIGHT_MODIFIER)
            mods |= MOD_RIGHT;

        if (bcode == KEY_LSHIFT)
            mods |= MOD_SHIFT;
        else if (bcode == KEY_RSHIFT)
            mods |= MOD_SHIFT;
        else if (bcode == KEY_CTRL)
            mods |= MOD_CTRL;
        else if (bcode == KEY_ALT) {
            if (mods & MOD_RIGHT)
                mods |= MOD_ALTGR;
            else
                mods |= MOD_ALT;
        }

        if (bcode < 0x60) {
            const char *mapped;
            if (mods & MOD_SHIFT)
                mapped = layout_upcase[bcode];
            else
                mapped = layout[bcode];
            if (mapped) {
                keybuffer_read_lock.lock();
                Video::get()->printf("%s", mapped);
                keybuffer[keybuffer_write] = mapped;
                keybuffer_write = (keybuffer_write + 1) & KEY_BUFFER_MASK;
                keybuffer_read_lock.unlock();
            }
        }

    }
}
