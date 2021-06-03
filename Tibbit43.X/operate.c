/*
 * Project: Tibbit #43-1
 * File:    operate.c
 * Author:  Dmitry Tarasov
 * License: GPLv3
 * Copyright (c) 2021. Tibbo Technology Inc. 
 * SPDX-License-Identifier: GPL-3.0
 */

 #include "operate.h"

// EEPROM store
// Settings
EEPROMSTORE eepromStoreConfig;
EEPROMSTORE eepromStoreShadow;

typedef struct {
    int16_t ADC[16];
} ADC_RAW;

typedef struct {
    int16_t ADC[4];
} ADC_RESULTS;

typedef struct {
    uint8_t SeqChannels[4];
    uint8_t NumSeqChannels;
} SEQUENCE_CHANNELS;

OPERATE_MODE      operateMode;
OPERATE_SAMPLE    operateSample;
OPERATE_DATA_MODE operateData;
uint16_t          operateRate;
uint16_t          operateChannels;
uint8_t           operateA[4];
int8_t            operateBP[4];
int8_t            operateBN[4];
uint8_t           operateDiffA[2];
int8_t            operateDiffBP[2];
int8_t            operateDiffBN[2];

bool              operateFirstInitADC;

SEQUENCE_CHANNELS operateSeqTMR;

#define           ADC_BUFFER_SIZE       4   // Only power of 2
volatile uint8_t  operateADCBufferHead;
volatile uint8_t  operateADCBufferLength;
volatile ADC_RAW  operateADCBuffer[ADC_BUFFER_SIZE];

// Table for TMR1 operation for long periods 1-15Hz
typedef struct {
    uint8_t swDiv;
    uint16_t hwDiv;
} TMR1_DIVIDERS;

volatile uint8_t timerDivider;
volatile uint8_t timerDividerReloader;

/*
 1             16            62500
 2             10            50000
 3             7             47619
 4             5             50000
 5             4             50000
 6             5             33333.4
 7             3             47619
 8             4             31250
 9             3             37037
 10            4             25000
 11            3             30303
 12            2             41666.5
 13            3             25641
 14            2             35714.5
 15            2             33333.5
*/

const TMR1_DIVIDERS tmrDivs[15] = {{16, 62500},
                                   {10, 50000},
                                   { 7, 47619},
                                   { 5, 50000},
                                   { 4, 50000},
                                   { 5, 33333},
                                   { 3, 47619},
                                   { 4, 31250},
                                   { 3, 37037},
                                   { 4, 25000},
                                   { 3, 30303},
                                   { 2, 41667},
                                   { 3, 25641},
                                   { 2, 35715},
                                   { 2, 33334}};

void operateFillSequencer(uint16_t channels, SEQUENCE_CHANNELS* sequencer);

void operateTMRset(uint16_t rate)
{
    uint16_t _hwDiv;
    
    // Calculate dividers for TMR1 for sampling rate
    if ((rate > 0) && (rate < 1001)) {  // check valid range
        TMR1_StopTimer();
        if (rate <= 15) { // Need table for select dividers of TMR1 for this rate
            timerDividerReloader = tmrDivs[rate - 1].swDiv;
            _hwDiv = tmrDivs[rate - 1].hwDiv;
        } else {
            // Calculate divider
            timerDividerReloader = 0;
            _hwDiv = (uint32_t)1000000/rate;
        }
        timerDivider = 0;
        TMR1_WriteReloader(0 - _hwDiv);
        TMR1_Reload();
        TMR1_StartTimer();
    }
}

void operateStoreCheckSumSet()
{
    uint8_t _sum = 0;
    
    for (uint8_t _i = 0; _i < sizeof(STORE_V1); _i++)
        _sum += eepromStoreConfig.eeprom_StoreBytes[_i];
    
    eepromStoreConfig.eeprom_CheckSum = _sum;
}

bool operateStoreCheckSumChecking(bool isShadow)
{
    uint8_t _sum = 0;
    
    if (isShadow) {
        for (uint8_t _i = 0; _i < sizeof(STORE_V1); _i++)
            _sum += eepromStoreShadow.eeprom_StoreBytes[_i];

        if (_sum == eepromStoreShadow.eeprom_CheckSum)
            return true;
        else
            return false;
    } else {
        for (uint8_t _i = 0; _i < sizeof(STORE_V1); _i++)
            _sum += eepromStoreConfig.eeprom_StoreBytes[_i];

        if (_sum == eepromStoreConfig.eeprom_CheckSum)
            return true;
        else
            return false;
    }
}

void operateCopyEEPROM(bool toShadow)
{
    uint8_t _data;
    uint8_t _addrS;
    uint8_t _addrD;
    
    if (toShadow) { // Copy MASTER to SHADOW
        _addrS = &eepromStoreConfig;
        _addrD = &eepromStoreShadow;
    } else {        // Copy SHADOW to MASTER
        _addrS = &eepromStoreShadow;
        _addrD = &eepromStoreConfig;
    }
    for (uint8_t _i = 0; _i < sizeof(eepromStoreConfig); _i++) {
        _data = eeprom_read(_addrS + _i);
        eeprom_write(_addrD + _i, _data);
    }
}

void operateEEPROMInit()
{
    bool _master = false;
    bool _shadow = false;

    // Check MASTER 
    if (eepromStoreConfig.eeprom_StoreVersion == EEPROM_STORE_V1)
        if (operateStoreCheckSumChecking(false))   // CheckSum OK
            // Master copy is OK
            _master = true;

    // Check SHADOW
    if (eepromStoreShadow.eeprom_StoreVersion == EEPROM_STORE_V1)
        if (operateStoreCheckSumChecking(true))   // CheckSum OK
            // S copy is OK
            _shadow = true;
    
    if ((_master == true) && (_shadow == true)) return;
    
    if (_master) {
        // Copy MASTER >> SHADOW
        operateCopyEEPROM(true);
        return;
    }
    
    if (_shadow) {
        // Copy SHADOW >> MASTER
        operateCopyEEPROM(false);
        return;
    }
    
    // Bad all copies. Need set to FACTORY
    operateSetFactory();
}

void operateInit()
{
    bool _needResetToFactory;
    
    // Set AD7323
    AD7323_CS_SetDigitalMode();
    AD7323_CS_SetDigitalOutput();
    AD7323_CS_SetHigh();

    _needResetToFactory = false;
    
    // Check EEPROM data consystency 
    operateEEPROMInit();
    
    operateMode = OPERATE_COMMAND;
    if (!operateSetSample(eepromStoreConfig.StoreV1.eeprom_SampleMode)) _needResetToFactory = true;
    if (!operateSetData(eepromStoreConfig.StoreV1.eeprom_DataMode)) _needResetToFactory = true;
    operateSetVoltage();
    if (!operateSetRate(eepromStoreConfig.StoreV1.eeprom_SampleRate)) _needResetToFactory = true;
    if (!operateSetChannels(eepromStoreConfig.StoreV1.eeprom_Channels)) _needResetToFactory = true;
    for (uint8_t _i = 0; _i < 4; _i++) {
        operateSetA(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstA[_i]);
        operateSetBP(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBP[_i]);
        operateSetBN(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBN[_i]);
    }
    for (uint8_t _i = 0; _i < 2; _i++) {
        operateSetA(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffA[_i]);
        operateSetBP(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBP[_i]);
        operateSetBN(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBN[_i]);
    }
    
    if (_needResetToFactory) {
        // Need set factory settings
        operateSetFactory();
    }
    
    operateFirstInitADC = true;
    
    operateADCBufferHead = 0;
    operateADCBufferLength = 0;
}

void operateStoreToEEPROM()
{
    if (eepromStoreConfig.StoreV1.eeprom_SampleRate != operateGetRate())
        eepromStoreConfig.StoreV1.eeprom_SampleRate = operateGetRate();
    if (eepromStoreConfig.StoreV1.eeprom_Channels != operateGetChannels())
        eepromStoreConfig.StoreV1.eeprom_Channels = operateGetChannels();
    if (eepromStoreConfig.StoreV1.eeprom_DataMode != operateGetData())
        eepromStoreConfig.StoreV1.eeprom_DataMode = operateGetData();
    if (eepromStoreConfig.StoreV1.eeprom_SampleMode != operateGetSample())
        eepromStoreConfig.StoreV1.eeprom_SampleMode = operateGetSample();
    for (uint8_t _i = 0; _i < 4; _i++) {
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstA[_i] != operateGetA(_i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstA[_i] = operateGetA(_i);
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBP[_i] != operateGetBP(_i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBP[_i] = operateGetBP(_i);
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBN[_i] != operateGetBN(_i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBN[_i] = operateGetBN(_i);
    }
    for (uint8_t _i = 0; _i < 2; _i++) {
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffA[_i] != operateGetA(4 + _i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffA[_i] = operateGetA(4 + _i);
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBP[_i] != operateGetBP(4 + _i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBP[_i] = operateGetBP(4 + _i);
        if (eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBN[_i] != operateGetBN(4 + _i))
            eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBN[_i] = operateGetBN(4 + _i);
    }
    
    operateStoreCheckSumSet();
    operateCopyEEPROM(true);
}

void operateSetFactory()
{
    eepromStoreConfig.eeprom_StoreVersion = EEPROM_STORE_V1;
    
    operateMode = OPERATE_COMMAND;
    operateSetSample(OPERATE_SAMPLE_SINGLE);
    operateSetData(OPERATE_DATA_ASCII);
    operateSetVoltage(OPERATE_V_M60_P60);
    operateSetRate(1);
    operateSetChannels(0x03e4);
    
    if ((eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstA[0] == 0x00) ||
        (eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstA[0] == 0xff)) {
        // Set Default FACTORY calibration
        for (uint8_t _i = 0; _i < 4; _i++) {
            eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstA[_i] = 0x80;
        }
        for (uint8_t _i = 0; _i < 2; _i++) {
            eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffA[_i] = 0x80;
        }
    }
    // Restore Calibrations from factory EEPROM place
    for (uint8_t _i = 0; _i < 4; _i++) {
        operateSetA(_i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstA[_i]);
        operateSetBP(_i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstBP[_i]);
        operateSetBN(_i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstBN[_i]);
    }
    for (uint8_t _i = 0; _i < 2; _i++) {
        operateSetA(4 + _i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffA[_i]);
        operateSetBP(4 + _i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffBP[_i]);
        operateSetBN(4 + _i, eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffBN[_i]);
    }
    
    operateStoreToEEPROM();
}

inline OPERATE_MODE operateGetMode()
{
    return operateMode;
}

void operateSetMode(OPERATE_MODE mode)
{
    if ((mode == OPERATE_COMMAND) || (mode == OPERATE_DATA))
        operateMode = mode;
    operateFirstInitADC = true;
}

inline uint16_t operateGetRate()
{
    return operateRate;
}

bool operateSetRate(uint16_t rate)
{
    if (operateMode == OPERATE_COMMAND)
        if ((rate > 0) && (rate <= 1000)) {
            operateRate = rate;
            operateTMRset(operateRate); // Set new rate to TMR1
            return true;
        }
    return false;
}

inline uint16_t operateGetChannels()
{
    return operateChannels;
}

bool operateSetChannels(uint16_t channels)
{
    if (operateMode == OPERATE_COMMAND) {
        operateChannels = channels;
        operateFillSequencer(channels, &operateSeqTMR);
        return true;
    }
    return false;
}

bool operateSetVoltage()
{
    uint16_t _AD7323Range;
    
    _AD7323Range = RANGE_A_10V | RANGE_B_10V | RANGE_C_10V | RANGE_D_10V; // set range -10+10 for all channels
    SPI_Exchange16bit(AD7323_WRITE_MODE | AD7323_RANGE_REGISTER | ((uint16_t)_AD7323Range << 5));
    
    return true;
}

inline OPERATE_SAMPLE operateGetSample()
{
    return operateSample;
}

bool operateSetSample(OPERATE_SAMPLE sample)
{
    uint16_t _AD7323_ControlRegister;
    
    if (operateMode == OPERATE_COMMAND)
        if ((sample >= OPERATE_SAMPLE_SINGLE) && (sample <= OPERATE_SAMPLE_DIFF)) {
            operateSample = sample;
            if (sample == OPERATE_SAMPLE_SINGLE) {
                _AD7323_ControlRegister = AD7323_CONTROL_REGISTER | AD7323_CONTROL_4SINGLE_INPUTS | 
                                          AD7323_CONTROL_NORMAL_POWER | AD7323_CONTROL_INTERNAL_REFERENCE | 
                                          AD7323_CONTROL_SEQUENCER_NOT_USE;
                operateSetChannels(0x03e4); // Set four channels for read 1,2,3,4
            }
            if (sample == OPERATE_SAMPLE_DIFF) {
                _AD7323_ControlRegister = AD7323_CONTROL_REGISTER | AD7323_CONTROL_2FULLY_DIFF_INPUTS | 
                                          AD7323_CONTROL_NORMAL_POWER | AD7323_CONTROL_INTERNAL_REFERENCE | 
                                          AD7323_CONTROL_SEQUENCER_NOT_USE;
                operateSetChannels(0x0104); // Set two channels for read 1,2
            }
            SPI_Exchange16bit(AD7323_WRITE_MODE | _AD7323_ControlRegister);
            
            operateFillSequencer(operateChannels, &operateSeqTMR);
            
            return true;
        }
    
    return false;
}

inline OPERATE_DATA_MODE operateGetData()
{
    return operateData;
}

bool operateSetData(OPERATE_DATA_MODE data)
{
    if (operateMode == OPERATE_COMMAND)
        if ((data >= OPERATE_DATA_ASCII) && (data <= OPERATE_DATA_HEX)) {
            operateData = data;
            
            return true;
        }
    
    return false;
}

void operateSetA(uint8_t index, uint8_t a)
{
    if (index < 4) 
        operateA[index] = a;
    
    if ((index >= 4) && (index < 6))
        operateDiffA[index - 4] = a;
}

void operateSetAF(uint8_t index, uint8_t a)
{
    if (index < 4) {
        operateA[index] = a;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstA[index] = a;
    }
    
    if ((index >= 4) && (index < 6)) {
        operateDiffA[index - 4] = a;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffA[index - 4] = a;
    }

    operateStoreCheckSumSet();
}

uint8_t operateGetA(uint8_t index)
{
    if (index < 4)
        return operateA[index];
    
    if ((index >= 4) && (index < 6))
        return operateDiffA[index - 4];
    
    return 0;
}

void operateSetBP(uint8_t index, int8_t b)
{
    if (index < 4)
        operateBP[index] = b;

    if ((index >= 4) && (index < 6))
        operateDiffBP[index - 4] = b;
}

void operateSetBN(uint8_t index, int8_t b)
{
    if (index < 4)
        operateBN[index] = b;

    if ((index >= 4) && (index < 6))
        operateDiffBN[index - 4] = b;
}

void operateSetBPF(uint8_t index, int8_t b)
{
    if (index < 4) {
        operateBP[index] = b;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstBP[index] = b;
    }
    
    if ((index >= 4) && (index < 6)) {
        operateDiffBP[index - 4] = b;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffBP[index - 4] = b;
    }

    operateStoreCheckSumSet();
}

void operateSetBNF(uint8_t index, int8_t b)
{
    if (index < 4) {
        operateBN[index] = b;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstBN[index] = b;
    }
    
    if ((index >= 4) && (index < 6)) {
        operateDiffBN[index - 4] = b;
        eepromStoreConfig.StoreV1.CalibrationFactoryV1.eeprom_ConstDiffBN[index - 4] = b;
    }

    operateStoreCheckSumSet();
}

int8_t operateGetBP(uint8_t index)
{
    if (index < 4)
        return operateBP[index];

    if ((index >= 4) && (index < 6))
        return operateDiffBP[index - 4];
    
    return 0;
}

int8_t operateGetBN(uint8_t index)
{
    if (index < 4)
        return operateBN[index];

    if ((index >= 4) && (index < 6))
        return operateDiffBN[index - 4];
    
    return 0;
}

inline void operateSendHexNibble(uint8_t data)
{
    if ((data & 0x0F) > 9)
        EUSART_Write((data & 0x0F) + 'A' - 10);
    else
        EUSART_Write((data & 0x0F) + '0');
}

inline void operateSendHexByte(uint8_t data)
{
    operateSendHexNibble(data >> 4);
    operateSendHexNibble(data);
}

inline void operateSendHex(uint16_t data)
{
    operateSendHexByte(data >> 8);
    operateSendHexByte(data);
}

inline void operateSendHex32(uint32_t data)
{
    operateSendHex(data >> 16);
    operateSendHex(data);
}

inline void operateSendString(char* data)
{
    for (uint8_t _i = 0; _i < 24; _i++) {
        if (data[_i] == 0) return;
        EUSART_Write(data[_i]);
    }
}

inline void sprintDecFixPoint(int32_t data)
{
    char _str[10];
    uint8_t _i;
    
    _str[9] = 0;
    
    if (data < 0) {
        _str[0] = '-';
        data = -data;
    } else
        _str[0] = 0;
    
    for (_i = 8; _i >= 1; _i--) {
        if (_i == 5)
            _str[_i] = '.';
        else {
            // get one DEC digit
            _str[_i] = (data % 10) + '0';
            data /= 10;
            if ((!data) && (_i < 5)) break;
        }
    }

    if (_str[0]) {
        _str[_i - 1] = '-';
        operateSendString((char*)_str + _i - 1);
    } else {
        operateSendString((char*)_str + _i);
    }
}

void operateSendSample(uint16_t data)
{
    int32_t _tempCalc;
    
    switch (operateData) {
        case OPERATE_DATA_ASCII:
            // Calculate
            _tempCalc = (int32_t)100574;
            _tempCalc = (_tempCalc * (int16_t)data) / 4096;
            sprintDecFixPoint(_tempCalc);
            break;
            
        case OPERATE_DATA_BIN:
            EUSART_Write(data >> 8);
            EUSART_Write(data);
            break;
            
        case OPERATE_DATA_HEX:
            operateSendHex(data);
            break;
            
        default:
            break;
    }
}

uint16_t operateSetControlADC(uint8_t channel)
{
    uint16_t _AD7323Set;
    
    if (operateSample == OPERATE_SAMPLE_SINGLE)
        _AD7323Set = AD7323_WRITE_MODE |
                     AD7323_CONTROL_REGISTER | AD7323_CONTROL_4SINGLE_INPUTS | 
                     AD7323_CONTROL_NORMAL_POWER | AD7323_CONTROL_INTERNAL_REFERENCE | 
                     AD7323_CONTROL_SEQUENCER_NOT_USE;
    else
        _AD7323Set = AD7323_WRITE_MODE |
                     AD7323_CONTROL_REGISTER | AD7323_CONTROL_2FULLY_DIFF_INPUTS | 
                     AD7323_CONTROL_NORMAL_POWER | AD7323_CONTROL_INTERNAL_REFERENCE | 
                     AD7323_CONTROL_SEQUENCER_NOT_USE;

    // Add Channel address
    _AD7323Set |= ((uint16_t)(channel & 0x03)) << 10;

    return _AD7323Set;
}

void operateReadChannels(SEQUENCE_CHANNELS* sequencer, ADC_RAW *adc)
{
    uint16_t _ADCvalue;
    uint8_t _i, _j;
    
    if (operateFirstInitADC) {
        // Set mode & channel address for first read sample
        SPI_Exchange16bit(operateSetControlADC(sequencer->SeqChannels[0]));
        operateFirstInitADC = false;
    }
    
    for (_j = 0; _j < AVERAGE_WINDOW_SIZE; _j++) {
        for (_i = 0; _i < sequencer->NumSeqChannels; _i++) {

            if (_i == sequencer->NumSeqChannels - 1) {
                _ADCvalue = SPI_Exchange16bit(operateSetControlADC(sequencer->SeqChannels[0]));
            }
            else {
                _ADCvalue = SPI_Exchange16bit(operateSetControlADC(sequencer->SeqChannels[_i + 1]));
            }
            
            if (_ADCvalue & 0x1000)                   // Test SIGN bit
                adc->ADC[_i * 4 + _j] = (int16_t)(0xf000 | _ADCvalue);  // expand SIGN
            else
                adc->ADC[_i * 4 + _j] = (int16_t)(0x0fff & _ADCvalue);
        }
    }
}

void operateMathValues(SEQUENCE_CHANNELS* sequencer, ADC_RAW* adc, ADC_RESULTS *results)
{
    for (uint8_t _i = 0; _i < sequencer->NumSeqChannels; _i++) {
        // Averaging
        results->ADC[_i] = 0;
        for (uint8_t _j = 0; _j < AVERAGE_WINDOW_SIZE; _j++)
            results->ADC[_i] += adc->ADC[_i * 4 + _j];
        results->ADC[_i] /= AVERAGE_WINDOW_SIZE;

        // Math = value[i] * A[i] + B[i]]
        if (operateSample == OPERATE_SAMPLE_DIFF) {
            results->ADC[_i] = (((int32_t)results->ADC[_i]) * operateDiffA[sequencer->SeqChannels[_i]/2]) / 128;
            if (results->ADC[_i] >= 0)
                results->ADC[_i] += operateDiffBP[sequencer->SeqChannels[_i]/2];
            else 
                results->ADC[_i] += operateDiffBN[sequencer->SeqChannels[_i]/2];
            results->ADC[_i] *= 2;
        } else {
            //DEBUG_2_SetHigh();
            results->ADC[_i] = (((int32_t)results->ADC[_i]) * operateA[sequencer->SeqChannels[_i]]) / 128;
            //DEBUG_2_SetLow();
            if (results->ADC[_i] >= 0)
                results->ADC[_i] += operateBP[sequencer->SeqChannels[_i]];
            else
                results->ADC[_i] += operateBN[sequencer->SeqChannels[_i]];
        }
    }
}

void operateFillSequencer(uint16_t channels, SEQUENCE_CHANNELS* sequencer)
{
    sequencer->NumSeqChannels = ((channels & 0x0300) >> 8) + 1;
    if (!sequencer->NumSeqChannels) return;
    
    for (uint8_t _i = 0; _i < sequencer->NumSeqChannels; _i++) {
        if (operateSample == OPERATE_SAMPLE_SINGLE)
            sequencer->SeqChannels[_i] = channels & 0x03;
        else 
            sequencer->SeqChannels[_i] = (channels & 0x03) * 2;
        channels = channels >> 2;
    }
}

void operateSendOneSample(SEQUENCE_CHANNELS* sequencer, ADC_RAW* adcValues, bool needA)
{
    uint8_t _i;
    ADC_RESULTS _results;
    
    // Math to all values for coefficients x*A+B
    operateMathValues(sequencer, adcValues, &_results);
    
    // Send data
    if (operateMode == OPERATE_COMMAND)
        EUSART_Write(CMD_STX);
    if (needA)
        EUSART_Write(CMD_ANSW_OK);

        
    for (_i = 0; _i < sequencer->NumSeqChannels; _i++) {
        if ((operateData == OPERATE_DATA_BIN) || (operateData == OPERATE_DATA_HEX))
            if (operateSample == OPERATE_SAMPLE_DIFF) {
                operateSendSample((((uint16_t)sequencer->SeqChannels[_i]) << 14) | _results.ADC[_i]);
            } else {
                operateSendSample((((uint16_t)sequencer->SeqChannels[_i]) << 14) | (_results.ADC[_i] & 0x2fff));
            }
        else
            operateSendSample(_results.ADC[_i]);
        
        if (_i < sequencer->NumSeqChannels - 1)
            if (operateData != OPERATE_DATA_BIN)
                EUSART_Write(',');
    }

    if (operateData != OPERATE_DATA_BIN) 
        EUSART_Write(';');

    if (operateMode == OPERATE_COMMAND)
        EUSART_Write(CMD_CR);
}

void operateTMR()
{
    // Called from ISR TMR1 for current sampling rate
    if (operateMode == OPERATE_DATA) {
        if (operateADCBufferLength < ADC_BUFFER_SIZE) {
            // Get all values from ADC
            operateReadChannels(&operateSeqTMR, &operateADCBuffer[operateADCBufferHead]);
            operateADCBufferLength++;
            operateADCBufferHead = (operateADCBufferHead + 1) & (ADC_BUFFER_SIZE - 1);
        }
    }
}

void operateTMRMathSend()
{
    if (operateADCBufferLength > 0) {
        // ADC data present
        operateSendOneSample(&operateSeqTMR,
                             &operateADCBuffer[(operateADCBufferHead - operateADCBufferLength) & (ADC_BUFFER_SIZE - 1)], false);
        operateADCBufferLength--;
    }
}

void operateDo(uint16_t channels, bool needA)
{    
    ADC_RAW     _adc;
    SEQUENCE_CHANNELS _seqCommand;

    if (operateMode == OPERATE_COMMAND) {
        operateFillSequencer(channels, &_seqCommand);
        operateReadChannels(&_seqCommand, &_adc);
        operateSendOneSample(&_seqCommand, &_adc, needA);
    }
}
