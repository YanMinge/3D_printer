/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   lcd_file
 * \brief   lcd_file_list.
 * @file    lcd_file.h
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for lcd_file list.
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
 * This file is the fundamental function of lcd_file list.
 *
 * \par Method List:
 *
 *    1.  void    udisk_reader::init(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "../Marlin.h"
#include "../../module/planner.h"
#include "../../gcode/gcode.h"
#include "../../module/temperature.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_file.h"
#include "lcd_process.h"
#include "lcd_parser.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"
#include "filament_ui.h"

#if PIN_EXISTS(BEEPER)
#include "../../libs/buzzer.h"
#endif

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"
#endif

#ifdef USE_MATERIAL_MOTION_CHECK
#include "material_check.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#include "../../feature/pause.h"
#endif

void lcd_process::show_start_up_page(void)
{
  head_t head_machine;
  head_machine = MachineInfo.get_head_type();
  if(HEAD_NULL == head_machine && LAN_NULL == ui_language)
  {
    return;
  }
  if(LAN_NULL == ui_language)
  {
    lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
    machine_status = PRINT_MACHINE_STATUS_NO_SET_LANGUAGE;
    return;
  }
  if(HEAD_PRINT == head_machine)
  {
    change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
  }
  else if(HEAD_LASER == head_machine)
  {
    change_lcd_page(LASER_HOME_PAGE_EN,LASER_HOME_PAGE_CH);
  }
}

void lcd_process::change_lcd_page(int en_page_num, int ch_page_num)
{
  if(LAN_CHINESE == ui_language)
  {
    lcd_send_data(PAGE_BASE + ch_page_num, PAGE_ADDR);
  }
  else if(LAN_ENGLISH == ui_language)
  {
    lcd_send_data(PAGE_BASE + en_page_num, PAGE_ADDR);
  }
}

void lcd_process::clear_page(unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
  for(int i = 0; i < PAGE_FILE_NUM; i++)
  {
    lcd_text_clear((addr + FILE_TEXT_LEN*i), FILE_NAME_LCD_LEN);
    lcd_send_data(TYPE_NULL,(PRINT_FILE_SIMAGE_ICON_ADDR + i));
  }
}

inline void lcd_process::send_page(unsigned long addr,int page,int num, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
#define PAGE(a,b) (a - PAGE_FILE_NUM*b)

  pfile_list_t temp = NULL;
  for(int i = page*PAGE_FILE_NUM; i < (page*PAGE_FILE_NUM + num); i++)
  {
    temp = LcdFile.file_list_index((i+1));
    lcd_send_data(TYPE_LOAD,(PRINT_FILE_SIMAGE_ICON_ADDR + PAGE(i,page)));
    lcd_send_data(temp->file_name,addr + FILE_TEXT_LEN*PAGE(i,page));
  }
}

void lcd_process::send_first_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page + 1, file_info.page_count); //send page num to lcd

  if((file_info.page_count > 1) && file_info.current_page == 0)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);  //send load icon to lcd
    SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);  // show first file_list page
  }
  else if((file_info.page_count == 1) && file_info.current_page == 0)
  {
    if(0 == file_info.last_page_file)
    {
      file_info.last_page_file = PAGE_FILE_NUM;
    }
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
  }
  else if((file_info.page_count == 0) && (file_info.current_page == 0))
  {
    SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
  }
  LcdFile.set_current_page(1);
}

void lcd_process::send_next_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page + 1, file_info.page_count);

  if(file_info.page_count == (file_info.current_page + 1))
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    //lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_END_PAGE, PAGE_ADDR);
    SEND_PAGE(PRINT, _FILE_LIST_END_PAGE, PAGE_BASE, LASER);
    LcdFile.set_current_page(file_info.current_page + 1);
  }
  else if(file_info.page_count > file_info.current_page + 1)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);
    //lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_MIDDLE_PAGE, PAGE_ADDR);
    SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
    LcdFile.set_current_page(file_info.current_page + 1);
  }
}

void lcd_process::send_last_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page-2,PAGE_FILE_NUM);
  if((file_info.page_count > 2))
  {
    if(file_info.current_page == 2)
    {
      SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
    }
    else
    {
      SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
    }
  }
  else if((file_info.page_count == 2))
  {
    SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
  }
  LcdFile.set_current_page(file_info.current_page - 1);
  get_file_info();
  send_file_list_page_num(file_info.current_page, file_info.page_count);
}

void lcd_process::send_given_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page, file_info.page_count);

  if(1 == file_info.current_page)
  {
    if(1 == file_info.page_count)
    {
      if(0 == file_info.last_page_file) file_info.last_page_file = PAGE_FILE_NUM;
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,file_info.last_page_file);
      SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.page_count >= 2)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,PAGE_FILE_NUM);
      SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.page_count == 0)
    {
      SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
    }
  }

  else if(file_info.current_page >= 2)
  {
    if(file_info.current_page == file_info.page_count)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,file_info.last_page_file);
      SEND_PAGE(PRINT, _FILE_LIST_END_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.current_page < file_info.page_count)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1, PAGE_FILE_NUM);
      SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
    }
  }
}

void lcd_process::show_print_set_page(void)
{
#if PIN_EXISTS(BEEPER)
  if(LAN_CHINESE == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_ON_SET_PAGE_CH, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_OFF_SET_PAGE_CH, PAGE_ADDR);
    }
  }
  else if(LAN_ENGLISH == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_ON_SET_PAGE_EN, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_OFF_SET_PAGE_EN, PAGE_ADDR);
    }
  }
#endif
}

void lcd_process::show_laser_set_page(void)
{
#if PIN_EXISTS(BEEPER)
  if(LAN_CHINESE == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_ON_SET_PAGE_CH, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_OFF_SET_PAGE_CH, PAGE_ADDR);
    }
  }
  else if(LAN_ENGLISH == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_ON_SET_PAGE_EN, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_OFF_SET_PAGE_EN, PAGE_ADDR);
    }
  }
#endif
}

void lcd_process::show_machine_set_page(void)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    dwin_process.show_print_set_page();
  }
  else
  {
    dwin_process.show_laser_set_page();
  }
}

void lcd_process::show_machine_status(uint8_t ch_type)
{

  if(LAN_CHINESE == ui_language)
  {
    if(ch_type <= MACHINE_STATUS_NUM)
      lcd_send_data(ch_type,PRINT_MACHINE_STATUS_ICON_ADDR);
    else
      lcd_send_data(ch_type - MACHINE_STATUS_NUM,PRINT_PREPARE_TEXT_ICON_ADDR);
  }
  else if(LAN_ENGLISH == ui_language)
  {
    lcd_send_data(ch_type + MACHINE_STATUS_NUM,PRINT_MACHINE_STATUS_ICON_ADDR);
  }
}

void lcd_process::show_machine_continue_print_page(void)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    show_machine_status(PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH);
  }
  else
  {
    return;
  }
  change_lcd_page(PRINT_CONFIRM_CANCEL_HINT_PAGE_EN,PRINT_CONFIRM_CANCEL_HINT_PAGE_CH);
  machine_status = PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH;
}

void lcd_process::show_machine_recovery_print_page(void)
{
  dwin_process.reset_image_send_parameters();
  LcdFile.set_current_status(out_printing);

#if ENABLED(POWER_LOSS_RECOVERY)
  if(udisk.job_recover_file_exists())
  {
    malloc_current_file();
    int16_t value = udisk.ls(LS_GET_FILE_NAME, dwin_parser.current_path, ".gcode");
    if(USB_NOT_DETECTED == value || value)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    dwin_parser.set_file_read_status(true);
    dwin_parser.set_file_list_open_status(true);
    LcdFile.set_file_page_info();
    LcdFile.set_current_page(1);

    strcpy(current_file->file_name,recovery.info.udisk_filename);
    if(udisk.check_gm_file(current_file->file_name))
    {
      current_file->file_type = TYPE_MAKEBLOCK_GM;
      dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
      dwin_process.lcd_send_data(current_file->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
      CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_START_PAGE_, EN, CH);
      dwin_process.limage_send_start();
    }
    else
    {
      current_file->file_type = TYPE_DEFAULT_FILE;
      dwin_process.lcd_send_data(TYPE_DEFAULT_FILE,PRINT_FILE_LIMAGE_ICON_ADDR);
      dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
      dwin_process.lcd_send_data(current_file->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
      CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_START_PAGE_, EN, CH);
    }
  }
#endif
}
void lcd_process::show_machine_status_page(machine_status_type print_status, \
                                                    machine_status_type laser_status, int page_en, int page_ch)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    show_machine_status(print_status);
    machine_status = print_status;
  }
  else
  {
    show_machine_status(laser_status);
    machine_status = laser_status;
  }
  change_lcd_page(page_en,page_ch);
}

void lcd_process::show_machine_status_page(machine_status_type status, int page_en, int page_ch)
{
  show_machine_status(status);
  machine_status = status;
  change_lcd_page(page_en,page_ch);
}

void lcd_process::show_start_print_file_page(pfile_list_t temp)
{
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_START_PAGE_, EN, CH)
  }
  else
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_START_PAGE_, EN, CH)
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}

void lcd_process::show_stop_print_file_page(pfile_list_t temp)
{
  if(machine_status == PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH)
  {
    //send file name ,image to the page;
  }
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_STOP_PAGE_, EN, CH)
  }
  else
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_STOP_PAGE_, EN, CH)
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}

void lcd_process::show_usb_pull_out_page(void)
{
  dwin_parser.set_file_list_open_status(false);
  dwin_parser.refresh_current_path();
  LcdFile.directory_stack_init();

  reset_image_send_parameters();
  show_machine_status(PRINT_MACHINE_STATUS_NO_USB_DISK_CH);
  set_machine_status(PRINT_MACHINE_STATUS_NO_USB_DISK_CH);
  CHANGE_PAGE(PRINT, LASER, _EXCEPTION_SURE_PAGE_, EN, CH)
}

void lcd_process::show_sure_block_page(machine_status_type ch_type)
{
  show_machine_status(ch_type);
  set_machine_status(ch_type);
  CHANGE_PAGE(PRINT, LASER, _EXCEPTION_SURE_PAGE_, EN, CH);
}

void lcd_process::show_confirm_cancel_page(machine_status_type ch_type)
{
  show_machine_status(ch_type);
  set_machine_status(ch_type);
  CHANGE_PAGE(PRINT, LASER, _CONFIRM_CANCEL_HINT_PAGE_, EN, CH);
}

void lcd_process::show_complete_hint_page(machine_status_type ch_type)
{
  show_machine_status(ch_type);
  set_machine_status(ch_type);
  CHANGE_PAGE(PRINT, LASER, _EXCEPTION_COMPLETE_HINT_PAGE_, EN, CH);
}

void lcd_process::show_prepare_block_page(machine_status_type ch_type)
{
  show_machine_status(ch_type);
  set_machine_status(ch_type);
  CHANGE_PAGE(PRINT, LASER, _PREPARE_BLOCK_PAGE_, EN, CH);
}

void lcd_process::show_prepare_no_block_page(machine_status_type ch_type)
{
  show_machine_status(ch_type);
  set_machine_status(ch_type);
  CHANGE_PAGE(PRINT, LASER, _PREPARE_HEAT_PAGE_, EN, CH);
}

void lcd_process::show_firmware_upate_page(void)
{
  temperature_progress_update(0);
  send_progress_percentage(0);
  change_lcd_page(PRINT_UPDATE_PAGE_EN,PRINT_UPDATE_PAGE_CH);

  show_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  set_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  change_lcd_page(PRINT_EXCEPTION_SURE_PAGE_EN,PRINT_EXCEPTION_SURE_PAGE_CH);
}

void lcd_process::show_calibration_page(void)
{
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_CALIBRATION_CH);
  UserExecution.cmd_user_synchronize();
  UserExecution.cmd_now_M104(200);
  UserExecution.cmd_now_M206(0);
  UserExecution.cmd_now_M420(false); //turn off bed leveling
  UserExecution.cmd_now_g28();
  UserExecution.cmd_now_g1_xy(X_BED_SIZE/2, Y_BED_SIZE/2,3000);
  UserExecution.cmd_now_M109(200);
  UserExecution.cmd_now_g38_z(2.5);
  UserExecution.cmd_user_synchronize();
  change_lcd_page(PRINT_CALIBRATION_PAGE_EN, PRINT_CALIBRATION_PAGE_CH);
}

void lcd_process::show_bed_leveling_page(void)
{
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_LEVELING_CH);
  UserExecution.cmd_user_synchronize();
  UserExecution.cmd_now_M104(200);
  UserExecution.cmd_now_g28();
  UserExecution.cmd_now_M109(200);
  UserExecution.cmd_now_g29();
  UserExecution.cmd_user_synchronize();
  UserExecution.cmd_now_M104(0);
  UserExecution.cmd_now_g28();
  UserExecution.cmd_user_synchronize();
  show_sure_block_page(PRINT_MACHINE_STATUS_LEVELING_OK_CH);
}

void lcd_process::show_save_calibration_data_page(void)
{
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_SAVE_OFFSET_CH);
  UserExecution.cmd_user_synchronize();
  UserExecution.cmd_now_M206(-current_position[Z_AXIS]);
  UserExecution.cmd_now_g28();
  UserExecution.cmd_now_M420(true); //turn on bed leveling
  UserExecution.cmd_now_M500();
  UserExecution.cmd_user_synchronize();
  show_sure_block_page(PRINT_MACHINE_STATUS_CALIBRATION_OK_CH);
  SERIAL_PRINTF("show_save_calibration_data_page \r\n");
}

void lcd_process::show_restore_factory_page(void)
{
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_RESTORE_CH);
  UserExecution.cmd_now_M502();
  UserExecution.cmd_now_M500();
  UserExecution.cmd_user_synchronize();
  delay(1000);
  show_sure_block_page(PRINT_MACHINE_STATUS_RESTORE_FACOTORY_CH);
}

void lcd_process::show_xyz_prepare_home_page(void)
{
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_HOMEING_CH);
  UserExecution.cmd_user_synchronize();
  UserExecution.cmd_now_g28();
  UserExecution.cmd_user_synchronize();
  show_sure_block_page(PRINT_MACHINE_STATUS_XYZ_HOME_OK_CH);
}

void lcd_process::show_prepare_print_page(pfile_list_t temp)
{
  dwin_process.set_lcd_temp_show_status(true);
  filament_show.set_heating_status_type(HEAT_PRINT_STATUS);
  filament_show.set_print_after_heat_status(true);
  UserExecution.user_start();
  show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_PRINT_CH);  //change to prepare_print_page

  //progress = 0;percentage = 0;
  pre_percentage = 0;
  send_temperature_percentage(pre_percentage);

  UserExecution.cmd_M2023(temp->file_name);  //open file and start read file
  UserExecution.cmd_M2024();                 //start print
}
//SERIAL_PRINTF("pause_position_x[%f],pause_position_y[%f],pause_position_z[%f],pause_position_e[%f]\r\n",(destination[X_AXIS]),(destination[Y_AXIS]),(destination[Z_AXIS]),(destination[E_AXIS]));
//SERIAL_PRINTF("pause_position_x[%f],pause_position_y[%f],pause_position_z[%f],pause_position_e[%f]\r\n",(LOGICAL_X_POSITION(current_position[X_AXIS])),(LOGICAL_X_POSITION(current_position[Y_AXIS])),(LOGICAL_X_POSITION(current_position[Z_AXIS])),(current_position[E_AXIS]));
//SERIAL_PRINTF("pause_position_x[%f],pause_position_y[%f],pause_position_z[%f],pause_position_e[%f]\r\n",planner.get_axis_position_mm(X_AXIS),planner.get_axis_position_mm(Y_AXIS),planner.get_axis_position_mm(Z_AXIS),planner.get_axis_position_mm(E_AXIS));

void lcd_process::show_pause_print_page(pfile_list_t temp)
{

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif
  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_STOP_TASK_CH);

  //save current position, fan speed, temperature hotended bed,
  pause_print_data.current_position[X_AXIS] = planner.get_axis_position_mm(X_AXIS);
  pause_print_data.current_position[Y_AXIS] = planner.get_axis_position_mm(Y_AXIS);
  pause_print_data.current_position[Z_AXIS] = LOGICAL_Z_POSITION(planner.get_axis_position_mm(Z_AXIS));
  pause_print_data.current_position[E_AXIS] = planner.get_axis_position_mm(E_AXIS);
  pause_print_data.fan_speed[0] =  thermalManager.fan_speed[0];
  pause_print_data.feedrate = uint16_t(feedrate_mm_s * 60.0f);
  pause_print_data.target_temperature[0] = thermalManager.degTargetHotend(0);
  pause_print_data.target_temperature_bed = thermalManager.degTargetBed();

  UserExecution.pause_udisk_print();
  SERIAL_PRINTF("D3 X(%f),Y(%f),Z(%f),E(%f)\r\n", pause_print_data.current_position[X_AXIS], pause_print_data.current_position[Y_AXIS], pause_print_data.current_position[Z_AXIS], pause_print_data.current_position[E_AXIS]);
  //turn on fan
  UserExecution.cmd_M106(255);
  safe_delay(20);

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  do_pause_e_move(-6, 50);
#endif
  SERIAL_PRINTF("D4 X(%f),Y(%f),Z(%f),E(%f)\r\n", current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
  //go home
  SERIAL_PRINTF("before cmd_now_g28\r\n");
  UserExecution.cmd_now_g28();

  dwin_process.show_start_print_file_page(temp);
  LcdFile.set_current_status(stop_printing);

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = true;
#endif
}

void lcd_process::show_prepare_from_pause_page(pfile_list_t temp)
{

  show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_PRINT_CH);  //change to prepare_print_page
  filament_show.set_heating_status_type(HEAT_PRINT_STATUS);

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif

  //turn on the peripheral fan temperature.
  if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
  {
    dwin_process.set_lcd_temp_show_status(true);
    dwin_process.pre_percentage = 0;
    dwin_process.send_temperature_percentage(dwin_process.pre_percentage);
    UserExecution.cmd_now_M106(pause_print_data.fan_speed[0]);
    UserExecution.cmd_now_M104(pause_print_data.target_temperature[0]);
    UserExecution.cmd_now_M190(pause_print_data.target_temperature_bed);
#if ENABLED(ADVANCED_PAUSE_FEATURE)
   do_pause_e_move(6, 5);
#endif
    // Now all extrusion positions are resumed and ready to be confirmed
    // Set extruder to saved position
    planner.set_e_position_mm((destination[E_AXIS] = current_position[E_AXIS] = pause_print_data.current_position[E_AXIS]));
    SERIAL_PRINTF("fan_speed(%d),target_temperature(%d),target_temperature_bed(%d)\r\n", pause_print_data.fan_speed[0],pause_print_data.target_temperature[0],pause_print_data.target_temperature_bed);
  }

  //turn on the peripheral fan temperature.
  if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
  {
    UserExecution.cmd_now_M109(pause_print_data.target_temperature[0]);
  }

  if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
  {
    UserExecution.cmd_user_synchronize();
  }

  if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
  {
    UserExecution.cmd_now_g1(pause_print_data.current_position);
  }

  if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
  {
    //change to start print page
    UserExecution.cmd_M2026(pause_print_data.udisk_pos);
    safe_delay(20);
    dwin_process.show_stop_print_file_page(temp);
    //change to start print page
    UserExecution.cmd_M2024();
    LcdFile.set_current_status(on_printing);
  }

}

void lcd_process::show_load_filament_page(void)
{
  filament_show.set_heating_status_type(HEAT_LOAD_STATUS);

  if(MaterialCheck.get_filamen_runout_report_status() && !MaterialCheck.is_filamen_runout())
  {
    dwin_process.show_sure_block_page(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
    return;
  }
  if(HEAT_LOAD_STATUS == filament_show.get_heating_status_type() ) //heating page before loading filament
  {
    dwin_process.set_lcd_temp_show_status(true);
    dwin_process.pre_percentage = 0;
    dwin_process.send_temperature_percentage(dwin_process.pre_percentage);

    show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_LOAD_CH);
    UserExecution.user_start();
    UserExecution.cmd_now_M106(150);
    UserExecution.cmd_now_M109(230);
  }

  if(HEAT_LOAD_STATUS == filament_show.get_heating_status_type() && \
    PRINT_MACHINE_STATUS_PREPARE_LOAD_CH == dwin_process.get_machine_status()) //loading filament page
  {
    show_complete_hint_page(PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH);
    UserExecution.cmd_now_M701();
  }

  if(HEAT_LOAD_STATUS == filament_show.get_heating_status_type() && \
    PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH == dwin_process.get_machine_status()) //loading filament success page
  {
    show_sure_block_page(PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_CH);
    UserExecution.cmd_now_M107();
    UserExecution.cmd_M300(300,500);
    UserExecution.cmd_M300(500,1000);
  }
}

void lcd_process::show_unload_filament_page(void)
{
  filament_show.set_heating_status_type(HEAT_UNLOAD_STATUS);

  if(HEAT_UNLOAD_STATUS == filament_show.get_heating_status_type()) //heating page before loading filament
  {
    dwin_process.set_lcd_temp_show_status(true);
    dwin_process.pre_percentage = 0;
    dwin_process.send_temperature_percentage(dwin_process.pre_percentage);

    show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_LOAD_CH);
    UserExecution.user_start();
    UserExecution.cmd_now_M106(150);
    UserExecution.cmd_now_M109(230);
  }

  if(HEAT_UNLOAD_STATUS == filament_show.get_heating_status_type()) //unloading filament page
  {
    dwin_process.set_lcd_temp_show_status(false);
    dwin_process.pre_percentage = 0;
    dwin_process.send_temperature_percentage(dwin_process.pre_percentage);

    show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_UNLOAD_CH);
    UserExecution.cmd_now_M702();
  }

  if(HEAT_UNLOAD_STATUS == filament_show.get_heating_status_type()) //unloading filament success page
  {
    show_sure_block_page(PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_CH);
    UserExecution.cmd_now_M107();
    UserExecution.cmd_now_M104(0);
    UserExecution.cmd_M300(300,500);
    UserExecution.cmd_M300(500,1000);
  }
}

void lcd_process::show_cancel_stop_print_page(pfile_list_t temp)
{
#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif

  print_status status;
  status = LcdFile.get_current_status();
  if(stop_printing == status)
  {
    show_start_print_file_page(temp);
  }
  else if(on_printing == status)
  {
    show_stop_print_file_page(temp);
  }
}

void lcd_process::show_confirm_stop_print_page(void)
{
#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif

  show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_QUIT_TASK_CH);
  dwin_process.reset_image_send_parameters();
  UserExecution.pause_udisk_print();
  udisk.stop_udisk_print();  //stop print

  if(IS_HEAD_LASER())
  {
    enqueue_and_echo_commands_P("M4 S0");
  }
  LcdFile.set_current_status(out_printing);

  SERIAL_PRINTF("before cmd_now_g28\r\n");
  UserExecution.cmd_now_g28();
  show_sure_block_page(PRINT_MACHINE_STATUS_TASK_CANCEL_CH);
#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = true;
#endif

  //dwin_process.send_given_page_data();
  //dwin_process.simage_send_start();
  //if(udisk.job_recover_file_exists())
  //{
  //  udisk.remove_job_recovery_file();
  //  dwin_process.delete_current_file();
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
