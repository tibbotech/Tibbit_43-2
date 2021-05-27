/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F1825
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA0 procedures
#define RA0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define RA0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define RA0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define RA0_GetValue()              PORTAbits.RA0
#define RA0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define RA0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define RA0_SetPullup()             do { WPUAbits.WPUA0 = 1; } while(0)
#define RA0_ResetPullup()           do { WPUAbits.WPUA0 = 0; } while(0)
#define RA0_SetAnalogMode()         do { ANSELAbits.ANSA0 = 1; } while(0)
#define RA0_SetDigitalMode()        do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set RA1 procedures
#define RA1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define RA1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define RA1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define RA1_GetValue()              PORTAbits.RA1
#define RA1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define RA1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define RA1_SetPullup()             do { WPUAbits.WPUA1 = 1; } while(0)
#define RA1_ResetPullup()           do { WPUAbits.WPUA1 = 0; } while(0)
#define RA1_SetAnalogMode()         do { ANSELAbits.ANSA1 = 1; } while(0)
#define RA1_SetDigitalMode()        do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set RTS aliases
#define RTS_TRIS                 TRISAbits.TRISA2
#define RTS_LAT                  LATAbits.LATA2
#define RTS_PORT                 PORTAbits.RA2
#define RTS_WPU                  WPUAbits.WPUA2
#define RTS_ANS                  ANSELAbits.ANSA2
#define RTS_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RTS_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RTS_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RTS_GetValue()           PORTAbits.RA2
#define RTS_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RTS_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RTS_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define RTS_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define RTS_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define RTS_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set SDO aliases
#define SDO_TRIS                 TRISAbits.TRISA4
#define SDO_LAT                  LATAbits.LATA4
#define SDO_PORT                 PORTAbits.RA4
#define SDO_WPU                  WPUAbits.WPUA4
#define SDO_ANS                  ANSELAbits.ANSA4
#define SDO_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define SDO_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define SDO_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define SDO_GetValue()           PORTAbits.RA4
#define SDO_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define SDO_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define SDO_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define SDO_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define SDO_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define SDO_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set SCK aliases
#define SCK_TRIS                 TRISCbits.TRISC0
#define SCK_LAT                  LATCbits.LATC0
#define SCK_PORT                 PORTCbits.RC0
#define SCK_WPU                  WPUCbits.WPUC0
#define SCK_ANS                  ANSELCbits.ANSC0
#define SCK_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define SCK_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define SCK_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define SCK_GetValue()           PORTCbits.RC0
#define SCK_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define SCK_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define SCK_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define SCK_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define SCK_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define SCK_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set SDI aliases
#define SDI_TRIS                 TRISCbits.TRISC1
#define SDI_LAT                  LATCbits.LATC1
#define SDI_PORT                 PORTCbits.RC1
#define SDI_WPU                  WPUCbits.WPUC1
#define SDI_ANS                  ANSELCbits.ANSC1
#define SDI_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define SDI_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define SDI_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define SDI_GetValue()           PORTCbits.RC1
#define SDI_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define SDI_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define SDI_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define SDI_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define SDI_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define SDI_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set AD7323_CS aliases
#define AD7323_CS_TRIS                 TRISCbits.TRISC3
#define AD7323_CS_LAT                  LATCbits.LATC3
#define AD7323_CS_PORT                 PORTCbits.RC3
#define AD7323_CS_WPU                  WPUCbits.WPUC3
#define AD7323_CS_ANS                  ANSELCbits.ANSC3
#define AD7323_CS_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define AD7323_CS_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define AD7323_CS_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define AD7323_CS_GetValue()           PORTCbits.RC3
#define AD7323_CS_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define AD7323_CS_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define AD7323_CS_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define AD7323_CS_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define AD7323_CS_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define AD7323_CS_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set DEBUG_1 aliases
#define DEBUG_1_TRIS                 TRISCbits.TRISC4
#define DEBUG_1_LAT                  LATCbits.LATC4
#define DEBUG_1_PORT                 PORTCbits.RC4
#define DEBUG_1_WPU                  WPUCbits.WPUC4
#define DEBUG_1_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define DEBUG_1_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define DEBUG_1_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define DEBUG_1_GetValue()           PORTCbits.RC4
#define DEBUG_1_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define DEBUG_1_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define DEBUG_1_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define DEBUG_1_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)

// get/set DEBUG_2 aliases
#define DEBUG_2_TRIS                 TRISCbits.TRISC5
#define DEBUG_2_LAT                  LATCbits.LATC5
#define DEBUG_2_PORT                 PORTCbits.RC5
#define DEBUG_2_WPU                  WPUCbits.WPUC5
#define DEBUG_2_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define DEBUG_2_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define DEBUG_2_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define DEBUG_2_GetValue()           PORTCbits.RC5
#define DEBUG_2_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define DEBUG_2_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define DEBUG_2_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define DEBUG_2_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/