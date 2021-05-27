/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OPERATE_H
#define	OPERATE_H

//#include <xc.h> // include processor files - each processor file is guarded.  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "command.h"
#include "ad7323.h"

#define AVERAGE_WINDOW_SIZE 4

typedef enum {
    OPERATE_COMMAND,
    OPERATE_DATA
} OPERATE_MODE;

typedef enum {
    OPERATE_V_M60_P60,
    OPERATE_V_M30_P30,
    OPERATE_V_M15_P15,
    OPERATE_V_0_P60,
    OPERATE_V_AUTO
} OPERATE_VOLTAGE;

typedef enum {
    OPERATE_SAMPLE_SINGLE,
    OPERATE_SAMPLE_DIFF
} OPERATE_SAMPLE;

typedef enum {
    OPERATE_DATA_ASCII,
    OPERATE_DATA_BIN,
    OPERATE_DATA_HEX
} OPERATE_DATA_MODE;

// EEPROM DEFINES

#define EEPROM_STORE_V1     1

#define OPERATE_DEFAULT_SAMPLE_RATE 1
#define OPERATE_DEFAULT_CHANNELS 0x03e4     // Selected 1,2,3,4 channels

typedef struct {
    uint8_t            eeprom_ConstA[4];
    int8_t             eeprom_ConstBP[4];
    int8_t             eeprom_ConstBN[4];
    uint8_t            eeprom_ConstDiffA[2];
    int8_t             eeprom_ConstDiffBP[2];
    int8_t             eeprom_ConstDiffBN[2];
} CALIBRATION_V1;

typedef struct {
    OPERATE_VOLTAGE    eeprom_Voltage;
    OPERATE_SAMPLE     eeprom_SampleMode;
    OPERATE_DATA_MODE  eeprom_DataMode;
    uint16_t           eeprom_SampleRate;
    uint16_t           eeprom_Channels; //4 channels selector <NUMCHANNELS1-1><NUMCHANNELS0-1><4.AD1><4.AD0><3.AD1><3.AD0><2.AD1><2.AD0><1.AD1><1.AD0> bits
    CALIBRATION_V1     CalibrationV1;
    CALIBRATION_V1     CalibrationFactoryV1;
} STORE_V1;

eeprom typedef struct {
    uint8_t            eeprom_StoreVersion;
    uint8_t            eeprom_CheckSum;
    
    union {
        STORE_V1       StoreV1;
        uint8_t        eeprom_StoreBytes[sizeof(STORE_V1)];
    };
} EEPROMSTORE;

extern EEPROMSTORE eepromStoreConfig;

extern volatile uint8_t timerDivider;
extern volatile uint8_t timerDividerReloader;
extern bool operateFirstInitADC;

void operateInit();
void operateStoreToEEPROM();
void operateSetFactory();
inline OPERATE_MODE operateGetMode();
void operateSetMode(OPERATE_MODE mode);
inline uint16_t operateGetRate();
bool operateSetRate(uint16_t rate);
inline uint16_t operateGetChannels();
bool operateSetChannels(uint16_t channels);
#ifndef CMD_VERSION_EASY
//inline OPERATE_VOLTAGE operateGetVoltage();
bool operateSetVoltage();   //(OPERATE_VOLTAGE voltage);
#else
bool operateSetVoltage();
#endif
inline OPERATE_SAMPLE operateGetSample();
bool operateSetSample(OPERATE_SAMPLE sample);
inline OPERATE_DATA_MODE operateGetData();
bool operateSetData(OPERATE_DATA_MODE data);
void operateSetA(uint8_t index, uint8_t a);
void operateSetAF(uint8_t index, uint8_t a);
uint8_t operateGetA(uint8_t index);
void operateSetBP(uint8_t index, int8_t b);
void operateSetBN(uint8_t index, int8_t b);
void operateSetBPF(uint8_t index, int8_t b);
void operateSetBNF(uint8_t index, int8_t b);
int8_t operateGetBP(uint8_t index);
int8_t operateGetBN(uint8_t index);
void operateTMR();
void operateTMRMathSend();
void operateDo(uint16_t channels, bool needA);

//void operateMathCheck();

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* OPERATE_H */

