/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   material_check
 * \brief   Material detection drive.
 * @file    material_check.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/11
 * @brief   Header file for material detection drive.
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
 * This file is a drive for material detection drive.
 *
 * \par Method List:
 *
 *    1.  void material_check::init(void);
 *    2.  void material_check::set_filamen_runout_report_status(bool set_val);
 *    3.  bool material_check::get_filamen_runout_report_status(void);
 *    4.  void material_check::set_filamen_motion_report_status(bool set_val);
 *    5.  bool material_check::get_filamen_motion_report_status(void);
 *    6.  bool material_check::is_filamen_runout(void);
 *    7.  uint16_t material_check::get_code_wheel_step_in_windows(void);
 *    8.  void material_check::code_wheel_step_update(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/11     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _MATERIAL_CHECK_H_
#define _MATERIAL_CHECK_H_

#ifdef TARGET_LPC1768
#ifdef USE_MATERIAL_MOTION_CHECK

#include <stdarg.h>
#include <stdio.h>
#include <Stream.h>
#include <Arduino.h>

#define CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM              10
#define CODE_WHEEL_STEP_STATISTICS_TIME                   500

class material_check
{
public:

  material_check(void);

  void init(void);
  void set_filamen_runout_report_status(bool set_val);
  bool get_filamen_runout_report_status(void);
  void set_filamen_motion_report_status(bool set_val);
  bool get_filamen_motion_report_status(void);
  bool is_filamen_runout(void);
  void code_wheel_step_update();
  void material_extrusion_update();
  uint16_t get_code_wheel_step_in_windows(void);
  uint16_t get_material_extrusion_in_windows(void);

private:
  bool is_report_filamen_runout;
  bool is_report_filamen_motion;
  uint32_t code_wheel_step_in_windows;
  uint32_t material_extrusion_in_windows;
  uint16_t code_wheel_step_in_statistics_time[CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM];
  uint16_t material_extrusion_in_statistics_time[CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM];
};

extern material_check MaterialCheck;

#endif // USE_MATERIAL_MOTION_CHECK
#endif // TARGET_LPC1768
#endif // _MATERIAL_CHECK_H_
