// Kernel.cpp (w)

// Some assembly stuff
extern "C" void kernel_main();

// This gets the hardware to talk
static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    asm volatile ( "inb %1, %0" : "=a"(val) : "Nd"(port) );
    return val;
}

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
            case 0x15: return 'y'; case 0x2C: return 'z';
            case 0x39: return ' '; // Spacebar
            case 0x1C: return '\n'; // Enter key
            default: return 0; // ignore the rest of the key. goon on them
        }
    }
private:
};

class VGADriver {
public:
    int x, y;
    volatile char* vm = (volatile char*)0xB8000;
    const char color = 0xF0;

    VGADriver() : x(0), y(0) {}

    void clear() {
        for (int i = 0; i < 80 * 25 * 2; i += 2) {
            vm[i] = ' ';
            vm[i+1] = color;
        }
    }

    void print(const char* str) {
        for (int i = 0; str[i] != '\0'; i++) {
            if (str[i] == '\n') {
                x = 0;
                y++;
            } else {
                int index = (y * 80 + x) * 2;
                vm[index] = str[i];
                vm[index+1] = color;
                x++;
            }
        }
    }
};

// Main function
void kernel_main() {
    
    // objects
    VGADriver vga;
    Keyboard KeyBoard;
    
    vga.clear();
    vga.print("SnowOS is now running\n");
    vga.print("Type something, let's see if it goons on you or not?\n");

    // Keyboard typing
    while(true){
        if(KeyBoard.data_ready()){

            // for yall info: ts gets the scancode 
            unsigned char scanCode = KeyBoard.get_scancode();

            // if it fits the table does this
            if(scanCode < 0x80){
                // it changes the scancode to the character from the table above 
                char character = KeyBoard.scancode_to_char(scanCode);
                if(character != 0){
                    // saves it to an array and prints out the character
                    char str[2] = {character, '\0'};
                    vga.print(str);
                }
            }
        }

    }
}


// 3AM kernel