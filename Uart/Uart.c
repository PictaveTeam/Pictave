
#define INCLUDE_UART_DEFINITION
#include "Uart.h"

#define GET_CURSOR_INCREMENT(cursor, maxValue) (((cursor)+1)%(maxValue))

#define RETURN_IF_EMPTY(queue, retVal)      \
    if(ByteQueue_Size(queue) == 0){         \
        return (retVal);                    \
    }

/*static void Uart_OnRxInterrupt(Uart uart, byte_t rxValue);
static void Uart_OnTransmitRequest(Uart uart);
static void Uart_AddToTransmitQueue(Uart, byte_t value);*/


/******************************************************************************/
float Uart_Start(Uart uart, float baudrate){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(baudrate > 0.0f);

    Uart_Stop(uart);
     
   /*-------------------------------------*
    * Démarrage de l'UART                 * 
    *-------------------------------------*/
    return uart->pfn_EnableUart(baudrate);
}

/******************************************************************************/
void Uart_Stop(Uart uart){
    __conditional_software_breakpoint(uart != NULL);
    
    uart->pfn_DisableUart();
}

/******************************************************************************/
bool Uart_Peek(Uart uart, byte_t* pVal){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pVal != NULL);
    
    RETURN_IF_EMPTY(uart->m_ReceiveQueue, false);
    
    *pVal = ByteQueue_Get(uart->m_ReceiveQueue);
    return true;
}

/******************************************************************************/
bool Uart_ReadByte(Uart uart, byte_t* pVal){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pVal != NULL);

    RETURN_IF_EMPTY(uart->m_ReceiveQueue, false);
    
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
    
    *pVal = ByteQueue_Get(uart->m_ReceiveQueue);
    ByteQueue_Pop(uart->m_ReceiveQueue);

    return true;
}

/******************************************************************************/
uint Uart_Read(Uart uart, byte_t* pBuffer, uint size){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(pBuffer != NULL);

    // Attendre les données
 
    
    while(ByteQueue_Size(uart->m_ReceiveQueue) < size);
    
    for(int i=0; i < size; i++){
        pBuffer[i] = ByteQueue_Get(uart->m_ReceiveQueue);
        ByteQueue_Pop(uart->m_ReceiveQueue);
    }
    
    return size;
}

/******************************************************************************/
uint Uart_Available(Uart uart){
    __conditional_software_breakpoint(uart != NULL);
    
    return ByteQueue_Size(uart->m_ReceiveQueue);
}


void Uart_WriteByte(Uart uart, byte_t data){
    __conditional_software_breakpoint(uart != NULL);
   
    // Attendre qu'une place se libère 
    while(ByteQueue_Size(uart->m_TransmitQueue) == ByteQueue_Capacity(uart->m_TransmitQueue));
    ByteQueue_Push(uart->m_TransmitQueue, data);
    uart->pfn_OnTransmitQueuePush();
    
}

void Uart_Write(Uart uart, const byte_t* data, uint len){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(data != NULL);

    for(int i=0; i < len; i++){
        Uart_WriteByte(uart, data[i]);
    }
}

void Uart_WriteString(Uart uart, const char* str){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(str != NULL);
    
    int i=0;
    while(str[i] != '\0'){
        Uart_WriteByte(uart, str[i]);
        i++;
    }
}

/**
 * \fn Uart_Flush(const Uart uart)
 * \brief Bloque jusquà ce que tous les envois soient complétés
 * \param uart Uart à utiliser
 * 
 * Cette fonction est bloquante jusqu'a ce que tous les envois soient complétés.
 */
void Uart_Flush(const Uart uart){
    while(ByteQueue_Size(uart->m_TransmitQueue) > 0);
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
  

// Si appellée par software -> vérifier qu'aucune transmission est en cours
// Sinon doit être appelée lors d'une interruption sur TX

 /*static void Uart_OnTransmitRequest(Uart uart){
    __conditional_software_breakpoint(uart != NULL);
    __conditional_software_breakpoint(uart->transmitQueueSize != 0);*/
    /*
     * Impossible d'avoir une interruption ici car 
     * Cas 1) On est déjà dans l'interruption TX
     * Cas 2) On a vérifié qu'aucune transmission n'est en cours
     */
   /* byte_t value = uart->txBuffer[uart->transmitCursor];
  
    uart->transmitCursor = GET_CURSOR_INCREMENT(uart->transmitCursor, uart->TX_BUFFER_SIZE);
    uart->transmitQueueSize--;
    uart->write_fnc(value);
    // Je viens d'écrire, potentiellement une interruption ici
 }*/
 
 
//static void Uart_OnRxInterrupt(Uart uart, byte_t rxValue){
    // Si l'uart est plein, on ne lit pas
    /*(uart->readBytesAvailable == uart->RX_BUFFER_SIZE){
        return;
    }*/
    
    // On est dans l'interruption RX, impossible d'avoir de la concurrence avec la réception d'un autre caractère
    // Le code de cette fonction est donc thread safe
   /* uart->rxBuffer[uart->receiveCursor] = rxValue;
    uart->receiveCursor = GET_CURSOR_INCREMENT(uart->receiveCursor, uart->RX_BUFFER_SIZE);
    uart->readBytesAvailable++;
}*/
    
//------------------------------------------------------------------------------
//----------------------[DEVICE SPECIFIC CODE]----------------------------------
//------------------------------------------------------------------------------
    
#if defined DEVICE_PIC18F2X_4XK22_FAMILY
    #error [Uart Library] PIC18F2X_4XK22_FAMILY not supported
    //#include "devices/pic18f/pic18f2x_4xk22_serial.c"
#elif defined DEVICE_PIC24FJ128GA010_FAMILY
    #include "./hardware/pic24f/pic24fj128ga010_family_serial.c"
#else
    #error [Serial Library] Device not supported
#endif