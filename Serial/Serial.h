#ifndef SERIAL_H
#define	SERIAL_H

#include <xc.h>
#include "../core.h"

#if SERIAL_BUFFER_SIZE > 255
    #define SERIAL_SIZE_TYPE uint
#else
    #define SERIAL_SIZE_TYPE uchar
#endif

struct Uart_t;
typedef struct Uart_t* Uart;

enum Serial_StopInfo{
    Serial_1Stop,
    Serial_2Stop
};

enum Serial_DataParityInfo{
    Serial_8BitsNoParity,
    Serial_8BitsOddParity,
    Serial_8BitsEvenParity,
};

//------------------------------------------------------------------------------
//-------------------------------[UART1]----------------------------------------
//------------------------------------------------------------------------------
#if SERIAL_COUNT >= 1 && USE_UART1 != 0
    #define SERIAL1_ENABLED 1
#else
    #define SERIAL1_ENABLED 0
#endif

#if SERIAL_COUNT >= 2 && USE_UART2 != 0
    #define SERIAL2_ENABLED 1
#else
    #define SERIAL2_ENABLED 0
#endif

#if SERIAL1_ENABLED == 1
    float Serial_Init(float baudrate);
    /*
     * Retourne le premier caract�re disponible
     * Retourne -1 si aucun caract�re n'est disponible
     */
    int Serial_Peek(void);
    int Serial_Read(void); 
    SERIAL_SIZE_TYPE Serial_Available(void); // Return the number of characters waiting to be read by the user

    void Serial_WriteByte(const uchar data);
    void Serial_Write(const uchar* data, int len);
    void Serial_WriteString(const char* str);
    SERIAL_SIZE_TYPE Serial_WriteAvailable(void); // Return the number of character waiting to be written to the serial port
    void Serial_Flush(void); // Attends jusqu'� ce que les donn�es 

    // A mettre dans l'interuption g�n�rale si une seule routine ISR est disponible
    // Incr�mente le compteur de r�ception et Ajoute les caract�res re�us au RxBuffer
    #ifdef SINGLE_ISR
        void Serial_ISR(void); // Device specific
    #endif
#endif // SERIAL_COUNT == 1
    
 
#if SERIAL2_ENABLED // Code uniquement pour les microcontrolleurs avec 2 UART ou plus    
    
    #if SERIAL2_BUFFER_SIZE > 255
        #define SERIAL2_SIZE_TYPE unsigned int
    #else
        #define SERIAL2_SIZE_TYPE uchar
    #endif

        
    float Serial2_Init(float baudrate);
    /*
     * Retourne le premier caract�re disponible
     * Retourne -1 si aucun caract�re n'est disponible
     */
    int Serial2_Peek(void);
    int Serial2_Read(void); 
    SERIAL2_SIZE_TYPE Serial2_Available(void); // Return the number of characters waiting to be read by the user

    void Serial2_WriteByte(const uchar data);
    void Serial2_Write(const uchar* data, int len);
    void Serial2_WriteString(const char* str);
    SERIAL2_SIZE_TYPE Serial2_WriteAvailable(void); // Return the number of character waiting to be written to the serial port
    void Serial2_Flush(void); // Attends jusqu'� ce que les donn�es 

    // A mettre dans l'interuption g�n�rale si une seule routine ISR est disponible
    // Incr�mente le compteur de r�ception et Ajoute les caract�res re�us au RxBuffer
    #ifdef SINGLE_ISR
        void Serial2_ISR(void); // Device specific
    #endif
#endif // SERIAL_COUNT == 2

#endif	/* SERSerialAvailableI
SERIAL_SIZE_TYPE SerialAvailable(void); // AL_H */
