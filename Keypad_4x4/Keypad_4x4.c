#include "Keypad_4x4.h"

#ifdef USE_KEYPAD4x4

#define KEYPAD_NUM_KEYS 16
/*static PinInfo mKeypadPins[8];
static char mKeysValue[4][4]; // [Ligne][Colonne]
static unsigned int mKeysResult; */

void Keypad4x4_Init(Keypad4x4* keypad){
    keypad->keysState = 0;
    char i;
    #if defined KP_RDRIVE_CLOW 
        for(i = KP_ROW0 ; i <= KP_ROW3 ; i++){ // Rows as output, and drived low
            (*mKeypadPins[i].pinTris) &= (unsigned int) ~(mKeypadPins[i].pinBit);  // Mise a 0 du bit port dans tris
            (*mKeypadPins[i].pinPort) &= (unsigned int) ~(mKeypadPins[i].pinBit); // Mise a 0 du bit dans le port;
        }
    
        for(i = KP_COL0 ; i <= KP_COL3 ; i++){ // Collumns as input
            (*mKeypadPins[i].pinTris) |= (unsigned int)  mKeypadPins[i].pinBit;  // Mise a 1 du bit port dans tris
        }
    #elif defined KP_RDRIVE_CHIGH
        for(i = KP_ROW0 ; i <= KP_ROW3 ; i++){ // Rows as output, and drived high
            (*mKeypadPins[i].pinTris) &=  (unsigned int) ~(mKeypadPins[i].pinBit);  // Mise a 0 du bit port dans tris
            (*mKeypadPins[i].pinPort) |=  (unsigned int) mKeypadPins[i].pinBit; // Mise a 1 du bit dans le port;
        }
    
        for(i = KP_COL0 ; i <= KP_COL3 ; i++){ // Collumns as input
            (*mKeypadPins[i].pinTris) |=  (unsigned int) mKeypadPins[i].pinBit;  // Mise a 1 du bit port dans tris
        }
    #elif defined KP_CDRIVE_RLOW
        for(i = KP_COL0 ; i <= KP_COL3 ; i++){ // Collumns as output, and drived low
            (*keypad->pins[i].pinTris) &=  (unsigned int) ~(keypad->pins[i].pinBit);  // Mise a 0 du bit port dans tris
            (*keypad->pins[i].pinPort) &=  (unsigned int) ~(keypad->pins[i].pinBit); // Mise a 0 du bit dans le port;
        }
    
        for(i = KP_ROW0 ; i <= KP_ROW3 ; i++){ // Rows as input
            (*keypad->pins[i].pinTris) |=  (unsigned int) keypad->pins[i].pinBit;  // Mise a 1 du bit port dans tris
        }

    #elif defined KP_CDRIVE_RHIGH
        for(i = KP_COL0 ; i <= KP_COL3 ; i++){ // Collumns as output, and drived high
            (*mKeypadPins[i].pinTris) &= (unsigned int)~(mKeypadPins[i].pinBit);  // Mise a 0 du bit port dans tris
            (*mKeypadPins[i].pinPort) |= (unsigned int)  mKeypadPins[i].pinBit; // Mise a 1 du bit dans le port;
        }
    
        for(i = KP_ROW0 ; i <= KP_ROW3 ; i++){ // Rows as input
            (*mKeypadPins[i].pinTris) |= (unsigned int) mKeypadPins[i].pinBit;  // Mise a 1 du bit port dans tris
        }
    #endif
}

unsigned int Keypad4x4_Read(Keypad4x4* keypad){
    unsigned char i; 
    unsigned char j;
    unsigned int result = 0; // On utilise pas mKeyResult car si une interruption survient le résultat sera temporairement = 0
    
    #if defined KP_RDRIVE_CLOW 
        [TODO]
        for(i = KP_ROW0 ; i <= KP_ROW3 ; i++){ // Drive the row HIGH to read the collumn
            (*mKeypadPins[i].pinPort) |= (unsigned int) mKeypadPins[i].pinBit; // Mise a 1 du bit dans le port;
            for(j = KP_COL0 ; j <= KP_COL3 ; j++){ // Read each collumn
                if((*mKeypadPins[j].pinPort) &  mKeypadPins[j].pinBit){
                    result++;
                }
                result = result << 1; // Décalage vers la gauche
            }
            (*mKeypadPins[i].pinPort) &= (unsigned int) ~mKeypadPins[i].pinBit; // Mise a 0 du bit dans le port;
        }
    #elif defined KP_RDRIVE_CHIGH
       // TODO
    #elif defined KP_CDRIVE_RLOW
        for(i = KP_COL0 ; i <= KP_COL3 ; i++){ // Drive the row HIGH to read the collumn
            // On active et désactive la sortie a chaque fois pour eviter les cours circuit avec les autres colonnes
            (*keypad->pins[i].pinTris) &= (unsigned int) ~keypad->pins[i].pinBit; // Mise a 0 du bit dans le tris;
            (*keypad->pins[i].pinPort) |= (unsigned int) keypad->pins[i].pinBit; // Mise a 1 du bit dans le port;
            for(j = KP_ROW0 ; j <= KP_ROW3 ; j++){ // Read each collumn
                if((*keypad->pins[j].pinPort) &  keypad->pins[j].pinBit){
                    result |= (0x8000) >> (j - KP_ROW0)*4 + (i-KP_COL0);
                }
            }
            (*keypad->pins[i].pinPort) &= (unsigned int) ~keypad->pins[i].pinBit; // Mise a 0 du bit dans le port;
            (*keypad->pins[i].pinTris) |= (unsigned int) keypad->pins[i].pinBit; // Mise a 1 du bit dans le tris;
        }
    #elif defined KP_CDRIVE_RHIGH
       // [TODO]
    #endif
    keypad->keysState = result;
    return result;
}
char Keypad4x4_GetKeyState(Keypad4x4* keypad, const char c){
    char i,j;
    unsigned int index = 0x8000;
    for(i = 0; i < 4; i++){ // Lignes
        for(j = 0; j < 4; j++){ // Colonnes
            if(keypad->keysName[i][j] == c){
                if(keypad->keysState & index){
                    return 1;
                }
                return 0;
            }
            index = index >> 1;
        }
    }
    return -1;
}
char Keypad4x4_GetKeyState_ByPos(Keypad4x4* keypad, uchar col, uchar row){
    if(col >= 4 || row >= 4){
        return -1;
    }
    
    uint index = (uint) 0x8000 >> (row*4+col);
    if(keypad->keysState & index){
        return 1;
    }
    return 0;
}

char Keypad4x4_GetKeys(Keypad4x4* const keypad, char* keyArray, uchar maxKeys){
    uint index = 0x8000;
    char result = 0;
    
    if(maxKeys > 16){
        maxKeys = 16;
    }
    
    uchar col, row;
    for(row = 0; row < 4; row++){
        for(col = 0; col < 4; col++){
            if(index & keypad->keysState){
                keyArray[result] = keypad->keysName[row][col];
                result ++;
                if(result == maxKeys){
                    return result;
                }
            }
            index = index >> 1;
        }
    }
    
    return result;
}


#undef KEYPAD_NUM_KEYS

#endif // defined USE_KEYPAD4x4
