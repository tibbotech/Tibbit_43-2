/*
 * Project: Tibbit #43-1
 * File:    command.h
 * Author:  Dmitry Tarasov
 * License: GPLv3
 * Copyright (c) 2021. Tibbo Technology Inc. 
 * SPDX-License-Identifier: GPL-3.0
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

