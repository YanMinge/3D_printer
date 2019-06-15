/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   machine_uuid
 * \brief   Header for machine_uuid.cpp.
 * @file    machine_uuid.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/15
 * @brief   Header file for management machine uuid.
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
 * This file is used for management machine uuid.
 *
 * \par Method List:
 *
 *    1.  void    machine_uuid::send_uuid_string(void);
 *    2.  void    machine_uuid::print_info(void);
 *    3.  bool    machine_uuid::set_uuid_from_str(char* string);
 *    4.  void    machine_uuid::set_uuid(uint8_t *uuid);
 *    5.  uint8_t*    machine_uuid::get_uuid(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _MACHINE_UUID_H_
#define _MACHINE_UUID_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(FACTORY_MACHINE_UUID)
class machine_uuid
{
public:

  machine_uuid(void);
  void send_uuid_string(void);
  void print_info(void);
  bool set_uuid_from_str(char* string);
  void set_uuid(uint8_t *uuid);
  uint8_t* get_uuid(void);

private:
  uint8_t factory_uuid[8];
};

extern machine_uuid MachineUuid;
#endif // FACTORY_MACHINE_UUID
#endif // TARGET_LPC1768
#endif // _MACHINE_UUID_H_
