/*
 * Project: Tibbit #43-1
 * File:    ad7323.h
 * Author:  Dmitry Tarasov
 * License: GPLv3
 * Copyright (c) 2021. Tibbo Technology Inc. 
 * SPDX-License-Identifier: GPL-3.0
 */

#ifndef AD7323_H
#define	AD7323_H

#ifdef	__cplusplus
extern "C" {
#endif

#define AD7323_READ_DATA   0x0000

#define AD7323_WRITE_MODE           0x8000
    
#define AD7323_CONTROL_REGISTER     0x0000
#define AD7323_RANGE_REGISTER       0x2000
#define AD7323_SEQUENCE_REGISTER    0x6000

    // Control register
#define AD7323_CONTROL_3PSEUDO_DIFF_INPUTS  0x0300
#define AD7323_CONTROL_2FULLY_DIFF_INPUTS   0x0200
#define AD7323_CONTROL_2PSEUDO_DIFF_INPUTS  0x0100
#define AD7323_CONTROL_4SINGLE_INPUTS       0x0000
    
#define AD7323_CONTROL_FULL_SHUTDOWN_POWER  0x00c0
#define AD7323_CONTROL_AUTOSHUTDOWN_POWER   0x0080
#define AD7323_CONTROL_AUTOSTANDBY_POWER    0x0040
#define AD7323_CONTROL_NORMAL_POWER         0x0000
    
#define AD7323_CONTROL_EXTERNAL_REFERENCE   0x0000
#define AD7323_CONTROL_INTERNAL_REFERENCE   0x0010
    
#define AD7323_CONTROL_SEQUENCER_NOT_USE    0x0000
#define AD7323_CONTROL_SEQUENCER_USE        0x0004
#define AD7323_CONTROL_SEQUENCER_USE_CONRT  0x0008
    
#define AD7323_CONTROL_CHANNEL_1            0x0000
#define AD7323_CONTROL_CHANNEL_2            0x0400
#define AD7323_CONTROL_CHANNEL_3            0x0800
#define AD7323_CONTROL_CHANNEL_4            0x0c00
    
    // Sequence register
#define AD7323_SEQUENCE_1CHANNEL            0x1000
#define AD7323_SEQUENCE_2CHANNEL            0x0800
#define AD7323_SEQUENCE_3CHANNEL            0x0400
#define AD7323_SEQUENCE_4CHANNEL            0x0200
    
    // Range register
#define AD7323_RANGE_1CHANNEL_10V           0x0000
#define AD7323_RANGE_1CHANNEL_5V            0x0800
#define AD7323_RANGE_1CHANNEL_2_5V          0x1000
#define AD7323_RANGE_1CHANNEL_0_10V         0x1800

#define AD7323_RANGE_2CHANNEL_10V           0x0000
#define AD7323_RANGE_2CHANNEL_5V            0x0200
#define AD7323_RANGE_2CHANNEL_2_5V          0x0400
#define AD7323_RANGE_2CHANNEL_0_10V         0x0600

#define AD7323_RANGE_3CHANNEL_10V           0x0000
#define AD7323_RANGE_3CHANNEL_5V            0x0080
#define AD7323_RANGE_3CHANNEL_2_5V          0x0100
#define AD7323_RANGE_3CHANNEL_0_10V         0x0180

#define AD7323_RANGE_4CHANNEL_10V           0x0000
#define AD7323_RANGE_4CHANNEL_5V            0x0020
#define AD7323_RANGE_4CHANNEL_2_5V          0x0040
#define AD7323_RANGE_4CHANNEL_0_10V         0x0060

#define RANGE_A_10V         0x00
#define RANGE_A_5V          0x40
#define RANGE_A_2_5V        0x80
#define RANGE_A_0_10V       0xc0

#define RANGE_B_10V         0x00
#define RANGE_B_5V          0x10
#define RANGE_B_2_5V        0x20
#define RANGE_B_0_10V       0x30

#define RANGE_C_10V         0x00
#define RANGE_C_5V          0x04
#define RANGE_C_2_5V        0x08
#define RANGE_C_0_10V       0x0c

#define RANGE_D_10V         0x00
#define RANGE_D_5V          0x01
#define RANGE_D_2_5V        0x02
#define RANGE_D_0_10V       0x03

#ifdef	__cplusplus
}
#endif

#endif	/* AD7323_H */

