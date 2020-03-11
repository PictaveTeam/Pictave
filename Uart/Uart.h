#ifndef PICTAVE_UART_H
#define	PICTAVE_UART_H

#include "../core.h"

struct Uart_t;
typedef struct Uart_t* Uart;

#define UART_DEFAULT_BUFFER_SIZE 64

#define UART1_ENABLE 1
//#define UART1_READ_ENABLED 1
//#define UART1_WRITE_ENABLED 1
#define UART1_RX_BUFFER_SIZE UART_DEFAULT_BUFFER_SIZE
#define UART1_TX_BUFFER_SIZE UART_DEFAULT_BUFFER_SIZE

#define UART2_ENABLE 1
#define UART2_RX_BUFFER_SIZE UART_DEFAULT_BUFFER_SIZE
#define UART2_TX_BUFFER_SIZE UART_DEFAULT_BUFFER_SIZE

#if UART1_ENABLE == 1
extern Uart Uart1;
#endif

#if UART2_ENABLE == 1
extern Uart Uart2;
#endif

/*enum Uart_Error{
    UART_NO_ERROR,
    UART_NOT_STARTED,
    UART_NO_DATA,
};*/

/**
 * \fn float Uart_Start(Uart uart, float baudrate)
 * \brief Démarre l'UART.
 * \param[in] uart Uart à initialiser
 * \param[in] baudrate Débit souhaité de la liaison série en bit/s. Le débit réel peut être différent selon les capacités matérielles.
 * \return Erreur entre le débit souhaité et le débit réel. 
 * 
 * Démarre l'UART spécifié par le paramètre uart. 
 * L'UART est configuré de la manière suivante :
 * - 1 bit de start
 * - 8 bits de données
 * - 1 bit de stop
 * - pas de parité
 * 
 */
float Uart_Start(Uart uart, float baudrate);

void Uart_Stop(Uart uart);



/**
 * \fn bool Uart_Peek(Uart uart, byte_t* pVal)
 * \brief Lit le premier octet sans passer au suivant
 * \param[in] uart Uart à utiliser
 * \param[out] pVal La valeur du premier octet dans la file de lecture sera stockée dans l'adresse pointée par pVal. Doit pointer vers une adresse valide
 * \return false si il n'y a pas d'octet diponnible en lecture. true sinon
 * 
 * Lit la première valeur disponnible dans la file d'attente et la stocke dans l'adresse pointée par pVal. La valeur lue n'est pas retirée de la file d'attente.
 * Deux appels consécutifs à Serial_Peek() retournerons le même résultat.
 * 
 * Si pVal est NULL ou ne pointe pas vers une adresse valide, le comportement de la fonction n'est pas défini.
 */
bool Uart_Peek(Uart uart, byte_t* pVal);


/**
 * \fn bool Uart_Read(Uart uart, byte_t* pVal)
 * \brief Lit le premier octet dans la file de lecture
 * \param[in] uart Uart à utiliser
 * \param[out] pVal La valeur du premier octet dans la file de lecture sera stockée dans l'adresse pointée par pVal. Doit pointer vers une adresse valide
 * \return false si il n'y a pas d'octet diponnible en lecture. true sinon
 * 
 * Lit la première valeur disponnible dans la file d'attente et la stocke dans l'adresse pointée par pVal. La valeur lue n'est est retirée de la file d'attente.
 * 
 * Si pVal est NULL ou ne pointe pas vers une adresse valide, le comportement de la fonction n'est pas défini.
 */

bool Uart_ReadByte(Uart uart, byte_t* pVal); 
uint Uart_Read(Uart uart, byte_t* pBuffer, uint size); 


/**
 * \fn 
 * @return 
 */
uint Uart_Available(Uart uart);

void Uart_WriteByte(Uart uart, byte_t data);
void Uart_Write(Uart uart, const byte_t* data, uint len);

/**
 * \fn Uart_Flush(const Uart uart)
 * \brief Bloque jusquà ce que tous les envois soient complétés
 * \param uart Uart à utiliser
 * 
 * Cette fonction est bloquante jusqu'a ce que tous les envois soient complétés.
 */
void Uart_Flush(const Uart uart);

#endif // !PICTAVE_UART_H
