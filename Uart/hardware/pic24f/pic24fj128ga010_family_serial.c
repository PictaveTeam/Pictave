/*!
\file pic18f45k22_serial.dev 
\author Alexandre Brunet
\date 20/02/2019
*
* THIS FILE MUST NOT BE ADDED TO COMPILATION 
* THIS FILE WILL BE INCLUDED BY Uart.c 
*/

#include <xc.h>

#define INCLUDE_UART_DEFINITION
#include "../../Uart.h"

#define INCLUDE_QUEUE_DEFINITION
#include "../../../Utils/ByteQueue.h"

#if UART1_ENABLED == 1

static float uart1_start(uint baudrate);
static void uart1_stop(void);
static void uart1_fillTxFifo(void);

static byte_t uart1_receiveQueueBuffer[UART1_RECEIVE_QUEUE_CAPACITY];
static byte_t uart1_transmitQueueBuffer[UART1_TRANSMIT_QUEUE_CAPACITY];

static struct ByteQueue_t uart1_receiveQueue;
static struct ByteQueue_t uart1_transmitQueue;

// Ici on met les fonctions
static struct Uart_t uart1 = {
    .pfn_EnableUart = uart1_start,
    .pfn_DisableUart = uart1_stop,
    .pfn_OnTransmitQueuePush = uart1_fillTxFifo
};

Uart Uart1 = &uart1;

/******************************************************************************/
// Initialisation des registres de l'uart1
static float uart1_start(uint baudrate){ 
   /*-----------------------------------
    * Inititialisation des structures
    *-----------------------------------*/
    ByteQueue_Init(&uart1_receiveQueue , uart1_receiveQueueBuffer , UART1_RECEIVE_QUEUE_CAPACITY);
    ByteQueue_Init(&uart1_transmitQueue, uart1_transmitQueueBuffer, UART1_TRANSMIT_QUEUE_CAPACITY);

    Uart1->m_ReceiveQueue = &uart1_receiveQueue;
    Uart1->m_TransmitQueue = &uart1_transmitQueue;    
   /*-------------------------------------*
    * Configuration du mode de RX et TX   *
    * NE PAS CONFIGURER LES PINS RX ET TX
    * ILS SONT CONFIGURÉS AUTOMATIQUEMENT AU DEMARRAGE DE L'UART
    * CHANGER LES TRIS MANUELLEMENT PROVOQUE UN RETARD 
    * D'ACTIVATION DE LA LIGNE D'ENVIRON 1 SECONDE !!!!!!
    *-------------------------------------*/   
   // TRISFbits.TRISF2 = PINMODE_INPUT; // rx1
    //TRISFbits.TRISF3 = PINMODE_OUTPUT; // tx1
  
   /*-------------------------------------*
    * Calcul du baudrate, et de l'erreur  * 
    *-------------------------------------*/

    // U1BRG = (fcy)/(16*baudrate)-1; <- erreur !! (16*baudrate) overflow, ne loge pas dans 16 bits
    float fbaud = baudrate;
    U1BRG = (FCY)/(fbaud*16.0f)-1;
    float realBaud = ((float)FCY)/((float)(16*(U1BRG+1)));
    float thBaud = (float) FCY;
    float error = (realBaud-thBaud)/thBaud;    
    
   /*---------------------------------------*
    * Config de l'uart et des interruptions * 
    *---------------------------------------*/
    U1MODEbits.STSEL = 0;       // un bit de stop 
    U1MODEbits.PDSEL = 0b00;    // 8 bits, pas de parité
    U1MODEbits.BRGH = 0;        // mode basse vitesse 
    
    // Netoyage des flags d'interruption
    IFS0bits.U1RXIF = 0; 
    IFS0bits.U1TXIF = 0;
    // Activation des interruptions
    IEC0bits.U1TXIE = 1;
    IEC0bits.U1RXIE = 1;
    
    // interruption quand toute la pile d'envoi vient d'être vidée
    U1STAbits.UTXISEL1 = 1;
    U1STAbits.UTXISEL0 = 0;
    
    /*-------------------------------------*
     * Activation de l'uart                * 
     *-------------------------------------*/
    U1MODEbits.UARTEN = 1; // Active l'UART 1
    Nop();
    U1STAbits.UTXEN = 1; // provoque un envoi, donc une interruption non voulue
  //  while(!U1STAbits.TRMT); // Attendre la fin de l'envoi
    return error;
}

/******************************************************************************/
static void uart1_stop(void){
   /*-------------------------------------*
    * Désactivation des interruptions     * 
    *-------------------------------------*/
    IEC0bits.U1TXIE = 0; // Désactive interruption TX 
    IEC0bits.U1RXIE = 0; // Désactive interruption TX 
    
   /*-------------------------------------*
    * Désactivation de l'UART             * 
    *-------------------------------------*/
    U1STAbits.UTXEN = 0; // Désactive l'envoi des données
    U1MODEbits.UARTEN = 0; // Désactive l'UART
}



static bool uart1_transmitQueueLock = false;
static void uart1_fillTxFifo(void){
    
    __conditional_software_breakpoint(false);
    
    if(uart1_transmitQueueLock == false){
        uart1_transmitQueueLock = true;
        
        // on essaie de remplir la fifo tx
        while((!U1STAbits.UTXBF) && (ByteQueue_Size(Uart1->m_TransmitQueue) > 0)){
            U1TXREG = ByteQueue_Get(Uart1->m_TransmitQueue);
            ByteQueue_Pop(Uart1->m_TransmitQueue);
        }
        
        uart1_transmitQueueLock = false;
    }
}

/******************************************************************************/
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt ( void ){
    IFS0bits.U1RXIF = 0;
	while(U1STAbits.URXDA){ // Clear the RX Fifo (FIFO is 4 bytes deep)
        ByteQueue_Push(Uart1->m_ReceiveQueue, U1RXREG);
	}
}

/******************************************************************************/
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt ( void ){
	IFS0bits.U1TXIF = 0;	
    
    // Toute la pile d'envoi est vide, et le dernier caractère vient d'être chargé dans le "transmit shift register" (sur la ligne série)
    // On remplit le FIFO TX si possible	
   uart1_fillTxFifo();
   
}

/* On ne fait rien si il y a une erreur */	
void __attribute__((interrupt, auto_psv)) _U1ErrInterrupt(void){
    IFS4bits.U1ERIF = 0;
}

#endif //UART1_ENABLE == 1