/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   PersistentStore
 * \brief   persistent store driver.
 * @file    PersistentStore.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/22
 * @brief   Source code for persistent store driver.
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
 * This file is a drive for persistent store.
 *
 * \par Method List:
 *
 *    1.  bool PersistentStore::access_start(void);
 *    2.  bool PersistentStore::access_finish(void);
 *    3.  bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc);
 *    4.  bool PersistentStore::read_data(int &pos, uint8_t* value, size_t size, uint16_t *crc, const bool writing)
 *    5.  size_t PersistentStore::capacity();
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/22     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768

#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(EEPROM_SETTINGS)

#include "../shared/persistent_store_api.h"
#include HAL_PATH(.., HAL.h)

bool PersistentStore::access_start(void)
{
  return true; 
}

bool PersistentStore::access_finish(void)
{
  return true;
}

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc)
{
  while (size--)
  {
    uint8_t * const p = (uint8_t * const)pos;
    uint8_t v = *value;
    // EEPROM has only ~100,000 write cycles,
    // so only write bytes that have changed!
    if (v != eeprom_read_byte(p))
    {
      eeprom_write_byte(p, v);
      if (eeprom_read_byte(p) != v)
      {
        SERIAL_ECHO_MSG(MSG_ERR_EEPROM_WRITE);
        return true;
      }
    }
    crc16(crc, &v, 1);
    pos++;
    value++;
  };
  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t* value, size_t size, uint16_t *crc, const bool writing)
{
  do
  {
    uint8_t c = eeprom_read_byte((uint8_t*)pos);
    *value = c;
    crc16(crc, &c, 1);
    pos++;
    value++;
  } while (--size);
  return false;
}

size_t PersistentStore::capacity()
{
  return E2END + 1;
}

#define SECTOR_START(sector)  ((sector < 16) ? (sector * 0x1000) : ((sector - 14) * 0x8000))
uint32_t PersistentStore::caculate_flash_crc32(uint32_t file_size)
{
  uint32_t crc_value = 0xffffffff;
  uint8_t *pmem= (uint8_t *)SECTOR_START(16);
  crc_value = crc32(crc_value, pmem, file_size, 1);
  return crc_value;
}

#endif // EEPROM_SETTINGS
#endif // TARGET_LPC1768
