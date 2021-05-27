/*
 * File:   command.c
 * Author: DAT
 *
 * Created on 14 ?????? 2020 ?., 12:27
 */

#include "command.h"

char cmdBuffer[CMD_BUFFER_SIZE_MAX];
char cmdElement[CMD_BUFFER_SIZE_MAX];

const char CMD_STR_GETVERSION[]                 = "V";
const char CMD_STR_SETCOMMANDMODE[]             = "C";
const char CMD_STR_SETDATAMODE[]                = "D";
const char CMD_STR_READCHANNELSASCII[]          = "RA";
const char CMD_STR_READCHANNELSHEX[]            = "RH";
const char CMD_STR_RESETSETTINGSTOFACTORY[]     = "SF";
const char CMD_STR_READSETTINGSFROMEEPROM[]     = "GE";
#ifndef CMD_VERSION_EASY
//const char CMD_STR_SELECTINPUTVOLTAGE[]         = "SV";
#endif
const char CMD_STR_SETSAMPLINGRATE[]            = "SR";
const char CMD_STR_SELECTSAMPLINGMODE[]         = "SM";
const char CMD_STR_ENABLECHANNELS[]             = "SC";
const char CMD_STR_SELECTDATAFORMAT[]           = "SD";
// New commands
const char CMD_STR_READCURRENTSETTINGS[]        = "GC";
const char CMD_STR_STORETOEEPROM[]              = "SE";
const char CMD_STR_RESTOREFROMEEPROM[]          = "FE";
const char CMD_STR_SETA[]                       = "SA";
const char CMD_STR_SETBP[]                      = "SBP";
const char CMD_STR_SETBN[]                      = "SBN";
const char CMD_STR_SETAF[]                      = "FA";
const char CMD_STR_SETBPF[]                     = "FBP";
const char CMD_STR_SETBNF[]                     = "FBN";

const char CMD_ANSWER_OK[]                      = "A";
const char CMD_ANSWER_ERROR_SYNTAX[]            = "C";
const char CMD_ANSWER_ERROR_OUTRANGE[]          = "O";
const char CMD_ANSWER_ERROR_EXECUTION[]         = "F";
#ifndef CMD_VERSION_EASY
const char CMD_ANSWER_VERSION[]                 = "Tibbo Inc. Tibbit#43-2 FW1.2";
#else
const char CMD_ANSWER_VERSION[]                 = "Tibbo Inc. Tibbit#43-1 FW1.1";
#endif

typedef enum {
    TYPE_ERROR,
    TYPE_DELIMETER,
    TYPE_END_COMMAND,
    TYPE_DIGIT,
    TYPE_LETTER
} CMD_ELEMENT_TYPE;

void cmdSendAnswer(char* answer, bool needA)
{
    uint8_t _lenAnswer = strlen(answer);
    
    if (_lenAnswer == 0) return;
    
    EUSART_Write(CMD_STX);
    if (needA)
        EUSART_Write(CMD_ANSW_OK);
        
    for (uint8_t _i = 0; _i < _lenAnswer; _i++) {
        EUSART_Write(answer[_i]);
    }
    EUSART_Write(CMD_CR);
}

inline CMD_COMMAND cmdGetCommand(char* element)
{
    if (!strcmp(element, CMD_STR_GETVERSION))             return CMD_GETVERSION;
    if (!strcmp(element, CMD_STR_SETCOMMANDMODE))         return CMD_SETCOMMANDMODE;
    if (!strcmp(element, CMD_STR_SETDATAMODE))            return CMD_SETDATAMODE;
    if (!strcmp(element, CMD_STR_READCHANNELSASCII))      return CMD_READCHANNELSASCII;
    if (!strcmp(element, CMD_STR_READCHANNELSHEX))        return CMD_READCHANNELSHEX;
    if (!strcmp(element, CMD_STR_RESETSETTINGSTOFACTORY)) return CMD_RESETSETTINGSTOFACTORY;
    if (!strcmp(element, CMD_STR_READSETTINGSFROMEEPROM)) return CMD_READSETTINGSFROMEEPROM;
#ifndef CMD_VERSION_EASY
    //if (!strcmp(element, CMD_STR_SELECTINPUTVOLTAGE))     return CMD_SELECTINPUTVOLTAGE;
#endif
    if (!strcmp(element, CMD_STR_SETSAMPLINGRATE))        return CMD_SETSAMPLINGRATE;
    if (!strcmp(element, CMD_STR_SELECTSAMPLINGMODE))     return CMD_SELECTSAMPLINGMODE;
    if (!strcmp(element, CMD_STR_ENABLECHANNELS))         return CMD_ENABLECHANNELS;
    if (!strcmp(element, CMD_STR_SELECTDATAFORMAT))       return CMD_SELECTDATAFORMAT;
    if (!strcmp(element, CMD_STR_READCURRENTSETTINGS))    return CMD_READSETTINGSCURRENT;
    if (!strcmp(element, CMD_STR_STORETOEEPROM))          return CMD_STORETOEEPROM;
    if (!strcmp(element, CMD_STR_RESTOREFROMEEPROM))      return CMD_RESTOREFROMEEPROM;
    if (!strcmp(element, CMD_STR_SETA))                   return CMD_SETA;
    if (!strcmp(element, CMD_STR_SETBP))                  return CMD_SETBP;
    if (!strcmp(element, CMD_STR_SETBN))                  return CMD_SETBN;
    if (!strcmp(element, CMD_STR_SETAF))                  return CMD_SETAF;
    if (!strcmp(element, CMD_STR_SETBPF))                 return CMD_SETBPF;
    if (!strcmp(element, CMD_STR_SETBNF))                 return CMD_SETBNF;

    return CMD_UNKNOWN;
}

uint8_t cmdElementsNumber = 0;
int16_t cmdParams[6];
uint8_t cmdParamsNumber = 0;
CMD_COMMAND cmdCommand;

inline bool cmdCheckCharIsEndCommand(char symbol)
{
    if (symbol == 0x00) return true;
    if (symbol == CMD_CR) return true;
    return false;
}

inline bool cmdCheckCharIsDigit(char symbol)
{
    if ((symbol >= 0x30) && (symbol <= 0x39)) return true;
    return false;
}

inline char cmdLetterToCapital(char symbol)
{
    return symbol & 0x5f;
}

inline bool cmdCheckCharIsLetter(char symbol)
{
    if ((symbol >= 0x41) && (symbol <= 0x5a)) return true;
    if ((symbol >= 0x61) && (symbol <= 0x7a)) return true;
    return false;
}

inline bool cmdCheckCharIsDelimiter(char symbol)
{
    if (symbol == ',') return true;
    return false;
}

inline bool cmdCheckCharIsSign(char symbol)
{
    if (symbol == '-') return true;
    if (symbol == '+') return true;
    return false;
}

inline bool cmdPointerInc(uint8_t* pPointer)
{
    (*pPointer)++;
    if (*pPointer >= cmdBufPointer) return false;
    return true;
}

// Copy one lexical element from cmdBuffer to cmdElement from start position POINTER.
// Length of element is returned to length variable and type of element returned
inline CMD_ELEMENT_TYPE cmdGetElement(uint8_t pointer, uint8_t* length)
{
    uint8_t _length = 0;
    
    if (pointer >= CMD_BUFFER_SIZE_MAX) return TYPE_ERROR;
    if (cmdCheckCharIsEndCommand(cmdBuffer[pointer])) return TYPE_END_COMMAND;
    
    // Check delimeter
    if (cmdCheckCharIsDelimiter(cmdBuffer[pointer])) {
        *length = 0x01;
        return TYPE_DELIMETER;
    }

    // Check Digits
    if (cmdCheckCharIsDigit(cmdBuffer[pointer]) || cmdCheckCharIsSign(cmdBuffer[pointer])) {
        cmdElement[_length++] = cmdBuffer[pointer];
        if (!cmdPointerInc(&pointer)) return TYPE_ERROR;
        while (cmdCheckCharIsDigit(cmdBuffer[pointer])) {
            cmdElement[_length++] = cmdBuffer[pointer];
            if (!cmdPointerInc(&pointer)) return TYPE_ERROR;
        }
        *length = _length;
        cmdElement[_length] = 0x00;
        return TYPE_DIGIT;
    }
    
    // Check Letters
    if (cmdCheckCharIsLetter(cmdBuffer[pointer])) {
        while (cmdCheckCharIsLetter(cmdBuffer[pointer])) {
            cmdElement[_length++] = cmdLetterToCapital(cmdBuffer[pointer]);
            if (!cmdPointerInc(&pointer)) return TYPE_ERROR;
        }
        *length = _length;
        cmdElement[_length] = 0x00;
        return TYPE_LETTER;
    }
    
    return TYPE_ERROR;
}

PARSE_RESULT cmdParse(void)
{
    uint8_t _parsePointer = 0;
    uint8_t _elementLength;
    CMD_ELEMENT_TYPE _elementType;
    
    cmdElementsNumber = 0;
    cmdParamsNumber = 0;
    cmdCommand = CMD_UNKNOWN;
    
    do {
        if (_parsePointer == 0) {
            if (cmdBuffer[_parsePointer] != CMD_STX) {
                // Wrong command
                return PARSE_SYNTAX;
            } else _parsePointer++;
        }
        
        _elementType = cmdGetElement(_parsePointer, &_elementLength);
        switch (_elementType) {
            case TYPE_ERROR:
                return PARSE_SYNTAX;
                break;

            case TYPE_DELIMETER:
                if ((cmdElementsNumber != 2) &&
                    (cmdElementsNumber != 4) &&
                    (cmdElementsNumber != 6) &&
                    (cmdElementsNumber != 8) &&
                    (cmdElementsNumber != 10)) return PARSE_SYNTAX;
                if ((cmdCommand != CMD_ENABLECHANNELS) &&
                    (cmdCommand != CMD_SETA) &&
                    (cmdCommand != CMD_SETBP) &&
                    (cmdCommand != CMD_SETBN) &&
                    (cmdCommand != CMD_SETAF) &&
                    (cmdCommand != CMD_SETBPF) &&
                    (cmdCommand != CMD_SETBNF) &&
                    (cmdCommand != CMD_READCHANNELSASCII) &&
                    (cmdCommand != CMD_READCHANNELSHEX)) return PARSE_SYNTAX;
                
                cmdElementsNumber++;
                _parsePointer += _elementLength;
                
                break;

            case TYPE_DIGIT:
                if ((cmdElementsNumber != 1) &&
                    (cmdElementsNumber != 3) &&
                    (cmdElementsNumber != 5) &&
                    (cmdElementsNumber != 7) &&
                    (cmdElementsNumber != 9) &&
                    (cmdElementsNumber != 11)) return PARSE_SYNTAX;
#ifndef CMD_VERSION_EASY
                if ((cmdCommand != CMD_ENABLECHANNELS) &&
                    //(cmdCommand != CMD_SELECTINPUTVOLTAGE) &&
                    (cmdCommand != CMD_SETSAMPLINGRATE) &&
                    (cmdCommand != CMD_SELECTSAMPLINGMODE) &&
                    (cmdCommand != CMD_SELECTDATAFORMAT) &&
                    (cmdCommand != CMD_READCHANNELSASCII) &&
                    (cmdCommand != CMD_READCHANNELSHEX) &&
                    (cmdCommand != CMD_SETA) &&
                    (cmdCommand != CMD_SETBP) &&
                    (cmdCommand != CMD_SETBN) &&
                    (cmdCommand != CMD_SETAF) &&
                    (cmdCommand != CMD_SETBPF) &&
                    (cmdCommand != CMD_SETBNF)) return PARSE_SYNTAX;
#else
                if ((cmdCommand != CMD_ENABLECHANNELS) &&
                    (cmdCommand != CMD_SETSAMPLINGRATE) &&
                    (cmdCommand != CMD_SELECTSAMPLINGMODE) &&
                    (cmdCommand != CMD_SELECTDATAFORMAT) &&
                    (cmdCommand != CMD_READCHANNELSASCII) &&
                    (cmdCommand != CMD_READCHANNELSHEX) &&
                    (cmdCommand != CMD_SETA) &&
                    (cmdCommand != CMD_SETBP) &&
                    (cmdCommand != CMD_SETBN) &&
                    (cmdCommand != CMD_SETAF) &&
                    (cmdCommand != CMD_SETBPF) &&
                    (cmdCommand != CMD_SETBNF)) return PARSE_SYNTAX;
#endif
#ifndef CMD_VERSION_EASY
                //if ((cmdCommand == CMD_SELECTINPUTVOLTAGE) && (cmdElementsNumber > 1)) return false;
#endif
                if ((cmdCommand == CMD_SETSAMPLINGRATE) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SELECTSAMPLINGMODE) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SELECTDATAFORMAT) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETA) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBP) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBN) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETAF) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBPF) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBNF) && (cmdElementsNumber > 12)) return PARSE_SYNTAX;

                cmdParams[cmdParamsNumber] = atoi(cmdElement);
                
#ifndef CMD_VERSION_EASY
                //if ((cmdCommand == CMD_SELECTINPUTVOLTAGE) && ((cmdParams[cmdParamsNumber] > 4) || (cmdParams[cmdParamsNumber] < 0))) return false;
#endif
                if ((cmdCommand == CMD_SETSAMPLINGRATE) && ((cmdParams[cmdParamsNumber] < 1) || 
                    (cmdParams[cmdParamsNumber] > 1000))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SELECTSAMPLINGMODE) && ((cmdParams[cmdParamsNumber] > 1) || (cmdParams[cmdParamsNumber] < 0))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SELECTDATAFORMAT) && ((cmdParams[cmdParamsNumber] > 2) || (cmdParams[cmdParamsNumber] < 0))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_READCHANNELSASCII) && ((cmdParams[cmdParamsNumber] < 1) ||
                    (cmdParams[cmdParamsNumber] > 4))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_READCHANNELSHEX) && ((cmdParams[cmdParamsNumber] < 1) ||
                    (cmdParams[cmdParamsNumber] > 4))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_ENABLECHANNELS) && ((cmdParams[cmdParamsNumber] < 1) ||
                    (cmdParams[cmdParamsNumber] > 4))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETA) && ((cmdParams[cmdParamsNumber] < 1) ||
                    (cmdParams[cmdParamsNumber] > 255))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETBP) && ((cmdParams[cmdParamsNumber] < -128) ||
                    (cmdParams[cmdParamsNumber] > 127))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETBN) && ((cmdParams[cmdParamsNumber] < -128) ||
                    (cmdParams[cmdParamsNumber] > 127))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETAF) && ((cmdParams[cmdParamsNumber] < 1) ||
                    (cmdParams[cmdParamsNumber] > 255))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETBPF) && ((cmdParams[cmdParamsNumber] < -128) ||
                    (cmdParams[cmdParamsNumber] > 127))) return PARSE_OUTRANGE;
                if ((cmdCommand == CMD_SETBNF) && ((cmdParams[cmdParamsNumber] < -128) ||
                    (cmdParams[cmdParamsNumber] > 127))) return PARSE_OUTRANGE;

                cmdElementsNumber++;
                cmdParamsNumber++;
                _parsePointer += _elementLength;
                
                break;

            case TYPE_LETTER:
                if (cmdElementsNumber > 0) return PARSE_SYNTAX;
                // First element is Letter
                if (_elementLength > 3) return PARSE_SYNTAX; // All commands only have 1-2-3 symbols
                cmdCommand = cmdGetCommand(cmdElement);
                if (cmdCommand == CMD_UNKNOWN) return PARSE_SYNTAX;
                cmdElementsNumber++;
                _parsePointer += _elementLength;
                break;

            case TYPE_END_COMMAND:
                if ((cmdCommand == CMD_GETVERSION) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETCOMMANDMODE) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETDATAMODE) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_READCHANNELSASCII) && !((cmdElementsNumber == 2) || (cmdElementsNumber == 4) || (cmdElementsNumber == 6) || (cmdElementsNumber == 8))) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_READCHANNELSHEX) && !((cmdElementsNumber == 2) || (cmdElementsNumber == 4) || (cmdElementsNumber == 6) || (cmdElementsNumber == 8))) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_RESETSETTINGSTOFACTORY) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_READSETTINGSFROMEEPROM) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
#ifndef CMD_VERSION_EASY
                //if ((cmdCommand == CMD_SELECTINPUTVOLTAGE) && (cmdElementsNumber != 2)) return false;
#endif
                if ((cmdCommand == CMD_SETSAMPLINGRATE) && (cmdElementsNumber != 2)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SELECTSAMPLINGMODE) && (cmdElementsNumber != 2)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_ENABLECHANNELS) && !((cmdElementsNumber == 2) || (cmdElementsNumber == 4) || (cmdElementsNumber == 6) || (cmdElementsNumber == 8))) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SELECTDATAFORMAT) && (cmdElementsNumber != 2)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_READSETTINGSCURRENT) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_STORETOEEPROM) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_RESTOREFROMEEPROM) && (cmdElementsNumber > 1)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETA) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBP) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBN) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETAF) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBPF) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                if ((cmdCommand == CMD_SETBNF) && (cmdElementsNumber != 12)) return PARSE_SYNTAX;
                break;

            default:
                break;

        }
    } while (_elementType != TYPE_END_COMMAND);

    return PARSE_OK;
}

PARSE_RESULT cmdCheckChannels()
{
    if (!cmdParamsNumber) return PARSE_SYNTAX;
    if (operateGetSample() == OPERATE_SAMPLE_SINGLE) {
        if (cmdParamsNumber == 1) return PARSE_OK;
        // channels difference check
        for (uint8_t _i = 0; _i < cmdParamsNumber - 1; _i++)
            for (uint8_t _j = _i + 1; _j < cmdParamsNumber; _j++)
                if (cmdParams[_i] == cmdParams[_j]) return PARSE_SYNTAX;
        return PARSE_OK;
    } else {
        if (cmdParamsNumber > 2) return PARSE_OUTRANGE;
        // Check number channels
        if (cmdParams[0] > 2) return PARSE_OUTRANGE;
        if (cmdParamsNumber == 2) {
            if (cmdParams[1] > 2) return PARSE_OUTRANGE;
            if (cmdParams[0] == cmdParams[1]) return PARSE_SYNTAX;
            return PARSE_OK;
        } else return PARSE_OK;
    }
    return PARSE_SYNTAX;
}

uint16_t cmdGetChannels()
{
    uint16_t _channels = 0;
    
    if (cmdParamsNumber == 0)
        if (operateGetSample() == OPERATE_SAMPLE_SINGLE) {
            return 0x03e4;
        } else return 0x0104;

    for (uint8_t _i = 0; _i < cmdParamsNumber; _i++) {
        _channels |= (cmdParams[_i] - 1) << (_i*2);
    }

    _channels |= ((uint16_t)cmdParamsNumber - 1) << 8;
    
    return _channels;
}

void cmdSendInteger(uint16_t value)
{
    char _sendData[5];
    if (value < 1001) {
        sprintf(_sendData, "%d", value);
        for (uint8_t _i = 0; _i < strlen(_sendData); _i++)
            EUSART_Write(_sendData[_i]);
    } else EUSART_Write('0');
}

void cmdSendIntegerSign(int8_t value)
{
    char _sendData[5];
        sprintf(_sendData, "%d", value);
        for (uint8_t _i = 0; _i < strlen(_sendData); _i++)
            EUSART_Write(_sendData[_i]);
}

void cmdSendSettingsFrom(bool fromEEPROM) // true - from EEPROM, false - from RAM
{
    uint16_t _ch;
    uint8_t _cnt;
    
    EUSART_Write(CMD_STX);
    EUSART_Write(CMD_ANSW_OK);
    
#ifndef CMD_VERSION_EASY
    //EUSART_Write('S');
    //EUSART_Write('V');
    //if (fromEEPROM)
    //    cmdSendInteger(eepromStoreConfig.eeprom_Voltage);
    //else
    //    cmdSendInteger(operateGetVoltage());
    //EUSART_Write(';');
#endif
    EUSART_Write('S');
    EUSART_Write('R');
    if (fromEEPROM)
        cmdSendInteger(eepromStoreConfig.StoreV1.eeprom_SampleRate);
    else
        cmdSendInteger(operateGetRate());
    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('M');
    if (fromEEPROM)
        cmdSendInteger(eepromStoreConfig.StoreV1.eeprom_SampleMode);
    else
        cmdSendInteger(operateGetSample());
    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('C');
    if (fromEEPROM)
        _ch = eepromStoreConfig.StoreV1.eeprom_Channels;
    else
        _ch = operateGetChannels();
    _cnt = ((_ch & 0x0300) >> 8) + 1;
    for (uint8_t _i = 0; _i < _cnt; _i++) {
        EUSART_Write((_ch & 0x03) + '1');

        _ch = _ch >> 2;

        if (_i < _cnt - 1)
            EUSART_Write(',');
    }

    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('D');
    if (fromEEPROM)
        cmdSendInteger(eepromStoreConfig.StoreV1.eeprom_DataMode);
    else
        cmdSendInteger(operateGetData());
    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('A');
    for (_cnt = 0; _cnt < 6; _cnt++) {
        if (fromEEPROM)
            if (_cnt < 4)
                cmdSendInteger(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstA[_cnt]);
            else
                cmdSendInteger(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffA[_cnt - 4]);
        else
            cmdSendInteger(operateGetA(_cnt));

        if (_cnt != 5)
            EUSART_Write(',');
    }
    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('B');
    EUSART_Write('P');
    for (_cnt = 0; _cnt < 6; _cnt++) {
        if (fromEEPROM)
            if (_cnt < 4)
                cmdSendIntegerSign(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBP[_cnt]);
            else
                cmdSendIntegerSign(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBP[_cnt - 4]);
        else
            cmdSendIntegerSign(operateGetBP(_cnt));

        if (_cnt != 5)
            EUSART_Write(',');
    }
    EUSART_Write(';');
    EUSART_Write('S');
    EUSART_Write('B');
    EUSART_Write('N');
    for (_cnt = 0; _cnt < 6; _cnt++) {
        if (fromEEPROM)
            if (_cnt < 4)
                cmdSendIntegerSign(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBN[_cnt]);
            else
                cmdSendIntegerSign(eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBN[_cnt - 4]);
        else
            cmdSendIntegerSign(operateGetBN(_cnt));

        if (_cnt != 5)
            EUSART_Write(',');
    }
    EUSART_Write(CMD_CR);
}

void cmdDoCommand()
{
    switch (cmdCommand) {
        case CMD_GETVERSION:                        //
            if (operateGetMode() == OPERATE_COMMAND)
                cmdSendAnswer((char*)CMD_ANSWER_VERSION, true); 
            break;
            
        case CMD_SETCOMMANDMODE:                    //
            operateSetMode(OPERATE_COMMAND);
            cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            break;
            
        case CMD_SETDATAMODE:                       //
            operateSetMode(OPERATE_DATA);
            break;
            
        case CMD_READCHANNELSASCII:                      // 
            if (operateGetMode() == OPERATE_COMMAND) {
                switch (cmdCheckChannels()) {
                case PARSE_OK:
                    operateFirstInitADC = true;
                    OPERATE_DATA_MODE _mode = operateGetData();
                    operateSetData(OPERATE_DATA_ASCII);
                    operateDo(cmdGetChannels(), true);
                    operateSetData(_mode);
                    break;
                    
                case PARSE_SYNTAX:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                    break;
                    
                case PARSE_OUTRANGE:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_OUTRANGE, false);
                    break;
                    
                default:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                }
            }
            break;
            
        case CMD_READCHANNELSHEX:                      // 
            if (operateGetMode() == OPERATE_COMMAND) {
                switch (cmdCheckChannels()){
                case PARSE_OK:
                    operateFirstInitADC = true;
                    OPERATE_DATA_MODE _mode = operateGetData();
                    operateSetData(OPERATE_DATA_HEX);
                    operateDo(cmdGetChannels(), true);
                    operateSetData(_mode);
                    break;
                    
                case PARSE_SYNTAX:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                    break;
                    
                case PARSE_OUTRANGE:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_OUTRANGE, false);
                    break;
                    
                default:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                }
            }
            break;
            
        case CMD_RESETSETTINGSTOFACTORY:            // 
            if (operateGetMode() == OPERATE_COMMAND) {
                operateSetFactory();
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_READSETTINGSFROMEEPROM:            // 
            if (operateGetMode() == OPERATE_COMMAND) {
                cmdSendSettingsFrom(true);
            }
            break;
            
#ifndef CMD_VERSION_EASY
/*      case CMD_SELECTINPUTVOLTAGE:                // 
            if (operateGetMode() == OPERATE_COMMAND) {
                if ((cmdParams[0] == OPERATE_V_AUTO) && (operateGetData() != OPERATE_DATA_ASCII)) {
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR);
                } else {
                    operateSetVoltage(cmdParams[0]);
                    cmdSendAnswer((char*)CMD_ANSWER_OK);
                }
            }
            break;  */
#endif
            
        case CMD_SETSAMPLINGRATE:                   // 
            if (operateGetMode() == OPERATE_COMMAND) {
                operateSetRate(cmdParams[0]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_SELECTSAMPLINGMODE:                // 
            if (operateGetMode() == OPERATE_COMMAND) {
                operateSetSample(cmdParams[0]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_ENABLECHANNELS:                    // 
            if (operateGetMode() == OPERATE_COMMAND) {
                switch (cmdCheckChannels()) {
                case PARSE_OK:
                    operateSetChannels(cmdGetChannels());
                    cmdSendAnswer((char*)CMD_ANSWER_OK, false);
                    break;
                    
                case PARSE_SYNTAX:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                    break;
                    
                case PARSE_OUTRANGE:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_OUTRANGE, false);
                    break;
                    
                default:
                    cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                }
            }
            break;
            
        case CMD_SELECTDATAFORMAT:                  // 
            if (operateGetMode() == OPERATE_COMMAND) {
#ifndef CMD_VERSION_EASY
                //if ((cmdParams[0] != OPERATE_DATA_ASCII) && (operateGetVoltage() == OPERATE_V_AUTO)) {
                //    cmdSendAnswer((char*)CMD_ANSWER_ERROR);
                //} else {
                    operateSetData(cmdParams[0]);
                    cmdSendAnswer((char*)CMD_ANSWER_OK, false);
                //}
#else
                operateSetData(cmdParams[0]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
#endif
            }
            break;
            
        case CMD_READSETTINGSCURRENT:
            if (operateGetMode() == OPERATE_COMMAND) {
                cmdSendSettingsFrom(false);
            }
            break;
            
        case CMD_STORETOEEPROM:
            if (operateGetMode() == OPERATE_COMMAND) {
                operateStoreToEEPROM();
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_RESTOREFROMEEPROM:
            if (operateGetMode() == OPERATE_COMMAND) {
                operateSetSample(eepromStoreConfig.StoreV1.eeprom_SampleMode);
                operateSetData(eepromStoreConfig.StoreV1.eeprom_DataMode);
#ifndef CMD_VERSION_EASY
                //operateSetVoltage(eepromStoreConfig.eeprom_Voltage);
#endif
                operateSetRate(eepromStoreConfig.StoreV1.eeprom_SampleRate);
                operateSetChannels(eepromStoreConfig.StoreV1.eeprom_Channels);
                for (uint8_t _i = 0; _i < 4; _i++)  {
                    operateSetA(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstA[_i]);
                    operateSetBP(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBP[_i]);
                    operateSetBN(_i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstBN[_i]);
                }
                for (uint8_t _i = 0; _i < 2; _i++)  {
                    operateSetA(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffA[_i]);
                    operateSetBP(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBP[_i]);
                    operateSetBN(4 + _i, eepromStoreConfig.StoreV1.CalibrationV1.eeprom_ConstDiffBN[_i]);
                }
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_SETA:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetA(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_SETBP:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetBP(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;

        case CMD_SETBN:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetBN(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;

        case CMD_SETAF:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetAF(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;
            
        case CMD_SETBPF:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetBPF(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;

        case CMD_SETBNF:
            if (operateGetMode() == OPERATE_COMMAND) {
                for (uint8_t _i = 0; _i < 6; _i++)
                    operateSetBNF(_i, cmdParams[_i]);
                cmdSendAnswer((char*)CMD_ANSWER_OK, false);
            }
            break;

        default:
            // Send fault response
            if (operateGetMode() == OPERATE_COMMAND)
                cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
    }
}

void cmdReceiveOperate(void)
{
    uint8_t _usartData;
    
    if (EUSART_is_rx_ready()) {
        _usartData = EUSART_Read();
        
        if (_usartData == CMD_STX)
            cmdBufPointer = 0;
            
        if (cmdBufPointer == CMD_BUFFER_SIZE_MAX) {
            // Command buffer full
            if (_usartData == CMD_CR) {
                // End fault command
                cmdBufPointer = 0;
                // Send fault response
                cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
            }
        } else {
            cmdBuffer[cmdBufPointer++] = _usartData;
            if (_usartData == CMD_CR) {
                // End command. Need parse

                switch (cmdParse()) {
                    case PARSE_OK:
                        // Parse complete successfully
                        cmdDoCommand();
                        break;

                    case PARSE_SYNTAX:                        
                        // Send fault syntax response
                        cmdSendAnswer((char*)CMD_ANSWER_ERROR_SYNTAX, false);
                        break;
                        
                    case PARSE_OUTRANGE:
                        // Send fault out range response
                        cmdSendAnswer((char*)CMD_ANSWER_ERROR_OUTRANGE, false);
                        break;
                }
                cmdBufPointer = 0;
            }
        }
    }
}
