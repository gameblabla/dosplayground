// Input manager
// (c) 2019 Jani Nykänen

#ifndef __INPUT__
#define __INPUT__

#include <stdint.h>
#include <dos.h>
#include <conio.h>
#include <bios.h>

#define KEY_BUFFER_SIZE 0x60

#define bool uint8_t
#define false 0
#define true 1

// Arrow key enumerations
enum {

    ArrowRight = 0,
    ArrowUp = 1,
    ArrowLeft = 2,
    ArrowDown = 3,
};

// Button states
enum {
    Up = 0,
    Down = 1,
    Pressed = 2,
    Released = 3,
};

// Input manager
typedef struct {

    // Normal keys
    uint8_t normalKeys[KEY_BUFFER_SIZE];
    // Extended keys
    uint8_t extKeys[KEY_BUFFER_SIZE];

    // Old key states (normal)
    uint8_t oldNormals[KEY_BUFFER_SIZE];
    // Old key states (extended)
    uint8_t oldExt[KEY_BUFFER_SIZE];
    // Read states (normal)
    bool normalRead[KEY_BUFFER_SIZE];
    // Read states (extended)
    bool extRead[KEY_BUFFER_SIZE];

    // Handlers
    void far interrupt (*oldHandler)(void);

} InputManager;


// Create input manager
InputManager* createInputManager();

// Destroy input manager
void destroyInputManager(InputManager* input);

// Get button
uint8_t inputGetKey(InputManager* input, uint8_t button);

// Get arrow key state
uint8_t inputGetArrow(InputManager* input, uint8_t d);

#endif // __INPUT__
