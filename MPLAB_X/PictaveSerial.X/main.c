#include "../../Uart/Uart.h"

int main(void){
    
    Uart m_Uart = Uart1;
    
    TRISD = 0;
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    
    Uart_Start(m_Uart, 9600);

    while(true){
    }
    
    return 0;
}