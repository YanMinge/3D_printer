/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   user_execution
 * \brief   Execution command API for LCD.
 * @file    user_execution.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   source code for LCD execution command API.
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
 * This file is used for LCD execution command.
 *
 * \par Method List:
 *
 *    1.  void    msd_reader::init(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "user_execution.h"

#if ENABLED(USE_DWIN_LCD)
user_execution UserExecution;

user_execution::user_execution(void)
{

}

void user_execution::init(void)
{

}

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
