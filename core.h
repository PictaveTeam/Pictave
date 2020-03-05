#ifndef PICTAVE_CORE_H
#define PICTAVE_CORE_H

//------------------------------------------------------------------------------
//-------------------------[DETECTION DU PIC]-----------------------------------
//------------------------------------------------------------------------------
#if defined _PIC18LF23K22_H_ || defined _PIC18LF24K22_H_ || defined _PIC18LF25K22_H_ || defined _PIC18LF26K22_H_ || \
    defined _PIC18LF43K22_H_ || defined _PIC18LF44K22_H_ || defined _PIC18LF45K22_H_ || defined _PIC18LF46K22_H_ || \
    defined _PIC18F23K22_H_  || defined _PIC18F24K22_H_  || defined _PIC18F25K22_H_  || defined _PIC18F26K22_H_  || \
    defined _PIC18F43K22_H_  || defined _PIC18F44K22_H_  || defined _PIC18F45K22_H_  || defined _PIC18F46K22_H_ 
#define DEVICE_PIC18F2X_4XK22_FAMILY

#elif   defined __24FJ64GA006_H || defined __24FJ96GA006_H || defined __24FJ128GA006_H || \
        defined __24FJ64GA008_H || defined __24FJ96GA008_H || defined __24FJ128GA008_H || \
        defined __24FJ64GA010_H || defined __24FJ96GA010_H || defined __24FJ128GA010_H
#define DEVICE_PIC24FJ128GA010_FAMILY
#endif


// TYPES PERSONNALISES

typedef unsigned char uchar;
typedef unsigned int uint;