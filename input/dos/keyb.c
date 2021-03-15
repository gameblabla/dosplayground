// Input manager
// (c) 2019 Jani Nykänen
// Modified for use in my game engine. Thank you Jani Nykänen !

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#ifdef DJGPP
#include <dpmi.h>
#include <go32.h>
static _go32_dpmi_seginfo oldInfo, newInfo;
#endif
#include "generic.h"
#include "keyb.h"

// Arrow keycodes
static const short ARROW_KEY_CODES[] = {
    77, 72, 75, 80,
};

// Reference to the current active
// input manager
static InputManager* inpRef;


// Keyboard interruption
// Source: some StackOverflow answer, I didn't
// copy the source when I found it, so I cannot
// say which one...
static void far interrupt handler() {

    static uint8_t buffer;
    uint8_t rawcode;
    uint8_t makeBreak;
    int16_t scancode;

    rawcode = inp(0x60); 
    makeBreak = !(rawcode & 0x80); 
    scancode = rawcode & 0x7F;

    if(buffer == 0xE0) { 

        if(scancode < 0x60) {

            inpRef->oldExt[scancode] = inpRef->extKeys[scancode];
            inpRef->extKeys[scancode] = makeBreak;
            
            if(inpRef->extKeys[scancode] != inpRef->oldExt[scancode])
                inpRef->extRead[scancode] = false;
        }
        buffer = 0;

    } 
    else if (buffer >= 0xE1 && buffer <= 0xE2) {

        buffer = 0;

    } 
    else if (rawcode >= 0xE0 && rawcode <= 0xE2) {

        buffer = rawcode; 

    } 
    else if (scancode < 0x60) {

        inpRef->oldNormals[scancode] = inpRef->normalKeys[scancode];
        inpRef->normalKeys[scancode] = makeBreak;

        if(inpRef->normalKeys[scancode] != inpRef->oldNormals[scancode])
            inpRef->normalRead[scancode] = false;
    }

    outp(0x20, 0x20);
}


// Get button value from an array
static char getValueFromArr(uint8_t* arr, bool* readArr, int16_t id) {

    uint8_t state = arr[id];
    bool read = readArr[id];
    uint8_t ret = 0;

    if(state == Down) {

        ret = read ? Down : Pressed;
    }
    else if(state == Up) {

        ret = read ? Up : Released;
    }
    readArr[id] = true;

    return ret;
}


// Create input manager
InputManager* createInputManager() {

    int16_t i;

    // Allocate memory
    InputManager* input = (InputManager*)malloc(sizeof(InputManager));
    if(input == NULL) {

        printf("malloc error.\n");
        exit(1);
    }
    inpRef = input;

    // Set defaults
    for(i=0; i < KEY_BUFFER_SIZE; ++ i) {

        input->oldNormals[i] = 255;
        input->oldExt[i] = 255;
        input->normalRead[i] = false;
        input->extRead[i] = false;
    }

    // Hook handlers
    #ifdef DJGPP
    _go32_dpmi_lock_code((void *) handler, 4096);

    _go32_dpmi_get_protected_mode_interrupt_vector(0x09, &oldInfo);

    newInfo.pm_offset = (int) handler;
    newInfo.pm_selector = _go32_my_cs();
    _go32_dpmi_allocate_iret_wrapper(&newInfo);

    _go32_dpmi_set_protected_mode_interrupt_vector(0x09, &newInfo);
    #else
    input->oldHandler = _dos_getvect(0x09);
    _dos_setvect(0x09, handler);
    #endif

    return input;
}


// Destroy input manager
void destroyInputManager(InputManager* input) {

    if(input == NULL) return;

    // Unhook
    #ifdef DJGPP
    _go32_dpmi_set_protected_mode_interrupt_vector(0x09, &oldInfo);
    _go32_dpmi_free_iret_wrapper(&newInfo);
    #else
    if (input->oldHandler != NULL) {

        _dos_setvect(0x09, input->oldHandler);
        input->oldHandler = NULL;
    }
    #endif

    free(input);
}


// Get button
uint8_t inputGetKey(InputManager* input, uint8_t button) {

    if(input == NULL || button > KEY_BUFFER_SIZE)
        return Up;

    // Get normal key
    return getValueFromArr(
        input->normalKeys, 
        input->normalRead, button);
}


// Get arrow key state
uint8_t inputGetArrow(InputManager* input, uint8_t d) {

    if(input == NULL || d > 4)
        return Up;

    return getValueFromArr(
            input->extKeys, 
            input->extRead, ARROW_KEY_CODES[d]);
}
