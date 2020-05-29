/* 
 * File:   Queue.h
 * Author: Alexandre
 *
 * Created on 14 mars 2020, 17:31
 */

#ifndef BYTE_QUEUE_H_INCLUDED
#define	BYTE_QUEUE_H_INCLUDED

#include "../core.h"

struct ByteQueue_t;
typedef struct ByteQueue_t* ByteQueue;
   
/**
 * @param queue La queue à initialiser. Dois pointer sur une instace de ByteQueue_t valide
 * @param buffer Le tableau à utiliser pour la queue. Doit posséder au moins 
 * @param capacity La taille maximale de la queue. 
 * 
 * Cette fonctuion permet d'initialiser une queue vide. 
 * Cette fonction doit être appelée avant toute autre opération sur la queue
 */
void ByteQueue_Init(ByteQueue queue, byte_t *buffer, uint capacity);


/**
 * @param queue La queue à vider
 * 
 * Cette fonction permet de vider une queue
 */
void ByteQueue_Clear(ByteQueue queue);


/**
 * @param queue La queue à utiliser
 * @param value La valeur à ajouter 
 * 
 * Cette fonction permet d'ajouter une donnée à la fin de la queue.
 * Si la queue est pleine, la donnée n'est pas ajoutée
 */

void ByteQueue_Push(ByteQueue queue, byte_t value);


/**
 * @param queue La queue à utiliser
 * 
 * Cette fonction permet de retirer la donnée en tête de queue.
 * Si la queue est vide aucune donnée n'est enlevée.
 */
void ByteQueue_Pop(ByteQueue queue);


/**
 * @param queue La queue à utiliser
 * @return Le donnée en tête de queue 
 * 
 * Permet d'obtenir la donnée en tête de queue.
 * Cette fonction ne doit pes être utilisée si la queue est vide car la valeur de retour serait non significative.
 */
byte_t ByteQueue_Get(ByteQueue queue);


/**
 * @param queue La queue à utiliser
 * @return Le nombre de données actuellement dans la queue.
 * 
 * Permet d'obtenir le nombre de données actuellement présentes dans la queue.
 */
uint ByteQueue_Size(ByteQueue queue);


/**
 * @param queue La queue à utiliser
 * @return La capacité maximale de la queue.
 * 
 * Permet d'obtenir la capacité maximale de la queue.
 */
uint ByteQueue_Capacity(ByteQueue queue);

#endif	// !BYTE_QUEUE_H_INCLUDED

#if !defined(QUEUE_DEFINITION_INCLUDED) && defined(INCLUDE_QUEUE_DEFINITION)
#define QUEUE_DEFINITION_INCLUDED

struct ByteQueue_t{
    byte_t *pBuffer;
    uint backCursor;
    uint frontCursor;
    uint capacity;
    uint size;
};


#endif // (!QUEUE_DEFINITION_INCLUDED) && INCLUDE_QUEUE_DEFINITION