/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \brief   cmd neede to seed to lcd
 * @file    lcd_data.h
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2019/04/29
 * @brief   cmd neede to seed to lcd

 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once
/**
 * lcd_data.h
 */
#include "../../inc/MarlinConfig.h"

/**
 * lcd Command Queue
 * A simple buffer of command strings.
 * Commands are copied into this buffer by the command injectors
 */
extern uint8_t CHANGE_PAGE_2[];
extern uint8_t CHANGE_PAGE_4[];
extern uint8_t CHANGE_PAGE_7[];
extern uint8_t CHANGE_PAGE_8[];

extern uint8_t CHANGE_PAGE_A[];
extern uint8_t CHANGE_PAGE_E[];
extern uint8_t CHANGE_PAGE_11[];
extern uint8_t CHANGE_PAGE_12[];

extern uint8_t CHANGE_ICON_1[];

extern uint8_t PAGE1_0_1[];
extern uint8_t PAGE1_0_2[];
extern uint8_t PAGE1_0_3[];

extern uint8_t PAGE1_1_1[];
extern uint8_t PAGE1_1_2[];

extern uint8_t PAGE2_0_1[];
extern uint8_t PAGE2_0_2[];
extern uint8_t PAGE2_0_3[];
extern uint8_t PAGE2_0_4[];
extern uint8_t PAGE2_0_5[];

extern uint8_t PAGE2_1_1[];
extern uint8_t PAGE2_1_2[];

extern uint8_t PAGE2_2_1[];
extern uint8_t PAGE2_2_2[];
extern uint8_t PAGE2_2_3[];
extern uint8_t PAGE2_2_4[];
extern uint8_t PAGE2_2_5[];

extern uint8_t PAGE2_4_1[];
extern uint8_t PAGE2_4_1_1[];
extern uint8_t PAGE2_4_2[];
extern uint8_t PAGE2_4_3[];
extern uint8_t PAGE2_4_4[];

extern uint8_t PAGE2_5_1[];
extern uint8_t PAGE2_5_2[];

extern uint8_t PAGE2_7_1[];

extern uint8_t PAGE2_8_1[];
extern uint8_t PAGE2_8_2[];
extern uint8_t PAGE2_8_3[];

extern uint8_t PAGE2_9_1[];
extern uint8_t PAGE2_ALL_0[];
extern uint8_t PAGE2_ALL_1[];
extern uint8_t TEST_0[];

extern uint8_t PAGE2_4_0[];
extern uint8_t PAGE2_5_0[];
extern uint8_t PAGE2_9_0[];
extern uint8_t PAGE2_7_0[];

/**
 * crc array
 */
extern const uint8_t lcdCRCH[];
extern const uint8_t lcdCRCL[];

uint16_t usMBCRC16( uint8_t * pucFrame, uint32_t len );

