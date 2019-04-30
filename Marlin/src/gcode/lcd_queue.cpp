/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   lcd_queue
 * \brief   Virtual serial port driver.
 * @file    lcd_queue.cpp
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2019/04/29
 * @brief   process the data lcd send to host

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

/**
 * queue.cpp - The G-code command queue
 */

#include "lcd_queue.h"
#include "../lcd/dwin/lcd_data.h"
#include "gcode.h"

#include "../lcd/ultralcd.h"
#include "../sd/cardreader.h"
#include "../module/planner.h"
#include "../module/temperature.h"
#include "../Marlin.h"

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../feature/leds/printer_event_leds.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../feature/power_loss_recovery.h"
#endif

uint8_t virtua1_serial_buffer[VIRTUAL_SIZE];
uint8_t have_serial_cmd = 0;
static int virtua_serial_count = 0;

uint8_t lcd_command_queue[LCD_SIZE];
uint8_t have_lcd_cmd = 0;
static int serial_count = 0;

const char command_ok[8]={0X5A, 0XA5, 0X05, 0X82, 0X4F, 0X4B, 0XA5, 0XEF};
const char command_order[7]={0X5A, 0XA5, 0X08, 0X83, 0X10, 0X00, 0X01};

/**
 * Clear the dwin lcd command queue
 */
void clear_lcd_command_queue() {
  memset(lcd_command_queue,0,LCD_SIZE);
  serial_count = 0;
  have_lcd_cmd = 0;
}

/**
 * Clear the virtual_serial command queue
 */
void clear_virtaul_command_queue() {
  memset(virtua1_serial_buffer,0,VIRTUAL_SIZE);
  virtua_serial_count = 0;
  have_serial_cmd = 0;
}

/**
 * get the command from the lcd_serial
 */
void get_lcd_commands() {
  static char serial_command_queue[LCD_SIZE];
  char c;
  // queue has space, serial has data
  while (MYSERIAL2.available()) {
    c = MYSERIAL2.read();
    //MYSERIAL1.write('a');
    serial_command_queue[serial_count++] = c;
    if(serial_count  == (serial_command_queue[2]+3))
    {
      serial_count = 0;
      have_lcd_cmd = 1;
      memcpy(lcd_command_queue,serial_command_queue,(serial_command_queue[2]+3));
      memset(serial_command_queue,0,LCD_SIZE);
    }
  }
  //data is error
  if(serial_command_queue[0]!=0x5A)
  {
    serial_count = 0;
    memset(serial_command_queue,0,LCD_SIZE);
  }
}

/**
 * crc check str from lcd_command_queue is right
 * str is the command 
 * if frame header is wrong return err
 * if crc is wrong return err
 * if right return CRC_OK
 */
CmdType check_lcd_cmd_crc(uint8_t * str)
{
  if((str[0]==0x5A)&&(str[1]==0xA5)&&(str[2]!=0))
  {
    uint16_t crc = usMBCRC16((str+3),(uint32_t)str[2]-2);
    if((str[str[2]+1]== (crc>>8))&&str[str[2]+2]== (crc&0x00FF))
    {
      return CRC_OK;
    }
    else
      return CRC_ERR;
  }
  else
    return CRC_ERR;
}

/**
 * send string to dwin_lcd
 * str:the data you need to send
 * on extra crc_value added
 */
void send_lcd_commands(uint8_t * str) {
  int i;
  if((str[0]==0x5A)&&(str[1]==0xA5)&&(str[2]!=0))
  {
    for(i=0;i<str[2]+3;i++)
      MYSERIAL2.write(str[i]);
      clear_lcd_command_queue();
  }
}

/**
 * send string to dwin_lcd
 * str:the data you need to send 
 * take care the value of str[2] will plus 2 automatic
 */
void send_lcd_commands_CRC16(uint8_t * str,uint8_t times) {
  int j;
  for(j=0;j<times;j++){
    if(add_lcd_commands_CRC16(str)==1)break;
  }
}
/**
 * send string to dwin_lcd
 * str:the data you need to send
 * str will be added crc check value
 * take care the value of str[2] need plus 2
 */
int add_lcd_commands_CRC16(uint8_t * str) {
  int i;
  if((str[0]==0x5A)&&(str[1]==0xA5)&&(str[2]!=0))
  {
    uint16_t crc = usMBCRC16((str+3),(uint32_t)str[2]);
    uint8_t low = (crc>>8);
    uint8_t high = (crc&0x00FF);
    str[2]+=2;
    for(i=0;i<str[2]+1;i++)
       MYSERIAL2.write(str[i]);
    MYSERIAL2.write(low);
    MYSERIAL2.write(high);
    uint32_t serial_timeout = millis() + 10UL;
    while(PENDING(millis(), serial_timeout)){
      get_lcd_commands();
      if(have_lcd_cmd)
      {
        if(OK_TYPE == get_command_type())return 1;
      }
    }
    return 0;
  }
  else
    return 0;
}

/**
 * get the cmd from virtual serial
 * 
 */
void get_virtual_serial_cmd(void) {
  static char serial_queue[20];
  while (MYSERIAL0.available_tx()) {
    int c;
    c = MYSERIAL0.read_tx();
    serial_queue[virtua_serial_count++] = c;
    if(c == '\n' || c == '\r')
    {
      serial_queue[virtua_serial_count++] = '\r';
      serial_queue[virtua_serial_count++] = '\n';
      memcpy(virtua1_serial_buffer,serial_queue,virtua_serial_count);
      virtua_serial_count = 0;
      have_serial_cmd = 1;
      memset(serial_queue,0,20);
    }
  }
}

/**
 * send string to virtual serial
 * str:the data you need to send 
 */
uint8_t send_virtual_serial(char * str) {
  //for(i=0;i<strlen((const char*)str);i++)
  MYSERIAL0.printf_rx("%s\n",str);
  uint32_t serial_timeout = millis() + 10UL;
  while(PENDING(millis(), serial_timeout)){
    get_virtual_serial_cmd();
    if(have_serial_cmd)
    {
      if(OK_TYPE == get_vserial_command_type())
      {
        SERIAL_ECHOLNPGM(" ok ");
        return 1;
      }
    }
  }
  return 0;
}

/**
 * send string to dwin_lcd
 * str:the data you need to send 
 * take care the value of str[2] will plus 2 automatic
 */
void send_virtual_serial_cmd(uint8_t * str,uint8_t times) {
  int j;
  for(j=0;j<times;j++){
    if(send_virtual_serial((char*)str)==1)break;
  }
}

#if ENABLED(VIRTUAL_DEBUG)
void virtual_serial_debug(uint8_t * str) {
  int i;
  for(i=0;i<20;i++)
    MYSERIAL1.write(str[i]);
  MYSERIAL1.write('\r');
  MYSERIAL1.write('\n');
}
#endif

/**
 * judge the type of the command the lcd send to host
 * command type can be seen in enum CmdType
 */
CmdType get_vserial_command_type(void)
{
  uint8_t str[VIRTUAL_SIZE];
  memset(str,0,VIRTUAL_SIZE);
  memcpy((void*)str,(void*)virtua1_serial_buffer,strlen((const char *)virtua1_serial_buffer));
  clear_virtaul_command_queue();
  //if have a command
  //SERIAL_ECHOLNPGM("get type");
  if (strncmp((char *)str,"ok",2) == 0)
  {
    return OK_TYPE;
  }
  else 
    return CMD_ERR;
}

/**
 * judge the type of the command the lcd send to host
 * command type can be seen in enum CmdType
 */
CmdType get_command_type(void)
{
  uint8_t str[LCD_SIZE];
  memset(str,0,LCD_SIZE);
  memcpy((void*)str,(void*)lcd_command_queue,LCD_SIZE);
  //virtual_serial_debug(str);
  //virtual_serial_debug(lcd_command_queue);
  clear_lcd_command_queue();
  //if have a command
  //SERIAL_ECHOLNPGM("get type");
  if (check_lcd_cmd_crc(str)==CRC_ERR)
  {
    SERIAL_ECHOLNPGM("crc err");
    return CMD_ERR;
  }
  //ok_type
  //SERIAL_ECHOLNPGM("crc ok");
  if(strncmp((const char*)str,(const char*)command_ok,8)==0)
  {
    return OK_TYPE;
  }
  //order
  else if(strncmp((const char*)str,(const char*)command_order,7)==0)
  {
    if(str[7]== 0x00)
    {
      SERIAL_ECHOLNPGM("cmd type");
      switch (str[8])
      {
        case 0x01:return EN_FONT_BT;break;
        case 0x02:return UK_FONT_BT;break;
        default:
        #if ENABLED(VIRTUAL_DEBUG)
        #endif
          return CMD_ERR;
          break;
      }
    }
    else if(str[7]== 0x01)
    {
      switch (str[8])
      {
        case 0x00:
          return MAINPAGE_PRINT_BT;
          break;
        case 0x01:
          return FILE_LIST1_DOWN_BT;
          break;
        case 0x02:
          return FILE_LIST2_UP_BT;
          break;
        case 0x03:
          return FILE_RETURN_BT;
          break;
        case 0x04:
          return LIST1_FILE1_BT;
          break;
        case 0x05:
          return LIST1_FILE2_BT;
          break;
        case 0x06:
          return LIST1_FILE3_BT;
          break;
        case 0x07:
          return LIST1_FILE4_BT;
          break;
        case 0x08:
          return LIST2_FILE1_BT;
          break;
        case 0x09:
          return LIST2_FILE2_BT;
          break;
        case 0x0A:
          return LIST2_FILE3_BT;
          break;
        case 0x0B:
          return LIST2_FILE4_BT;
          break;
        case 0x0C:
          return LIST3_FILE1_BT;
          break;
        case 0x0D:
          return LIST3_FILE2_BT;
          break;
        case 0x0E:
          return LIST3_FILE3_BT;
          break;
        case 0x0F:
          return LIST3_FILE4_BT;
          break;
        case 0x11:
          return FILE_LIST2_DOWN_BT;
          break;
        case 0x12:
          return FILE_LIST3_UP_BT;
          break;
        case 0x13:
          return FILE_START_STOP_BT;
          break;
        case 0x14:
          return LIGHT_BT;
          break;
        case 0x15:
          return PRINT_RETURN_BT;
          break;
        case 0x16:
          return LOAD_FILAMENT_BT;
          break;
        case 0x17:
          return LOAD_HEAT_STOP_BT;
          break;
        case 0x18:
          return UNLOAD_FILAMENT_BT;
          break;
        case 0x19:
          return UNLOAD_HEAT_STOP_BT;
          break;
        case 0x1A:
          return BUZZER_BT;
          break;
        case 0x1B:
          return X_STEP_ADD_BT;
          break;
        case 0x1C:
          return XYZ_HOME_BT;
          break;
        default:
          return CMD_ERR;
          break;
      }
    }
    else
      return CMD_ERR;
  }
  else 
    return CMD_ERR;
}

/**
 * judge the type of the command the lcd send to host
 * command type can be seen in enum CmdType
 */
void processing_lcd_command(void)
{
  if(have_lcd_cmd)
  {
    CmdType type = get_command_type();
    switch (type){
      case CMD_ERR:break;
      case OK_TYPE:break;
      case EN_FONT_BT:
        SERIAL_ECHOLNPGM("start");
        send_virtual_serial_cmd((uint8_t*)"G1 X100",5);
        send_lcd_commands_CRC16(CHANGE_PAGE_2,10);
        break;
      case UK_FONT_BT:break;
      default:break;
    }
  }
}
