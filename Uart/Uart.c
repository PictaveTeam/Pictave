#include "Uart.h"

// Si sous XC16, la directive #info n'existe pas. Pr�vient qu'on doit utiliser #warning � la place

#include "Uart_t.h"

float Uart_Start(Uart uart, float baudrate){
    uart->stop_fnc();
    
   /*-------------------------------------*
    * Configuration de la structure       * 
    *-------------------------------------*/
    uart->readCursor = 0;
    uart->receiveCursor = 0;
    uart->transmitCursor = 0;
    uart->writeCursor = 0;
    
   /*-------------------------------------*
    * Démarrage de l'UART                 * 
    *-------------------------------------*/
    return uart->start_fnc(baudrate);
}

bool Uart_Read(Uart uart, byte_t* pVal){
   /* if(Uart_Available(uart)){
        
        
    }*/
    return -1;
}

uint Uart_Available(Uart uart){
    return -1;
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
    
    int Serial_Peek(void)
    {
        /* if(!Serial_Available()){
            return -1;
        }
        return rxBuffer1[readCursor1+1];*/
        return -1;
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