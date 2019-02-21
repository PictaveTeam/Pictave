#include "Serial.h"

// Si sous XC16, la directive #info n'existe pas. Prévient qu'on doit utiliser #warning à la place
#ifdef XC16
    #warning [Serial Library] #info not supported on XC16, #warning will be used instead
#endif // XC16

#if SERIAL_BUFFER_SIZE > 255
    #ifdef XC16
        #warning [SerialLibrary] Buffer Size : unsigned int
    #else
        #info [SerialLibrary] Buffer Size : unsigned int
    #endif
#else
    #ifdef XC16
        #warning [SerialLibrary] Buffer Size : unsigned char
    #else
        #info [SerialLibrary] Buffer Size : unsigned char
    #endif
#endif

#if defined FOSC_REQUEST && !defined FOSC
    #error [Serial Library] FOSC must be defined
#endif

#if defined FCY_REQUEST && !defined FCY
    #error [Serial Library] FCY must be defined
#endif

#if defined SINGLE_ISR
    #warning [Serial Library] Single ISR mode detected. Do not forget to include Serial_ISR() in your ISR
#endif

static int FloatToInt(float v);

#if SERIAL1_ENABLED
    static unsigned char rxBuffer1[SERIAL_BUFFER_SIZE]; // Tableau de caractères reçus sur RX
    static SERIAL_SIZE_TYPE readCursor1; // Case du dernier caracère lu par l'utilisateur
    static SERIAL_SIZE_TYPE rxCursor1;   // Case du dernier caractère recu sur le port série

    static unsigned char txBuffer1[SERIAL_BUFFER_SIZE]; // Tableau de caractères à envoyer sur TX
    static SERIAL_SIZE_TYPE writeCursor1; // Case du dernier caracère écrit par l'utilisateur
    static SERIAL_SIZE_TYPE txCursor1;   // Case du dernier caractère envoyé sur le port série

    static void incrementCursor(SERIAL_SIZE_TYPE* cursor);
    
    static float Serial_DeviceInit(float baudrate); 
    static void Serial_DeviceEnableTXInterrupt(void);


    float Serial_Init(float baudrate)
    {
        readCursor1 = 0;
        rxCursor1 = 0;
        return Serial_DeviceInit(baudrate);
    } 
    
    int Serial_Peek(void)
    {
         if(!Serial_Available()){
            return -1;
        }
        return rxBuffer1[readCursor1+1];
    }

    int Serial_Read(void)
    {
        if(!Serial_Available()){
            return -1;
        }
        incrementCursor(&readCursor1);
        return rxBuffer1[readCursor1];
    }
    SERIAL_SIZE_TYPE Serial_Available(void){
        return (SERIAL_SIZE_TYPE) (rxCursor1 - readCursor1);
    }
    void Serial_WriteByte(const uchar data){
        incrementCursor(&writeCursor1); // Incrémentation du curseur d'écriture
        txBuffer1[writeCursor1] = data;  // Ajout de la donnée au buffer
        Serial_DeviceEnableTXInterrupt();                    // Envoi des données 
    }
    void Serial_Write(const uchar* data, int len){
        int i;
        for(i = 0; i < len; i++){
            incrementCursor(&writeCursor1); // Incrémentation du curseur d'écriture
            txBuffer1[writeCursor1] = data[i];  // Ajout de la donnée au buffer
        }
        Serial_DeviceEnableTXInterrupt();
    }
    void Serial_WriteString(const char* str){
        int i = 0;
        while(str[i] != 0){
            incrementCursor(&writeCursor1); // Incrémentation du curseur d'écriture
            txBuffer1[writeCursor1] = str[i++];  // Ajout de la donnée au buffer
        }
        Serial_DeviceEnableTXInterrupt();                    // Envoi des données 
    }
    SERIAL_SIZE_TYPE Serial_WriteAvailable(void){ 
        return (SERIAL_SIZE_TYPE) (writeCursor1 - txCursor1); 
    }
    void Serial_Flush(void){
        while(Serial_WriteAvailable());
    }

    static void incrementCursor(SERIAL_SIZE_TYPE* cursor){
        (*cursor)++;
        if((*cursor) == SERIAL_BUFFER_SIZE){
            (*cursor) = 0;
        }
    }
#endif // SERIAL_COUNT >= 1
    
#if SERIAL2_ENABLED
    static unsigned char rxBuffer2[SERIAL2_BUFFER_SIZE]; // Tableau de caractères reçus sur RX
    static SERIAL2_SIZE_TYPE readCursor2; // Case du dernier caracère lu par l'utilisateur
    static SERIAL2_SIZE_TYPE rxCursor2;   // Case du dernier caractère recu sur le port série

    static unsigned char txBuffer2[SERIAL2_BUFFER_SIZE]; // Tableau de caractères à envoyer sur TX
    static SERIAL2_SIZE_TYPE writeCursor2; // Case du dernier caracère écrit par l'utilisateur
    static SERIAL2_SIZE_TYPE txCursor2;   // Case du dernier caractère envoyé sur le port série
    
    static void incrementCursor2(SERIAL2_SIZE_TYPE* cursor);
    static float Serial2_DeviceInit(float baudrate); 
    static void Serial2_DeviceEnableTXInterrupt(void);


    float Serial2_Init(float baudrate)
    {
        readCursor2 = 0;
        rxCursor2 = 0;
        return Serial2_DeviceInit(baudrate);
    } 
    
    int Serial2_Peek(void)
    {
         if(!Serial2_Available()){
            return -1;
        }
        return rxBuffer2[readCursor2+1];
    }
    
    int Serial2_Read(void)
    {
        if(!Serial2_Available()){
            return -1;
        }
        incrementCursor2(&readCursor2);
        return rxBuffer2[readCursor2];
    }
    
    SERIAL2_SIZE_TYPE Serial2_Available(void){
        return (SERIAL2_SIZE_TYPE) (rxCursor2 - readCursor2);
    }
    void Serial2_WriteByte(const uchar data){
        incrementCursor2(&writeCursor2); // Incrémentation du curseur d'écriture
        txBuffer2[writeCursor2] = data;  // Ajout de la donnée au buffer
        Serial2_DeviceEnableTXInterrupt();                    // Envoi des données 
    }
    void Serial2_Write(const uchar* data, int len){
        int i;
        for(i = 0; i < len; i++){
            incrementCursor2(&writeCursor2); // Incrémentation du curseur d'écriture
            txBuffer2[writeCursor2] = data[i];  // Ajout de la donnée au buffer
        }
        Serial2_DeviceEnableTXInterrupt();
    }
    void Serial2_WriteString(const char* str){
        int i = 0;
        while(str[i] != 0){
            incrementCursor2(&writeCursor2); // Incrémentation du curseur d'écriture
            txBuffer2[writeCursor2] = str[i++];  // Ajout de la donnée au buffer
        }
        Serial2_DeviceEnableTXInterrupt();                    // Envoi des données 
    }
    SERIAL2_SIZE_TYPE Serial2_WriteAvailable(void){ 
        return (SERIAL2_SIZE_TYPE) (writeCursor2 - txCursor2); 
    }
    void Serial2_Flush(void){
        while(Serial2_WriteAvailable());
    }
    
    static void incrementCursor2(SERIAL2_SIZE_TYPE* cursor){
        (*cursor)++;
        if((*cursor) == SERIAL2_BUFFER_SIZE){
            (*cursor) = 0;
        }
    }

#endif // SERIAL_COUNT >= 2
    
    
//------------------------------------------------------------------------------
//----------------------[DEVICE SPECIFIC CODE]----------------------------------
//------------------------------------------------------------------------------
    
#if defined PIC18F2X_4XK22_FAMILY
    #include "devices/pic18f/pic18f2x_4xk22_serial.c"
#elif defined PIC24FJ128GA010_FAMILY
    #include "devices/pic24f/pic24fj128ga010_family_serial.c"
#else
    #error [Serial Library] Device not supported
#endif

static int FloatToInt(float v){
    int vInt = (int) v;
    if(v - (float) vInt > 0.5){
        return vInt + 1;
    }
    return vInt;
}