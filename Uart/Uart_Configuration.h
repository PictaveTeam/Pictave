#ifndef PICTAVE_UART_CONFIGURATION_H
#define PICTAVE_UART_CONFIGURATION_H

// La taille par défaut des queues des UARTs.
#define UART_DEFAULT_QUEUE_CAPACITY 64 


/*
 * Les lignes suivantes contiennent les données pour la configuration des 
 * différents ports série. 
 * Pour l'uart X (X = 0, 1, 2 ...) on a :
 
 * --- UARTX_ENABLED [0|1] ---
 * Définit si l'UART X est activé ou non. Il est conseillé de désactiver les UARTs non utilisés pour économiser
 * la ram et la mémoire flash.                    
 * 0 : L'UART ne peut pas être utilisé avec les fonctions UART_XXX, et aucun code n'est compilé pour cet UART.
 *     Il est toujours possible d'utiliser l'UART en manipulant directement les registres.
 * 1 : L'UART peut être utilisé via les fonctions dédiées de la bibliothèque. Il est fortement déconseillé
 *     de manipuler directement les registres liés à l'utilisation du port série pour éviter les conflits 
 *
 * --- UARTX_RECEIVE_QUEUE_CAPACITY n ---
 * Permet de définir la taille maximale de la queue de réception de l'UART X.
 * Pour être plus clair, le nombre maximal d'octets qu'il est possible de recevoir d'affilée sans effectuer de
 * lecture. Une fois cette capacité maximale dépassée, les données recue écraseront les données les plus ancienes.
 * 
 * --- UART1_TRANSMIT_QUEUE_CAPACITY n ---
 * Définit la taille maximale de la queue d'envoi en octets.
 * Une fois la taille maximale dépassée, un envoi sera bloquant tant que la taille maximale de la queue sera
 * dépassée. Une valeur plus grande permet d'éviter que les envois soient bloquants mais occupe plus de RAM.
 */

// [CONFIGURATION UART 1]
#define UART1_ENABLED 1
#define UART1_RECEIVE_QUEUE_CAPACITY UART_DEFAULT_QUEUE_CAPACITY
#define UART1_TRANSMIT_QUEUE_CAPACITY UART_DEFAULT_QUEUE_CAPACITY


// [CONFIGURATION UART 2]
#define UART2_ENABLED 1 // 
#define UART2_RECEIVE_QUEUE_CAPACITY UART_DEFAULT_QUEUE_CAPACITY
#define UART2_TRANSMIT_QUEUE_CAPACITY UART_DEFAULT_QUEUE_CAPACITY

#endif // !PICTAVE_UART_CONFIGURATION_H