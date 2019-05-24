/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   msd_reader
 * \brief   USB HOST mass storge driver.
 * @file    msd_reader.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for USB HOST mass storge driver.
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
 * This file is a drive for usb host mass storge.
 *
 * \par Method List:
 *
 *    1.  void    msd_reader::init(void);
 *    2.  bool    msd_reader::is_usb_detected(void);
 *    3.  void    msd_reader::usb_status_polling(void);
 *    4.  void    msd_reader::test_code(void);
 *    5.  uint16_t msd_reader::ls(is_action_t action, const char *path = "", const char * const match = NULL);
 *    6.  uint16_t msd_reader::is_dive(const char *path = "", const char * const match = NULL);
 *    7.  uint16_t msd_reader::get_num_Files(const char *path = "", const char * const match = NULL);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _MSD_READER_H_
#define _MSD_READER_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(USBMSCSUPPORT)
#include "MassStorageLib.h"
#include "../../gcode/lcd_file.h"

enum is_action_t : uint8_t { LS_SERIAL_PRINT, LS_COUNT, LS_GET_FILE_NAME };

#define USB_NOT_DETECTED 100;

class msd_reader
{
public:

  msd_reader(void);
  virtual ~msd_reader(void) { }

  void init(void);
  bool is_usb_detected(void);
  bool is_usb_Initialized(void);
  void usb_status_polling(void);
  uint16_t ls(is_action_t action, const char *path = "", const char * const match = NULL);
  uint16_t ls_dive(const char *path = "", const char * const match = NULL);
  uint16_t get_num_Files(const char *path = "", const char * const match = NULL);
  void get(void);
  inline bool eof() { return udisk_pos >= file_size; }
  void start_file_print(void);
  void stop_udisk_Print(void);
  void test_code(void);
  bool get_udisk_printing_flag(void);
private:
  //Variable definitions
  FATFS fatFS;	/* File system object */

  bool detected;
  bool Initialized;
  bool udisk_printing;
  bool abort_udisk_printing;

  is_action_t is_action;

  uint16_t file_count;  //counter for the files in the current directory

  uint32_t udisk_pos;
  uint32_t file_size;
  lcd_file file;
};

#define IS_UDISK_PRINTING() udisk.get_udisk_printing_flag()
#define IS_UDISK_FILE_OPEN()

extern msd_reader udisk;
#endif // USBMSCSUPPORT
#endif // TARGET_LPC1768
#endif // _MSD_READER_H_
