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
 *    5.  uint16_t msd_reader::ls(LsAction Action, const char *path = "", const char * const match = NULL);
 *    6.  uint16_t msd_reader::lsDive(LsAction Action, const char *path = "", const char * const match = NULL);
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

enum LsAction : uint8_t { LS_SerialPrint, LS_Count, LS_GetFilename };

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
  uint16_t ls(LsAction Action, const char *path = "", const char * const match = NULL);
  uint16_t lsDive(const char *path = "", const char * const match = NULL);
  uint16_t get_num_Files(const char *path = "", const char * const match = NULL);
  void test_code(void);

private:
  //Variable definitions
  FATFS fatFS;	/* File system object */
  bool detected;
  bool Initialized;;
  LsAction lsAction;
  uint8_t file_count;
};

extern msd_reader MsdReader;
#endif // USBMSCSUPPORT
#endif // TARGET_LPC1768
#endif // _MSD_READER_H_
