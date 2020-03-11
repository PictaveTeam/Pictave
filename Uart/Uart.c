#include "Uart.h"
#include "Uart_t.h"

#define GET_CURSOR_INCREMENT(cursor, maxValue) (((cursor)+1)%(maxValue))
    

static void Uart_OnRxInterrupt(Uart uart, byte_t rxValue);

float Uart_Start(Uart uart, float baudrate){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(baudrate > 0.0f);

    uart->onRxInterrupt = Uart_OnRxInterrupt;
    Uart_Stop(uart);
    
   /*-------------------------------------*
    * Configuration de la structure       * 
    *-------------------------------------*/
    uart->readCursor = 0;
    uart->receiveCursor = 0;
    uart->transmitCursor = 0;
    uart->writeCursor = 0;
    uart->readBytesAvailable = 0;
    
//    uart->rxBufferStatus = UART_BUFFER_EMPTY;
    
   /*-------------------------------------*
    * Démarrage de l'UART                 * 
    *-------------------------------------*/
    return uart->start_fnc(baudrate);
}

/******************************************************************************/
void Uart_Stop(Uart uart){
    __conditional_software_breakpoint(uart != NULL);
    
    uart->stop_fnc();
}

/******************************************************************************/
bool Uart_Peek(Uart uart, byte_t* pVal){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pVal != NULL);
    
    if(uart->readBytesAvailable == 0){
        return false;
    }
    
    *pVal = uart->rxBuffer[uart->readCursor];
    return true;
}

/******************************************************************************/
bool Uart_ReadByte(Uart uart, byte_t* pVal){
     __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pVal != NULL);

    if(uart->readBytesAvailable == 0){
        return false;
    }
    
   /* Ici on peut avoir une interruption sur rx.
    * Si on incrémente readCursor avant l'interruption, mais 
    * qu'on ne lit pas la valeur avant l'interrution ie
    * 
    * uart->readCursor++;
    * -- Interruption ici 
    * *pVal = uart->rxBuffer[uart->readCursor-1];
    *
    * Alors il est possible de perdre la valeur qu'on voulait lire si 
    * l'interruption vient de remplir le buffer.
    * 
    * Conclusion :
    * Il faut d'abbord lire la value, et incrémenter le curseur ensuite
    * 
    */
    
    *pVal = uart->rxBuffer[uart->readCursor];
    uart->readCursor = GET_CURSOR_INCREMENT(uart->readCursor, uart->RX_BUFFER_SIZE);
    uart->readBytesAvailable--;

    return true;
}

/******************************************************************************/
uint Uart_Read(Uart uart, byte_t* pBuffer, uint size){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pBuffer != NULL);

    // Attendre les données
    while(uart->readBytesAvailable < size);
    /*if(uart->readBytesAvailable < size){
        return false;
    }*/
    
    for(int i=0; i < size; i++){
        pBuffer[i] =  uart->rxBuffer[uart->readCursor];
        uart->readCursor = GET_CURSOR_INCREMENT(uart->readCursor, uart->RX_BUFFER_SIZE);
        uart->readBytesAvailable--;
    }
    
    return true;
}

/******************************************************************************/
uint Uart_Available(Uart uart){
    __conditional_software_breakpoint(uart != NULL);
    
    return uart->readBytesAvailable;
}

void Uart_WriteByte(Uart uart, byte_t data){
    
}

void Uart_Write(Uart uart, const byte_t* data, uint len){
    
}

/**
 * \fn Uart_Flush(const Uart uart)
 * \brief Bloque jusquà ce que tous les envois soient complétés
 * \param uart Uart à utiliser
 * 
 * Cette fonction est bloquante jusqu'a ce que tous les envois soient complétés.
 */
void Uart_Flush(const Uart uart)
{
    /*readCursor1 = 0;
        rxCursor1 = 0;
        return Serial_DeviceInit(baudrate);*/
} 

    int Serial_Read(void)
    {
       /* if(!Serial_Available()){
            return -1;
        }
        incrementCursor(&readCursor1);
        return rxBuffer1[readCursor1];
    }
    SERIAL_SIZE_TYPE Serial_Available(void){
        return (SERIAL_SIZE_TYPE) (rxCursor1 - readCursor1);*/
        return -1;
    }
    void Serial_WriteByte(const uchar data){
     /*   incrementCursor(&writeCursor1); // Incr�mentation du curseur d'�criture
        txBuffer1[writeCursor1] = data;  // Ajout de la donn�e au buffer
        Serial_DeviceEnableTXInterrupt();                    // Envoi des donn�es */
    }
    void Serial_Write(const uchar* data, int len){
      /*  int i;
        for(i = 0; i < len; i++){
            incrementCursor(&writeCursor1); // Incr�mentation du curseur d'�criture
            txBuffer1[writeCursor1] = data[i];  // Ajout de la donn�e au buffer
        }
        Serial_DeviceEnableTXInterrupt();*/
    }

    /*SERIAL_SIZE_TYPE Serial_WriteAvailable(void){ 
        return (SERIAL_SIZE_TYPE) (writeCursor1 - txCursor1); 
    }*/
    /*void Serial_Flush(void){
        while(Serial_WriteAvailable());
    }*/

    /*static void incrementCursor(SERIAL_SIZE_TYPE* cursor){
        (*cursor)++;
        if((*cursor) == SERIAL_BUFFER_SIZE){
            (*cursor) = 0;
        }
    }*/
     
static void Uart_OnRxInterrupt(Uart uart, byte_t rxValue){
    // Si l'uart est plein, on ne lit pas
    if(uart->readBytesAvailable == uart->RX_BUFFER_SIZE){
        return;
    }
    
    // On est dans l'interruption RX, impossible d'avoir de la concurrence avec la réception d'un autre caractère
    // Le code de cette fonction est donc thread safe
    uart->rxBuffer[uart->receiveCursor] = rxValue;
    uart->receiveCursor = GET_CURSOR_INCREMENT(uart->receiveCursor, uart->RX_BUFFER_SIZE);
    uart->readBytesAvailable++;
}
    
//------------------------------------------------------------------------------
//----------------------[DEVICE SPECIFIC CODE]----------------------------------
//------------------------------------------------------------------------------
    
#if defined DEVICE_PIC18F2X_4XK22_FAMILY
    #error [Uart Library] PIC18F2X_4XK22_FAMILY not supported
    //#include "devices/pic18f/pic18f2x_4xk22_serial.c"
#elif defined DEVICE_PIC24FJ128GA010_FAMILY
    #include "hardware/pic24f/pic24fj128ga010_family_serial.c"
#else
    #error [Serial Library] Device not supported
#endif