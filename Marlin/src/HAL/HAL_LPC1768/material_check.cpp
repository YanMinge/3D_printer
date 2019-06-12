/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   material_check
 * \brief   Material detection drive.
 * @file    material_check.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/11
 * @brief   Source code for material detection drive.
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

#ifdef TARGET_LPC1768
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"
#include HAL_PATH(.., HAL.h)

#ifdef USE_MATERIAL_MOTION_CHECK

#include "material_check.h"
#include "../../module/motion.h"
#include "../../module/planner.h"

material_check MaterialCheck;

material_check::material_check(void)
{

}

void material_check::init(void)
{
  is_report_filamen_runout = false;
  is_report_filamen_motion = false;
  SET_INPUT(MATERIAL_MOTION_PIN);
}

void material_check::set_filamen_runout_report_status(bool set_val)
{
  is_report_filamen_runout = set_val;
}

bool material_check::get_filamen_runout_report_status(void)
{
  return is_report_filamen_runout;
}


void material_check::set_filamen_motion_report_status(bool set_val)
{
  is_report_filamen_motion = set_val;
}

bool material_check::get_filamen_motion_report_status(void)
{
  return is_report_filamen_motion;
}

bool material_check::is_filamen_runout(void)
{
#if HAS_FILAMENT_SENSOR
  if(READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_INVERTING)
  {
    return true;
  }
  else
  {
    return false;
  }
#endif //HAS_FILAMENT_SENSOR
  return false;
}

uint16_t material_check::get_code_wheel_step_in_windows(void)
{
  code_wheel_step_in_windows = 0;
  for(uint8_t i = 0; i< CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM; i++)
  {
    code_wheel_step_in_windows += code_wheel_step_in_statistics_time[i];
  }
  return code_wheel_step_in_windows;
}

uint16_t material_check::get_material_extrusion_in_windows(void)
{
  material_extrusion_in_windows = 0;
  for(uint8_t i = 0; i< CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM; i++)
  {
    material_extrusion_in_windows += material_extrusion_in_statistics_time[i];
  }
  return material_extrusion_in_windows;
}

void material_check::code_wheel_step_update(void)
{
  static uint16_t code_wheel_update_time_local = 0;
  static uint16_t code_wheel_update_statistics_time_counter;
  static uint16_t code_wheel_step = 0;
  if(code_wheel_update_time_local < CODE_WHEEL_STEP_STATISTICS_TIME)
  {
    static bool previous_step_status = false;
    bool step_status = READ(MATERIAL_MOTION_PIN);
    if(step_status != previous_step_status)
    {
      code_wheel_step++;
	  previous_step_status = step_status;
    }
    code_wheel_update_time_local++;
  }
  else
  {
    code_wheel_step_in_statistics_time[code_wheel_update_statistics_time_counter] = code_wheel_step;
    code_wheel_update_statistics_time_counter++;
	code_wheel_update_time_local = 0;
	code_wheel_step = 0;
	if(code_wheel_update_statistics_time_counter == CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM)
    {
      code_wheel_update_statistics_time_counter = 0;
    }
  }
}

void material_check::material_extrusion_update(void)
{
  static uint16_t material_update_time_local = 0;
  static uint16_t material_update_statistics_time_counter;
  static int32_t pre_material_extrusion = 0;
  if(material_update_time_local < CODE_WHEEL_STEP_STATISTICS_TIME)
  {
    material_update_time_local++;
  }
  else
  {
    int32_t material_extrusion = round(planner.get_axis_position_mm(E_AXIS));
    material_extrusion_in_statistics_time[material_update_statistics_time_counter] = material_extrusion - pre_material_extrusion;
    material_update_statistics_time_counter++;
	material_update_time_local = 0; 
	pre_material_extrusion = material_extrusion;
	if(material_update_statistics_time_counter == CODE_WHEEL_STEP_WINDOWS_ELEMENTS_NUM)
    {
      material_update_statistics_time_counter = 0;
    }
  }
}

#endif // USE_MATERIAL_MOTION_CHECK
#endif // TARGET_LPC1768
