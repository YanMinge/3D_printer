/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   no
 * \brief   head file for dwin lcd.
 * @file    dwin.h
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   head file for dwin lcd.
 *
 * \par Copyright
 * This software is Copyright (C), 2018-2019, MakeBlock. Use is subject to license \n
 * conditions. The main licensing options available are GPL V2 or Commercial: \n
 *
 * \par Open Source Licensing GPL V2
 * This is the appropriate option if you want to share the source code of your \n
 * application with everyone you distribute it to, and you also want to give them \n
 * the right to share who uses it. If you wish to use this software under Open \n
 * Source Licensing, you must contribute all your source code to the open source \n
 * community in accordance with the GPL Version 2 when your application is \n
 * distributed. See http://www.gnu.org/copyleft/gpl.html
 *
 * \par Description
 * This file is the head file for dwin lcd.
 *
 * \par Method List:
 *
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#if ENABLED(USE_DWIN_LCD)

#ifndef _DWIN_H_
#define _DWIN_H_

#define GET_MAX_INDEX(X) (4*X.page_count + X.last_page_file_count)
//#define DEBUGPRINTF(...)
#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

//marco defines have relation with UI layout
#define PAGE_FILE_NUM   4
#define SEND_IMAGE_LEN  250
#define FILE_NAME_LEN   32
#define FILE_TEXT_LEN   (FILE_NAME_LEN/2)
#define FILE_NUM_MAX    40

#define DATA_BUF_SIZE		10  //lcd_recive_buf_size
#define MAX_SEND_BUF		256
#define FILE_NUM_MAX    40

//marco defines have relation with lcd command
#define	WRITE_REGISTER_ADDR	0x80
#define	READ_REGISTER_ADDR	0x81
#define	WRITE_VARIABLE_ADDR	0x82
#define	READ_VARIABLE_ADDR	0x83

#define HEAD_ONE   (0x5A)
#define HEAD_TWO   (0xA5)
#define TAIL_ONE   (0x4F)
#define TAIL_TWO   (0x4B)

/*************Register and Variable addr*****************/
#define PAGE_BASE	      (unsigned long)0x5A010000
#define PICTURE_DISPLAY_BASE1 (unsigned long)0x5A018000
#define PICTURE_DISPLAY_BASE2 (unsigned long)0x00000000
#define PICTURE_ADDR    0x00A6
#define STOP_MESSAGE    (unsigned long)0xCDA3D6B9
#define START_MESSAGE   (unsigned long)0xBFAACABC

//eeprom varaddr
#define EEPROM_LANGUAGE_ADDR  ((uint8_t*)0x10)

//btn addr
#define MENU_BUTTONS    0x1200
#define SELECT_BUTTONS  0x1202
#define PRINT_BUTTONS   0x1204

#define AXIS_MOVE_BTN   0x120E
#define X_AXIS_MOVE_BTN 0x1210
#define Y_AXIS_MOVE_BTN 0x1212
#define Z_AXIS_MOVE_BTN 0x1214
#define HOME_MOVE_BTN   0x1216
#define LANGUAGE_SET_BTN   0x1218
#define FILEMENT_BTN   	   0x121A

//var addr
#define	PAGE_ADDR	            0x0084
#define MAKEBLOCK_ICON_ADDR   0x1000
#define START_ICON_ADDR       0x1002
#define FILE_ICON_ADDR        0x1004

#define PREPARE_ICON_ADDR       0x1010
#define VOICE_ICON_ADDR         0x1020
#define TEMP_HOTEND_ADDR        0X1300
#define TEMP_HOTEND_TARGET_ADDR 0X1302
#define TEMP_BED_ADDR           0X1304
#define TEMP_BED_TARGET_ADDR    0X1306

#define START_STOP_ICON_ADDR    0X1430

#define FILE_TEXT_ADDR_1 0x1500
#define FILE_TEXT_ADDR_5 0x1540
#define FILE_TEXT_ADDR_9 0x1580
#define FILE_TEXT_ADDR_D 0x15C0

/***************not used text button addr*************/
/*****************************************************
#define MainPagetextaddr   1400--1438  //进料退料、返回复位等文字的地址
#define EnglishPagetextaddr   1440-1460  //英文主几面地址
#define 12page ADDR   1470-1480
#define EnTextAddrD  0x15D0  0x1600//language set text addr
******************************************************/

enum print_status {
  out_printing,on_printing,stop_printing,
};

#endif // USE_DWIN_LCD
#endif // _LCD_FILE_H_