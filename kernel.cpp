// Kernel.cpp (w)

// Some assembly stuff
extern "C" void kernel_main();

// This gets the hardware to talk
static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    asm volatile ( "inb %1, %0" : "=a"(val) : "Nd"(port) );
    return val;
}

// String utilities
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void outb(unsigned short port, unsigned char val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}


class VGADriver {
public:
    int x, y;
    volatile char* vm = (volatile char*)0xB8000;
    const char color = 0x0F; // if yall want change colour from here

    VGADriver() : x(0), y(0) {}

    void clear() {
        for (int i = 0; i < 80 * 25 * 2; i += 2) {
            vm[i] = ' ';
            vm[i+1] = color;
        }
    }

    void put_char(char c) {
        if (c == '\n') {
            x = 0; y++;
        } else if (c == '\b') {
            if (x > 2) {
                x--;
                int index = (y * 80 + x) * 2;
                vm[index] = ' ';
            }
        } else {
            int index = (y * 80 + x) * 2;
            vm[index] = c;
            vm[index+1] = 0x0F;
            x++;
        }
        if (x >= 80) { x = 0; y++; }
    }

    void print(const char* str) {
        for (int i = 0; str[i] != '\0'; i++) put_char(str[i]);
    }
};

class Shell {
    char buffer[256];
    int index = 0;
    VGADriver* vga;

public:
    Shell(VGADriver* v) : vga(v) {
        for(int i=0; i<256; i++) buffer[i] = 0;
    }

    void prompt() {
        vga->print("\nsnowOS> ");
        index = 0;
        for(int i=0; i<256; i++) buffer[i] = 0;
    }

    void handle_char(char c) {
        if (c == '\n') {
            execute_command();
            prompt();
        } else if (c == '\b') {
            if (index > 0) {
                index--;
                buffer[index] = 0;
                vga->put_char('\b');
            }
        } else {
            if (index < 255) {
                buffer[index++] = c;
                vga->put_char(c);
            }
        }
    }

    void execute_command() {
        // We got the whole goon commands up in here
        vga->print("\n");
        if (strcmp(buffer, "help") == 0) {
            vga->print("Commands: help, version, clear, halt");
        } else if (strcmp(buffer, "version") == 0) {
            vga->print("snowOS v0.0.2 (Goon Edition)");
        } else if (strcmp(buffer, "clear") == 0) {
            vga->clear();
        } else if (strcmp(buffer, "halt") == 0) {
            vga->print("System Halted.");
            asm volatile("hlt");
        } else if(strcmp(buffer, "osama") == 0){
            vga->print("Ay how do you know this?, anyways stop gooning");
        } else if (strcmp(buffer, "charlie-kirk") == 0){
            vga->print("WE ARE CHARLIE KIRK..");
        } else if (index > 0) {
            vga->print("Unknown command: ");
            vga->print(buffer);
        }
    }
};

class Keyboard{
public:

    bool data_ready() {
        return inb(0x64) & 0x01; // Port 0x64 is the Status Register
    }

    // gets the scan-code
        unsigned char get_scancode() {
        return inb(0x60); // btw gng the 0x60 is port number in data register.
    }

    // ok the code might get bigger cuz of this scancode to keyboard table
    char scancode_to_char(unsigned char scancode) {
        switch(scancode) {
            case 0x1E: return 'a'; case 0x30: return 'b'; case 0x2E: return 'c';
            case 0x20: return 'd'; case 0x12: return 'e'; case 0x21: return 'f';
            case 0x22: return 'g'; case 0x23: return 'h'; case 0x17: return 'i';
            case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l';
            case 0x32: return 'm'; case 0x31: return 'n'; case 0x18: return 'o';
            case 0x19: return 'p'; case 0x10: return 'q'; case 0x13: return 'r';
            case 0x1F: return 's'; case 0x14: return 't'; case 0x16: return 'u';
            case 0x2F: return 'v'; case 0x11: return 'w'; case 0x2D: return 'x';
            case 0x15: return 'y'; case 0x2C: return 'z'; case 0x0C: return '-';
            case 0x39: return ' '; case 0x1C: return '\n'; case 0x0E: return '\b';
            default: return 0;
        }
    }
private:
};

// Main function
void kernel_main() {
    
    // objects
    VGADriver vga;
    Keyboard KeyBoard;
    Shell shell(&vga);
    
    vga.clear();
    vga.print("SnowOS is now running\n");
    vga.print("Type something, let's see if it goons on you or not?\n");
    shell.prompt();

    // Keyboard typing
    while(1) {
        if (KeyBoard.data_ready()) {
            unsigned char scancode = KeyBoard.get_scancode();
            if (scancode < 0x80) {
                char c = KeyBoard.scancode_to_char(scancode);
                if (c != 0) shell.handle_char(c);
            }
        }
    }
}


// 3AM kernel