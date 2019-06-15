/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   machine_uuid
 * \brief   management for machine uuid.
 * @file    machine_uuid.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/15
 * @brief   source code for management machine uuid.
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

#ifdef TARGET_LPC1768
#include "../../Marlin.h"

#if ENABLED(FACTORY_MACHINE_UUID)
#include "machine_uuid.h"

machine_uuid MachineUuid;

machine_uuid::machine_uuid(void)
{

}

void machine_uuid::print_info(void)
{
  for(uint8_t i=0; i<8; i++)
  {
    if(i == 3)
    {
      SERIAL_ECHOPGM("-");
    }
    SERIAL_PRINTF("%02x", factory_uuid[i]);
  }
  SERIAL_EOL();
}

void machine_uuid::send_uuid_string(void)
{
  char str_product[7];
  char str_machine[12];
  char str_uuid[18];
  sprintf_P(str_product,"%02x%02x%02x", factory_uuid[0], factory_uuid[1], factory_uuid[2]);
  sprintf_P(str_machine,"-%02x%02x%02x%02x%02x", factory_uuid[3], factory_uuid[4], factory_uuid[5], factory_uuid[6], factory_uuid[7]);
  strcpy(str_uuid, str_product);
  strcat(str_uuid, str_machine);
  SERIAL_PRINTF("%s", str_uuid);
}

bool machine_uuid::set_uuid_from_str(char* string)
{
  char *p = string;
  uint8_t str_len = strlen(string);
  if(str_len == 2 * (sizeof(factory_uuid) / sizeof(factory_uuid[0])))
  {
    uint8_t factory_uuid_temp[8];
    for(uint8_t i=0; i<str_len; i++)
    {
      char c = p[i];
      uint8_t dbyte = 0x00;

      //Convert all to uppercase
      if((c >= 'a') && (c <= 'f'))
      {
        c = c - 32;
      }

      if((c >= '0') && (c <= '9'))
      {
        dbyte = c - '0';
      }
      else if((c >= 'A') && (c <= 'F'))
      {
        dbyte = c - 'A' + 10;
      }
      else
      {
        return false;
      }
      if(i%2 == 0)
      {
        factory_uuid_temp[i/2] = (dbyte << 4) & 0xf0;
      }
      else
      {
        factory_uuid_temp[i/2] += dbyte & 0x0f;
      }
    }
    for(uint8_t i=0; i<8; i++)
    {
      factory_uuid[i] = factory_uuid_temp[i];
    }
  }
  return true;
}

void machine_uuid::set_uuid(uint8_t *uuid)
{
  for(uint8_t i=0; i<8; i++)
  {
    factory_uuid[i] = uuid[i];
  }
}

uint8_t* machine_uuid::get_uuid(void)
{
  return factory_uuid;
}

#endif // FACTORY_MACHINE_UUID
#endif // TARGET_LPC1768
