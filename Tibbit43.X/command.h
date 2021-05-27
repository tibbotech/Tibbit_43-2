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
#ifndef COMMAND_H
#define	COMMAND_H

#include <string.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "ad7323.h"
#include "operate.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#define CMD_BUFFER_SIZE_MAX     34

//#define CMD_VERSION_EASY        // If this define is defined then compiled EASY version of FW 10V
    
    // Define USART commands
#define CMD_STX             0x02
#define CMD_CR              0x0d
#define CMD_ANSW_OK         'A'

typedef enum {
    CMD_UNKNOWN,
    CMD_GETVERSION,
    CMD_SETCOMMANDMODE,
    CMD_SETDATAMODE,
    CMD_READCHANNELSASCII,
    CMD_READCHANNELSHEX,
    CMD_RESETSETTINGSTOFACTORY,
    CMD_READSETTINGSFROMEEPROM,
    CMD_SELECTINPUTVOLTAGE,
    CMD_SETSAMPLINGRATE,
    CMD_SELECTSAMPLINGMODE,
    CMD_ENABLECHANNELS,
    CMD_SELECTDATAFORMAT,
    // New command
    CMD_READSETTINGSCURRENT,
    CMD_STORETOEEPROM,
    CMD_RESTOREFROMEEPROM,
    CMD_SETA,
    CMD_SETBP,
    CMD_SETBN,
    CMD_SETAF,
    CMD_SETBPF,
    CMD_SETBNF
} CMD_COMMAND;

typedef enum {
    PARSE_OK,
    PARSE_SYNTAX,
    PARSE_OUTRANGE
} PARSE_RESULT;

uint8_t cmdBufPointer;

extern uint8_t cmdElementsNumber;
extern int16_t cmdParams[6];
extern uint8_t cmdParamsNumber;
extern CMD_COMMAND cmdCommand;

void cmdReceiveOperate(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

