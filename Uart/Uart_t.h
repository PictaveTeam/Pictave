#ifndef UART_T_HPP
#define UART_T_HPP

#include "../core.h"

struct Uart_t{
    byte_t* rxBuffer;
    byte_t* txBuffer;
    
    uint receiveCursor;
    uint readCursor;
    
    uint transmitCursor;
    uint writeCursor;
    
    float(*start_fnc)(uint);
    void(*stop_fnc)(void);
    void(*write_fnc)(void);
    void(*read_fnc)(void);
};

#endif //!UART_T_HPP