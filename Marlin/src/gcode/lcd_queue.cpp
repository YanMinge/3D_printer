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

static uint8_t virtua1_serial_buffer[VIRTUAL_SIZE];
uint8_t have_serial_cmd = 0;
static int virtua_serial_count = 0;

static uint8_t lcd_command_queue[LCD_SIZE];
static uint8_t have_lcd_cmd = 0;

static int PrintStatus = 0;
static int StartStop = 0;
static int HeatErr = 0;
static int IsPrint = 0;

LCDFILINFO lcd_file[20];

const char command_ok[8]={0X5A, 0XA5, 0X05, 0X82, 0X4F, 0X4B, 0XA5, 0XEF};
const char command_order[7]={0X5A, 0XA5, 0X08, 0X83, 0X10, 0X00, 0X01};

/**
 * send the font to display in lcd_pannel
 */
void lcd_font_init(void)
{
  send_lcd_commands_CRC16(PAGE1_0_1,10);
  send_lcd_commands_CRC16(PAGE1_0_2,10);
  send_lcd_commands_CRC16(PAGE1_0_3,10);

  //send_lcd_commands_CRC16(PAGE1_1_1,10);
  //send_lcd_commands_CRC16(PAGE1_1_2,10);

  send_lcd_commands_CRC16(PAGE2_0_1,10);
  send_lcd_commands_CRC16(PAGE2_0_2,10);
  send_lcd_commands_CRC16(PAGE2_0_3,10);
  send_lcd_commands_CRC16(PAGE2_0_4,10);
  send_lcd_commands_CRC16(PAGE2_0_5,10);

  send_lcd_commands_CRC16(PAGE2_1_1,10);
  send_lcd_commands_CRC16(PAGE2_1_2,10);

  //send_lcd_commands_CRC16(PAGE2_2_1,10);
  //send_lcd_commands_CRC16(PAGE2_2_2,10);
  //send_lcd_commands_CRC16(PAGE2_2_3,10);
  //send_lcd_commands_CRC16(PAGE2_2_4,10);
  //send_lcd_commands_CRC16(PAGE2_2_5,10);

  send_lcd_commands_CRC16(PAGE2_4_1,10);
  //send_lcd_commands_CRC16(PAGE2_4_1_1,10);
  send_lcd_commands_CRC16(PAGE2_4_2,10);
  send_lcd_commands_CRC16(PAGE2_4_3,10);
  send_lcd_commands_CRC16(PAGE2_4_4,10);

  send_lcd_commands_CRC16(PAGE2_5_1,10);
  send_lcd_commands_CRC16(PAGE2_5_2,10);

  send_lcd_commands_CRC16(PAGE2_7_1,10);

  send_lcd_commands_CRC16(PAGE2_8_1,10);
  send_lcd_commands_CRC16(PAGE2_8_2,10);
  send_lcd_commands_CRC16(PAGE2_8_3,10);

  send_lcd_commands_CRC16(PAGE2_9_1,10);
  send_lcd_commands_CRC16(PAGE2_ALL_0,10);
  send_lcd_commands_CRC16(PAGE2_ALL_1,10);
  //send_lcd_commands_CRC16(TEST_0,10);

  send_lcd_commands_CRC16(PAGE2_9_0,10);
  send_lcd_commands_CRC16(PAGE2_5_0,10);
  send_lcd_commands_CRC16(PAGE2_4_0,10);
  send_lcd_commands_CRC16(PAGE2_7_0,10);
  //send_lcd_commands_CRC16(PAGE2_2_1,10);
}

/**
 * Clear the dwin lcd command queue
 */
void clear_lcd_command_queue() {
  memset(lcd_command_queue,0,LCD_SIZE);
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
  char c;
  static uint8_t serial_count = 0;
  static char serial_command_queue[LCD_SIZE];
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
  //if data is error
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
 * if right return CMD_CHECK_OK
 */
CmdType check_lcd_commands(uint8_t * str)
{
  if((str[0]==0x5A)&&(str[1]==0xA5)&&(str[2]!=0))
  {
    uint16_t crc = usMBCRC16((str+3),(uint32_t)str[2]-2);
    if((str[str[2]+1]== (crc>>8))&&(str[str[2]+2]== (crc&0x00FF)))
    {
      return CMD_CHECK_OK;
    }
    else
    {
      return CMD_CHECK_ERR;
    }
  }
  else
  {
    return CMD_CHECK_ERR;
  }
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
void send_file_CRC16(uint8_t * str,uint8_t times) {
  int j;
  uint8_t str1[40];
  str1[0] = 0x5A;
  str1[1] = 0xA5;
  str1[2] = strlen((const char *)str)+3;
  str1[3] = 0x82;
  str1[4] = 0x11;
  str1[5] = 0xD0;
  memcpy((str1+6),(char *)str,strlen((char *)str));
  //memcpy(str1,str,40);
  for(j=0;j<times;j++){
    if(add_lcd_commands_CRC16(str1)==1)break;
  }
}

/**
 * send string to dwin_lcd
 * str:the data you need to send 
 * take care the value of str[2] will plus 2 automatic
 */
void send_lcd_commands_CRC16(uint8_t * str,uint8_t times) {
  int j;
  uint8_t str1[40];
  memcpy(str1,str,40);
  for(j=0;j<times;j++){
    if(add_lcd_commands_CRC16(str1)==1)break;
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
    for(i=0;i<str[2]+1;i++)
       MYSERIAL1.write(str[i]);
    MYSERIAL1.write(low);
    MYSERIAL1.write(high);
    uint32_t serial_timeout = millis() + 100UL;
    while(PENDING(millis(), serial_timeout)){
      get_lcd_commands();
      if(have_lcd_cmd)
      {
        if(CMD_OK == get_command_type())return 1;
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
      if(CMD_OK == get_vserial_command_type())
      {
        clear_virtaul_command_queue();
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
    return CMD_OK;
  }
  else 
    return CMD_ERROR;
}

/**
 * judge the type of the command the lcd send to host
 * command type can be seen in enum CmdType
 */
CmdType get_command_type(void)
{
  uint16_t num = 0;
  uint8_t str[LCD_SIZE];
  memset(str,0,LCD_SIZE);
  memcpy((void*)str,(void*)lcd_command_queue,LCD_SIZE);
  clear_lcd_command_queue();
  SERIAL_PRINTF("\r\n..get type..\r\n");
  if (CMD_CHECK_ERR == check_lcd_commands(str))
  {
    SERIAL_PRINTF("\r\ncrc err\r\n");
    return CMD_ERROR;
  }
  if(0 == strncmp((const char*)str,(const char*)command_ok,8))
  {
    return CMD_OK;
  }
  //order
  else if(strncmp((const char*)str,(const char*)command_order,7)==0)
  {
    num = (((uint16_t)str[7] << 8)||str[8]);
    switch (num)
    {
      case 0x0001:return CMD_BUTTON_SET_CN;break;
      case 0x0002:return CMD_BUTTON_SET_EN;break;
      case 0x0003:return FONT_CHECK_BT;break;
      case 0x0100:return MAINPAGE_PRINT_BT;break;
      case 0x0101:return FILE_LIST1_DOWN_BT;break;
      case 0x0102:return FILE_LIST2_UP_BT;break;
      case 0x0103:return FILE_RETURN_BT;break;
      case 0x0104:return LIST1_FILE1_BT;break;
      case 0x0105:return LIST1_FILE2_BT;break;
      case 0x0106:return LIST1_FILE3_BT;break;
      case 0x0107:return LIST1_FILE4_BT;break;
      case 0x0108:return LIST2_FILE1_BT;break;
      case 0x0109:return LIST2_FILE2_BT;break;
      case 0x010A:return LIST2_FILE3_BT;break;
      case 0x010B:return LIST2_FILE4_BT;break;
      case 0x010C:return LIST3_FILE1_BT;break;
      case 0x010D:return LIST3_FILE2_BT;break;
      case 0x010E:return LIST3_FILE3_BT;break;
      case 0x010F:return LIST3_FILE4_BT;break;
      case 0x0111:return FILE_LIST2_DOWN_BT;break;
      case 0x0112:return FILE_LIST3_UP_BT;break;
      case 0x0113:return FILE_START_STOP_BT;break;
      case 0x0114:return LIGHT_BT;break;
      case 0x0115:return PRINT_RETURN_BT;break;
      case 0x0116:return LOAD_FILAMENT_BT;break;
      case 0x0117:return LOAD_HEAT_STOP_BT;break;
      case 0x0118:return UNLOAD_FILAMENT_BT;break;
      case 0x0119:return UNLOAD_HEAT_STOP_BT;break;
      case 0x011A:return BUZZER_BT;break;
      case 0x011B:return X_STEP_ADD_BT;break;
      case 0x011C:return X_STEP_MIN_BT;break;
      case 0x011D:return Y_STEP_ADD_BT;break;
      case 0x011E:return Y_STEP_MIN_BT;break;
      case 0x011F:return Z_STEP_ADD_BT;break;
      case 0x0120:return Z_STEP_MIN_BT;break;
      case 0x0121:return XYZ_HOME_BT;break;
      default:return CMD_ERROR;break;
    }
  }
  else
    return CMD_ERROR;
}
/**
 * judge the type of the command the lcd send to host
 * command type can be seen in enum CmdType
 */
void parser_lcd_command(void)
{
  if(have_lcd_cmd)
  {
    CmdType type = get_command_type();
    switch (type){
      case CMD_ERROR:break;
      case CMD_OK:break;
      case CMD_BUTTON_SET_CN:
        //SERIAL_ECHOLNPGM("change page 2");
        send_lcd_commands_CRC16(PAGE1_1_1,10);
        send_lcd_commands_CRC16(PAGE1_1_2,10);
        send_lcd_commands_CRC16(CHANGE_PAGE_2,10);
        break;
      case CMD_BUTTON_SET_EN:break;
      case FONT_CHECK_BT:
        SERIAL_ECHOLNPGM("FONT_CHECK_BT");
        send_lcd_commands_CRC16(CHANGE_PAGE_4,10);
        break;


      case MAINPAGE_PRINT_BT:
      {
        if(PrintStatus == 0)
        {
          send_lcd_commands_CRC16(CHANGE_PAGE_7,10);
          PrintStatus = 1;
        }
        else
        {
          //send_lcd_commands_CRC16(CHANGE_PAGE_8,10);
          //send_lcd_commands_CRC16(PAGE2_2_2,10);
          //send_lcd_commands_CRC16(PAGE2_2_3,10);
          //send_lcd_commands_CRC16(PAGE2_2_4,10);
          //send_lcd_commands_CRC16(PAGE2_2_5,10);
          //send_lcd_commands_CRC16(PAGE2_2_1,10);

          //send M22
          //MYSERIAL0.printf_rx("M22\r\n");
          //memset(((void *)&lcd_file[0]), 0, sizeof(lcd_file[0]));
          //strcpy(lcd_file[0].fname ,"123.gcode");
          
          usb_read_test_lcd();
          send_lcd_commands_CRC16(CHANGE_PAGE_8,10);
          send_file_CRC16((uint8_t *)lcd_file[17].fname,1);
          send_lcd_commands_CRC16(PAGE2_2_3,10);
          send_lcd_commands_CRC16(PAGE2_2_4,10);
          send_lcd_commands_CRC16(PAGE2_2_5,10);
        }
        break;}

      case LIST1_FILE1_BT:
        PAGE2_4_2[11] = 0x31;
        send_lcd_commands_CRC16(PAGE2_4_2,10);
        send_lcd_commands_CRC16(CHANGE_PAGE_A,10);
        break;
      case LIST1_FILE2_BT:
        PAGE2_4_2[11] = 0x32;
        send_lcd_commands_CRC16(PAGE2_4_2,10);
        send_lcd_commands_CRC16(CHANGE_PAGE_A,10);
        break;
      case LIST1_FILE3_BT:
        PAGE2_4_2[11] = 0x33;
        send_lcd_commands_CRC16(PAGE2_4_2,10);
        send_lcd_commands_CRC16(CHANGE_PAGE_A,10);
        break;
      case LIST1_FILE4_BT:
        PAGE2_4_2[11] = 0x34;
        send_lcd_commands_CRC16(PAGE2_4_2,10);
        send_lcd_commands_CRC16(CHANGE_PAGE_A,10);
        break;

      case FILE_START_STOP_BT:{
        if(StartStop == 0)
        {
          if(HeatErr == 0)
          {
            send_lcd_commands_CRC16(CHANGE_PAGE_12,10);
            HeatErr = 1;
            break;
          }
          send_lcd_commands_CRC16(CHANGE_PAGE_E,10);
          int i;
          PAGE2_5_2[7] = 0X30;
          CHANGE_ICON_1[7] = 0X00;
          send_lcd_commands_CRC16(PAGE2_5_2,10);
          for(i=0; i<10; i++)
          {
            PAGE2_5_2[7] += 1;
            CHANGE_ICON_1[7] += 1;
            send_lcd_commands_CRC16(PAGE2_5_2,10);
            send_lcd_commands_CRC16(CHANGE_ICON_1,10);
            uint32_t serial_timeout = millis() + 100UL;
            while(PENDING(millis(), serial_timeout));
          }
          send_lcd_commands_CRC16(PAGE2_4_1_1,10);
          StartStop = 1;
          IsPrint = 1;
          send_lcd_commands_CRC16(CHANGE_PAGE_A,10);
        }
        else
        {
          send_lcd_commands_CRC16(PAGE2_4_1,10);
          StartStop = 0;
          IsPrint = 0;
        }
        
        break;}
      case X_STEP_ADD_BT:{
        float dis_x = current_position[X_AXIS] + 2;
        MYSERIAL0.printf_rx("G1 X%f\n",dis_x);
        uint32_t serial_timeout = millis() + 10UL;
        while(PENDING(millis(), serial_timeout)){
          if (commands_in_queue < BUFSIZE) get_available_commands();
          parser_lcd_command();
          get_virtual_serial_cmd();
          if(have_serial_cmd)
          {
            if(CMD_OK == get_vserial_command_type())
            {
              SERIAL_ECHOLNPGM("x add bt");
              clear_virtaul_command_queue();
            }
          }
        }
        break;}
      case X_STEP_MIN_BT:{
        float dis_x = current_position[X_AXIS] - 2;
        MYSERIAL0.printf_rx("G1 X%f\n",dis_x);
        break;}
      case Y_STEP_ADD_BT:{
        float dis_y = current_position[Y_AXIS] + 2;
        MYSERIAL0.printf_rx("G1 Y%f\n",dis_y);
        break;}
      case Y_STEP_MIN_BT:{
        float dis_y = current_position[Y_AXIS] - 2;
        MYSERIAL0.printf_rx("G1 Y%f\n",dis_y);
        break;}
      case Z_STEP_ADD_BT:{
        float dis_z = current_position[Z_AXIS] + 2;
        MYSERIAL0.printf_rx("G1 Z%f\n",dis_z);
        break;}
      case Z_STEP_MIN_BT:{
        float dis_z = current_position[Z_AXIS] - 2;
        MYSERIAL0.printf_rx("G1 Z%f\n",dis_z);
        break;}
      case XYZ_HOME_BT:{
        MYSERIAL0.printf_rx("G28\n");
        break;}
      default:break;
    }
  }
  if(IsPrint)
  {
    int i;
    for(i=0; i<5; i++)
    {
      uint32_t serial_timeout = millis() + 200UL;
      while(PENDING(millis(), serial_timeout));
    }
    send_lcd_commands_CRC16(PAGE2_4_1,10);
    send_lcd_commands_CRC16(CHANGE_PAGE_11,10);
    IsPrint = 0;
    StartStop = 0;
  }
}
