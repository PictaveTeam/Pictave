#ifndef UART_T_HPP
#define UART_T_HPP

#include "../core.h"

enum UartBufferState{
    UART_BUFFER_FULL,
    UART_BUFFER_EMPTY,
    UART_BUFFER_OK
};

struct Uart_t{
    byte_t* rxBuffer;
    byte_t* txBuffer;
    const uint TX_BUFFER_SIZE;
    const uint RX_BUFFER_SIZE;
    
    uint receiveCursor;  // Indique la case dans laquelle la prochaine donnée sera écrite
    uint readCursor;     //
    
    uint transmitCursor;
    uint writeCursor;
    
    enum UartBufferState rxBufferStatus;
    
    float(*start_fnc)(uint);
    void(*stop_fnc)(void);
    void(*write_fnc)(void);
    void(*onRxInterrupt)(struct Uart_t*, byte_t);
};

#endif //!UART_T_HPP