/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   laser
 * \brief   The driver for laser head.
 * @file    laser.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/08/14
 * @brief   Header file for laser head driver.
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
 * This file is used for laser head driver.
 *
 * \par Method List:
 *
 *    1.  uint16_t laser_class::get_laser_power(void);
 *    2.  void     laser_class::set_laser_power(uint16_t power);
 *    3.  void     laser_class::delay_for_power_up(void);
 *    4.  void     laser_class::delay_for_power_down(void);
 *    5.  void     laser_class::set_laser_frequency(uint16_t laser_frequency);
 *    6.  void     laser_class::set_laser_ocr(uint16_t power);
 *    7.  void     laser_class::reset(void);
 *    8.  void     laser_class::update_laser_power(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/08/14     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _LASER_H_
#define _LASER_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(USE_DWIN_LCD)
  #include "lcd_file.h"
#endif

#if ENABLED(SPINDLE_LASER_ENABLE)

typedef struct{
  float upper_left_x_position;
  float upper_left_y_position;
  float buttom_right_x_position;
  float buttom_right_y_position;
}laser_border_xy_position_t;

#define LASER_FOCUS_STEP    2
const uint8_t laser_path[][20] = {
  {"M3 S0"},
  {"M106 S255"},
  {"G90"},
  {"G0 F4000"},
  {"G1 F2450"},
  {"M3 S1000"},
  {"G0 X121.691 Y0.330"},
  {"G1 X109.610 Y0.330"},
  {"G1 X107.462 Y12.087"},
  {"G1 X107.579 Y12.030"},
  {"G1 X107.694 Y11.975"},
  {"G1 X107.809 Y11.922"},
  {"G1 X107.923 Y11.872"},
  {"G1 X108.037 Y11.823"},
  {"G1 X108.149 Y11.777"},
  {"G1 X108.260 Y11.732"},
  {"G1 X108.371 Y11.690"},
  {"G1 X108.481 Y11.649"},
  {"G1 X108.590 Y11.611"},
  {"G1 X108.698 Y11.574"},
  {"G1 X108.805 Y11.539"},
  {"G1 X108.911 Y11.506"},
  {"G1 X109.016 Y11.474"},
  {"G1 X109.120 Y11.444"},
  {"G1 X109.223 Y11.416"},
  {"G1 X109.325 Y11.389"},
  {"G1 X109.425 Y11.364"},
  {"G1 X109.545 Y11.336"},
  {"G1 X109.663 Y11.310"},
  {"G1 X109.780 Y11.285"},
  {"G1 X109.895 Y11.263"},
  {"G1 X110.008 Y11.243"},
  {"G1 X110.120 Y11.224"},
  {"G1 X110.230 Y11.207"},
  {"G1 X110.338 Y11.191"},
  {"G1 X110.445 Y11.177"},
  {"G1 X110.550 Y11.165"},
  {"G1 X110.653 Y11.154"},
  {"G1 X110.754 Y11.144"},
  {"G1 X110.870 Y11.134"},
  {"G1 X110.983 Y11.125"},
  {"G1 X111.093 Y11.118"},
  {"G1 X111.201 Y11.112"},
  {"G1 X111.306 Y11.107"},
  {"G1 X111.409 Y11.103"},
  {"G1 X111.522 Y11.100"},
  {"G1 X111.632 Y11.098"},
  {"G1 X111.738 Y11.097"},
  {"G1 X111.841 Y11.096"},
  {"G1 X111.951 Y11.096"},
  {"G1 X112.057 Y11.095"},
  {"G1 X112.183 Y11.096"},
  {"G1 X112.297 Y11.099"},
  {"G1 X112.411 Y11.102"},
  {"G1 X112.524 Y11.108"},
  {"G1 X112.637 Y11.115"},
  {"G1 X112.750 Y11.123"},
  {"G1 X112.863 Y11.133"},
  {"G1 X112.975 Y11.144"},
  {"G1 X113.086 Y11.157"},
  {"G1 X113.197 Y11.171"},
  {"G1 X113.308 Y11.187"},
  {"G1 X113.418 Y11.204"},
  {"G1 X113.528 Y11.223"},
  {"G1 X113.638 Y11.243"},
  {"G1 X113.746 Y11.265"},
  {"G1 X113.855 Y11.287"},
  {"G1 X113.963 Y11.312"},
  {"G1 X114.070 Y11.337"},
  {"G1 X114.177 Y11.364"},
  {"G1 X114.284 Y11.393"},
  {"G1 X114.390 Y11.423"},
  {"G1 X114.495 Y11.454"},
  {"G1 X114.600 Y11.486"},
  {"G1 X114.704 Y11.520"},
  {"G1 X114.808 Y11.555"},
  {"G1 X114.911 Y11.592"},
  {"G1 X115.014 Y11.630"},
  {"G1 X115.116 Y11.669"},
  {"G1 X115.218 Y11.709"},
  {"G1 X115.319 Y11.751"},
  {"G1 X115.419 Y11.794"},
  {"G1 X115.568 Y11.860"},
  {"G1 X115.716 Y11.930"},
  {"G1 X115.863 Y12.002"},
  {"G1 X116.008 Y12.077"},
  {"G1 X116.151 Y12.155"},
  {"G1 X116.293 Y12.235"},
  {"G1 X116.433 Y12.319"},
  {"G1 X116.572 Y12.404"},
  {"G1 X116.709 Y12.493"},
  {"G1 X116.845 Y12.583"},
  {"G1 X116.979 Y12.677"},
  {"G1 X117.111 Y12.773"},
  {"G1 X117.242 Y12.871"},
  {"G1 X117.370 Y12.972"},
  {"G1 X117.497 Y13.075"},
  {"G1 X117.622 Y13.180"},
  {"G1 X117.745 Y13.288"},
  {"G1 X117.867 Y13.398"},
  {"G1 X117.986 Y13.511"},
  {"G1 X118.104 Y13.625"},
  {"G1 X118.219 Y13.742"},
  {"G1 X118.333 Y13.861"},
  {"G1 X118.444 Y13.982"},
  {"G1 X118.554 Y14.106"},
  {"G1 X118.661 Y14.231"},
  {"G1 X118.766 Y14.358"},
  {"G1 X118.869 Y14.488"},
  {"G1 X118.970 Y14.619"},
  {"G1 X119.069 Y14.753"},
  {"G1 X119.165 Y14.888"},
  {"G1 X119.259 Y15.025"},
  {"G1 X119.351 Y15.164"},
  {"G1 X119.441 Y15.305"},
  {"G1 X119.528 Y15.448"},
  {"G1 X119.613 Y15.592"},
  {"G1 X119.695 Y15.738"},
  {"G1 X119.775 Y15.886"},
  {"G1 X119.853 Y16.036"},
  {"G1 X119.903 Y16.136"},
  {"G1 X119.952 Y16.238"},
  {"G1 X120.000 Y16.340"},
  {"G1 X120.047 Y16.442"},
  {"G1 X120.093 Y16.546"},
  {"G1 X120.137 Y16.650"},
  {"G1 X120.181 Y16.755"},
  {"G1 X120.223 Y16.860"},
  {"G1 X120.264 Y16.966"},
  {"G1 X120.304 Y17.073"},
  {"G1 X120.342 Y17.180"},
  {"G1 X120.380 Y17.289"},
  {"G1 X120.416 Y17.397"},
  {"G1 X120.451 Y17.506"},
  {"G1 X120.485 Y17.616"},
  {"G1 X120.517 Y17.727"},
  {"G1 X120.549 Y17.838"},
  {"G1 X120.579 Y17.949"},
  {"G1 X120.607 Y18.061"},
  {"G1 X120.635 Y18.174"},
  {"G1 X120.661 Y18.287"},
  {"G1 X120.686 Y18.401"},
  {"G1 X120.709 Y18.515"},
  {"G1 X120.731 Y18.630"},
  {"G1 X120.752 Y18.746"},
  {"G1 X120.772 Y18.861"},
  {"G1 X120.790 Y18.978"},
  {"G1 X120.807 Y19.094"},
  {"G1 X120.823 Y19.212"},
  {"G1 X120.837 Y19.329"},
  {"G1 X120.850 Y19.448"},
  {"G1 X120.861 Y19.566"},
  {"G1 X120.871 Y19.685"},
  {"G1 X120.880 Y19.805"},
  {"G1 X120.887 Y19.925"},
  {"G1 X120.893 Y20.045"},
  {"G1 X120.897 Y20.165"},
  {"G1 X120.900 Y20.287"},
  {"G1 X120.901 Y20.408"},
  {"G1 X120.901 Y20.530"},
  {"G1 X120.900 Y20.651"},
  {"G1 X120.897 Y20.772"},
  {"G1 X120.893 Y20.893"},
  {"G1 X120.887 Y21.013"},
  {"G1 X120.880 Y21.133"},
  {"G1 X120.871 Y21.252"},
  {"G1 X120.861 Y21.371"},
  {"G1 X120.850 Y21.490"},
  {"G1 X120.837 Y21.608"},
  {"G1 X120.823 Y21.726"},
  {"G1 X120.807 Y21.843"},
  {"G1 X120.790 Y21.960"},
  {"G1 X120.772 Y22.076"},
  {"G1 X120.753 Y22.192"},
  {"G1 X120.732 Y22.307"},
  {"G1 X120.710 Y22.422"},
  {"G1 X120.686 Y22.537"},
  {"G1 X120.661 Y22.650"},
  {"G1 X120.635 Y22.764"},
  {"G1 X120.608 Y22.876"},
  {"G1 X120.579 Y22.988"},
  {"G1 X120.549 Y23.100"},
  {"G1 X120.518 Y23.211"},
  {"G1 X120.485 Y23.322"},
  {"G1 X120.452 Y23.431"},
  {"G1 X120.417 Y23.541"},
  {"G1 X120.381 Y23.649"},
  {"G1 X120.343 Y23.757"},
  {"G1 X120.305 Y23.865"},
  {"G1 X120.265 Y23.972"},
  {"G1 X120.224 Y24.078"},
  {"G1 X120.182 Y24.183"},
  {"G1 X120.138 Y24.288"},
  {"G1 X120.094 Y24.392"},
  {"G1 X120.048 Y24.496"},
  {"G1 X120.001 Y24.598"},
  {"G1 X119.953 Y24.700"},
  {"G1 X119.904 Y24.802"},
  {"G1 X119.854 Y24.902"},
  {"G1 X119.777 Y25.052"},
  {"G1 X119.697 Y25.200"},
  {"G1 X119.614 Y25.346"},
  {"G1 X119.530 Y25.490"},
  {"G1 X119.443 Y25.633"},
  {"G1 X119.353 Y25.774"},
  {"G1 X119.261 Y25.913"},
  {"G1 X119.167 Y26.050"},
  {"G1 X119.071 Y26.186"},
  {"G1 X118.972 Y26.319"},
  {"G1 X118.871 Y26.451"},
  {"G1 X118.768 Y26.580"},
  {"G1 X118.663 Y26.707"},
  {"G1 X118.556 Y26.833"},
  {"G1 X118.447 Y26.956"},
  {"G1 X118.335 Y27.077"},
  {"G1 X118.222 Y27.196"},
  {"G1 X118.106 Y27.313"},
  {"G1 X117.989 Y27.428"},
  {"G1 X117.869 Y27.540"},
  {"G1 X117.748 Y27.650"},
  {"G1 X117.625 Y27.758"},
  {"G1 X117.500 Y27.864"},
  {"G1 X117.373 Y27.967"},
  {"G1 X117.244 Y28.068"},
  {"G1 X117.114 Y28.166"},
  {"G1 X116.982 Y28.262"},
  {"G1 X116.848 Y28.355"},
  {"G1 X116.712 Y28.446"},
  {"G1 X116.575 Y28.535"},
  {"G1 X116.436 Y28.620"},
  {"G1 X116.296 Y28.703"},
  {"G1 X116.154 Y28.784"},
  {"G1 X116.010 Y28.862"},
  {"G1 X115.865 Y28.937"},
  {"G1 X115.719 Y29.009"},
  {"G1 X115.571 Y29.079"},
  {"G1 X115.422 Y29.145"},
  {"G1 X115.321 Y29.188"},
  {"G1 X115.220 Y29.230"},
  {"G1 X115.119 Y29.270"},
  {"G1 X115.017 Y29.309"},
  {"G1 X114.914 Y29.347"},
  {"G1 X114.811 Y29.384"},
  {"G1 X114.707 Y29.419"},
  {"G1 X114.602 Y29.453"},
  {"G1 X114.497 Y29.485"},
  {"G1 X114.392 Y29.516"},
  {"G1 X114.286 Y29.546"},
  {"G1 X114.179 Y29.575"},
  {"G1 X114.072 Y29.602"},
  {"G1 X113.965 Y29.627"},
  {"G1 X113.857 Y29.652"},
  {"G1 X113.748 Y29.675"},
  {"G1 X113.639 Y29.696"},
  {"G1 X113.530 Y29.716"},
  {"G1 X113.420 Y29.735"},
  {"G1 X113.309 Y29.752"},
  {"G1 X113.199 Y29.768"},
  {"G1 X113.087 Y29.782"},
  {"G1 X112.976 Y29.795"},
  {"G1 X112.864 Y29.806"},
  {"G1 X112.751 Y29.816"},
  {"G1 X112.638 Y29.825"},
  {"G1 X112.525 Y29.831"},
  {"G1 X112.411 Y29.837"},
  {"G1 X112.297 Y29.841"},
  {"G1 X112.183 Y29.843"},
  {"G1 X112.068 Y29.844"},
  {"G1 X111.922 Y29.842"},
  {"G1 X111.776 Y29.839"},
  {"G1 X111.631 Y29.832"},
  {"G1 X111.487 Y29.824"},
  {"G1 X111.343 Y29.813"},
  {"G1 X111.200 Y29.799"},
  {"G1 X111.057 Y29.783"},
  {"G1 X110.915 Y29.764"},
  {"G1 X110.774 Y29.744"},
  {"G1 X110.634 Y29.721"},
  {"G1 X110.494 Y29.695"},
  {"G1 X110.355 Y29.667"},
  {"G1 X110.217 Y29.637"},
  {"G1 X110.080 Y29.605"},
  {"G1 X109.943 Y29.570"},
  {"G1 X109.808 Y29.533"},
  {"G1 X109.673 Y29.494"},
  {"G1 X109.539 Y29.453"},
  {"G1 X109.406 Y29.410"},
  {"G1 X109.274 Y29.364"},
  {"G1 X109.142 Y29.316"},
  {"G1 X109.012 Y29.267"},
  {"G1 X108.883 Y29.215"},
  {"G1 X108.754 Y29.161"},
  {"G1 X108.627 Y29.105"},
  {"G1 X108.500 Y29.046"},
  {"G1 X108.375 Y28.986"},
  {"G1 X108.251 Y28.924"},
  {"G1 X108.127 Y28.860"},
  {"G1 X108.005 Y28.794"},
  {"G1 X107.884 Y28.726"},
  {"G1 X107.764 Y28.656"},
  {"G1 X107.645 Y28.584"},
  {"G1 X107.528 Y28.510"},
  {"G1 X107.411 Y28.435"},
  {"G1 X107.296 Y28.357"},
  {"G1 X107.182 Y28.278"},
  {"G1 X107.069 Y28.197"},
  {"G1 X106.958 Y28.114"},
  {"G1 X106.847 Y28.030"},
  {"G1 X106.738 Y27.943"},
  {"G1 X106.631 Y27.855"},
  {"G1 X106.524 Y27.765"},
  {"G1 X106.419 Y27.674"},
  {"G1 X106.315 Y27.581"},
  {"G1 X106.213 Y27.486"},
  {"G1 X106.112 Y27.389"},
  {"G1 X105.980 Y27.258"},
  {"G1 X105.882 Y27.158"},
  {"G1 X105.786 Y27.056"},
  {"G1 X105.691 Y26.953"},
  {"G1 X105.598 Y26.848"},
  {"G1 X105.506 Y26.742"},
  {"G1 X105.416 Y26.634"},
  {"G1 X105.328 Y26.525"},
  {"G1 X105.240 Y26.414"},
  {"G1 X105.155 Y26.302"},
  {"G1 X105.071 Y26.188"},
  {"G1 X104.989 Y26.073"},
  {"G1 X104.908 Y25.957"},
  {"G1 X104.829 Y25.839"},
  {"G1 X104.752 Y25.720"},
  {"G1 X104.676 Y25.600"},
  {"G1 X104.603 Y25.478"},
  {"G1 X104.530 Y25.355"},
  {"G1 X104.460 Y25.231"},
  {"G1 X104.391 Y25.106"},
  {"G1 X104.324 Y24.979"},
  {"G1 X104.259 Y24.851"},
  {"G1 X104.196 Y24.723"},
  {"G1 X104.135 Y24.592"},
  {"G1 X104.075 Y24.461"},
  {"G1 X104.018 Y24.329"},
  {"G1 X103.962 Y24.195"},
  {"G1 X103.908 Y24.061"},
  {"G1 X103.856 Y23.925"},
  {"G1 X103.806 Y23.788"},
  {"G1 X103.758 Y23.651"},
  {"G1 X103.712 Y23.512"},
  {"G1 X103.668 Y23.372"},
  {"G1 X103.626 Y23.232"},
  {"G1 X103.586 Y23.090"},
  {"G1 X103.548 Y22.948"},
  {"G1 X103.512 Y22.804"},

  {"G0 X94.866 Y20.741"},
  {"G1 X74.691 Y20.741"},
  {"G1 X89.242 Y0.250"},
  {"G1 X89.242 Y29.924"},

  {"G0 X50.005 Y6.653"},
  {"G1 X50.006 Y6.530"},
  {"G1 X50.010 Y6.408"},
  {"G1 X50.017 Y6.286"},
  {"G1 X50.027 Y6.165"},
  {"G1 X50.038 Y6.044"},
  {"G1 X50.053 Y5.924"},
  {"G1 X50.070 Y5.805"},
  {"G1 X50.090 Y5.687"},
  {"G1 X50.112 Y5.569"},
  {"G1 X50.137 Y5.452"},
  {"G1 X50.164 Y5.335"},
  {"G1 X50.194 Y5.220"},
  {"G1 X50.226 Y5.105"},
  {"G1 X50.260 Y4.991"},
  {"G1 X50.297 Y4.878"},
  {"G1 X50.337 Y4.766"},
  {"G1 X50.378 Y4.655"},
  {"G1 X50.422 Y4.544"},
  {"G1 X50.468 Y4.435"},
  {"G1 X50.516 Y4.326"},
  {"G1 X50.567 Y4.219"},
  {"G1 X50.620 Y4.112"},
  {"G1 X50.675 Y4.007"},
  {"G1 X50.732 Y3.902"},
  {"G1 X50.791 Y3.799"},
  {"G1 X50.853 Y3.696"},
  {"G1 X50.916 Y3.595"},
  {"G1 X50.982 Y3.495"},
  {"G1 X51.072 Y3.363"},
  {"G1 X51.166 Y3.233"},
  {"G1 X51.264 Y3.106"},
  {"G1 X51.365 Y2.981"},
  {"G1 X51.470 Y2.857"},
  {"G1 X51.577 Y2.737"},
  {"G1 X51.688 Y2.618"},
  {"G1 X51.803 Y2.502"},
  {"G1 X51.920 Y2.388"},
  {"G1 X52.041 Y2.277"},
  {"G1 X52.164 Y2.169"},
  {"G1 X52.291 Y2.062"},
  {"G1 X52.420 Y1.959"},
  {"G1 X52.552 Y1.858"},
  {"G1 X52.653 Y1.785"},
  {"G1 X52.756 Y1.712"},
  {"G1 X52.860 Y1.642"},
  {"G1 X52.966 Y1.573"},
  {"G1 X53.073 Y1.505"},
  {"G1 X53.181 Y1.440"},
  {"G1 X53.291 Y1.375"},
  {"G1 X53.403 Y1.313"},
  {"G1 X53.516 Y1.252"},
  {"G1 X53.630 Y1.193"},
  {"G1 X53.745 Y1.136"},
  {"G1 X53.862 Y1.081"},
  {"G1 X53.980 Y1.027"},
  {"G1 X54.099 Y0.975"},
  {"G1 X54.220 Y0.925"},
  {"G1 X54.342 Y0.877"},
  {"G1 X54.465 Y0.831"},
  {"G1 X54.589 Y0.786"},
  {"G1 X54.714 Y0.744"},
  {"G1 X54.840 Y0.703"},
  {"G1 X54.968 Y0.665"},
  {"G1 X55.096 Y0.628"},
  {"G1 X55.226 Y0.594"},
  {"G1 X55.357 Y0.561"},
  {"G1 X55.488 Y0.531"},
  {"G1 X55.621 Y0.502"},
  {"G1 X55.754 Y0.476"},
  {"G1 X55.888 Y0.452"},
  {"G1 X56.024 Y0.430"},
  {"G1 X56.160 Y0.410"},
  {"G1 X56.297 Y0.392"},
  {"G1 X56.435 Y0.376"},
  {"G1 X56.573 Y0.363"},
  {"G1 X56.712 Y0.352"},
  {"G1 X56.853 Y0.343"},
  {"G1 X56.993 Y0.336"},
  {"G1 X57.135 Y0.332"},
  {"G1 X57.277 Y0.330"},
  {"G1 X57.420 Y0.331"},
  {"G1 X57.562 Y0.333"},
  {"G1 X57.703 Y0.338"},
  {"G1 X57.843 Y0.346"},
  {"G1 X57.983 Y0.355"},
  {"G1 X58.122 Y0.367"},
  {"G1 X58.261 Y0.381"},
  {"G1 X58.398 Y0.398"},
  {"G1 X58.535 Y0.416"},
  {"G1 X58.671 Y0.437"},
  {"G1 X58.806 Y0.460"},
  {"G1 X58.940 Y0.484"},
  {"G1 X59.073 Y0.512"},
  {"G1 X59.205 Y0.541"},
  {"G1 X59.336 Y0.572"},
  {"G1 X59.466 Y0.605"},
  {"G1 X59.596 Y0.640"},
  {"G1 X59.724 Y0.677"},
  {"G1 X59.851 Y0.717"},
  {"G1 X59.977 Y0.758"},
  {"G1 X60.102 Y0.801"},
  {"G1 X60.225 Y0.846"},
  {"G1 X60.348 Y0.893"},
  {"G1 X60.469 Y0.942"},
  {"G1 X60.590 Y0.992"},
  {"G1 X60.708 Y1.045"},
  {"G1 X60.826 Y1.099"},
  {"G1 X60.942 Y1.155"},
  {"G1 X61.058 Y1.213"},
  {"G1 X61.171 Y1.272"},
  {"G1 X61.284 Y1.334"},
  {"G1 X61.394 Y1.397"},
  {"G1 X61.504 Y1.461"},
  {"G1 X61.612 Y1.528"},
  {"G1 X61.719 Y1.595"},
  {"G1 X61.824 Y1.665"},
  {"G1 X61.928 Y1.736"},
  {"G1 X62.030 Y1.809"},
  {"G1 X62.130 Y1.883"},
  {"G1 X62.262 Y1.985"},
  {"G1 X62.390 Y2.089"},
  {"G1 X62.516 Y2.195"},
  {"G1 X62.639 Y2.305"},
  {"G1 X62.759 Y2.417"},
  {"G1 X62.875 Y2.531"},
  {"G1 X62.989 Y2.648"},
  {"G1 X63.099 Y2.767"},
  {"G1 X63.206 Y2.888"},
  {"G1 X63.310 Y3.012"},
  {"G1 X63.410 Y3.138"},
  {"G1 X63.506 Y3.266"},
  {"G1 X63.600 Y3.396"},
  {"G1 X63.689 Y3.528"},
  {"G1 X63.754 Y3.629"},
  {"G1 X63.817 Y3.730"},
  {"G1 X63.878 Y3.833"},
  {"G1 X63.936 Y3.937"},
  {"G1 X63.993 Y4.042"},
  {"G1 X64.047 Y4.147"},
  {"G1 X64.099 Y4.254"},
  {"G1 X64.149 Y4.362"},
  {"G1 X64.197 Y4.471"},
  {"G1 X64.242 Y4.581"},
  {"G1 X64.285 Y4.692"},
  {"G1 X64.326 Y4.803"},
  {"G1 X64.364 Y4.916"},
  {"G1 X64.400 Y5.029"},
  {"G1 X64.434 Y5.143"},
  {"G1 X64.465 Y5.258"},
  {"G1 X64.494 Y5.374"},
  {"G1 X64.521 Y5.491"},
  {"G1 X64.545 Y5.608"},
  {"G1 X64.566 Y5.726"},
  {"G1 X64.585 Y5.845"},
  {"G1 X64.601 Y5.964"},
  {"G1 X64.615 Y6.084"},
  {"G1 X64.626 Y6.205"},
  {"G1 X64.634 Y6.327"},
  {"G1 X64.640 Y6.449"},
  {"G1 X64.643 Y6.571"},
  {"G1 X64.644 Y6.694"},
  {"G1 X64.642 Y6.817"},
  {"G1 X64.637 Y6.939"},
  {"G1 X64.629 Y7.061"},
  {"G1 X64.619 Y7.182"},
  {"G1 X64.606 Y7.302"},
  {"G1 X64.591 Y7.422"},
  {"G1 X64.573 Y7.541"},
  {"G1 X64.552 Y7.659"},
  {"G1 X64.529 Y7.777"},
  {"G1 X64.504 Y7.894"},
  {"G1 X64.476 Y8.010"},
  {"G1 X64.445 Y8.125"},
  {"G1 X64.413 Y8.239"},
  {"G1 X64.377 Y8.353"},
  {"G1 X64.340 Y8.466"},
  {"G1 X64.300 Y8.578"},
  {"G1 X64.258 Y8.689"},
  {"G1 X64.213 Y8.799"},
  {"G1 X64.167 Y8.908"},
  {"G1 X64.118 Y9.016"},
  {"G1 X64.066 Y9.123"},
  {"G1 X64.013 Y9.229"},
  {"G1 X63.957 Y9.334"},
  {"G1 X63.900 Y9.439"},
  {"G1 X63.840 Y9.542"},
  {"G1 X63.778 Y9.644"},
  {"G1 X63.714 Y9.745"},
  {"G1 X63.625 Y9.877"},
  {"G1 X63.533 Y10.008"},
  {"G1 X63.437 Y10.136"},
  {"G1 X63.338 Y10.263"},
  {"G1 X63.236 Y10.387"},
  {"G1 X63.130 Y10.509"},
  {"G1 X63.021 Y10.629"},
  {"G1 X62.908 Y10.746"},
  {"G1 X62.792 Y10.861"},
  {"G1 X62.674 Y10.973"},
  {"G1 X62.552 Y11.083"},
  {"G1 X62.427 Y11.190"},
  {"G1 X62.299 Y11.295"},
  {"G1 X62.169 Y11.397"},
  {"G1 X62.035 Y11.496"},
  {"G1 X61.933 Y11.569"},
  {"G1 X61.830 Y11.640"},
  {"G1 X61.725 Y11.710"},
  {"G1 X61.618 Y11.778"},
  {"G1 X61.510 Y11.844"},
  {"G1 X61.401 Y11.909"},
  {"G1 X61.290 Y11.972"},
  {"G1 X61.178 Y12.033"},
  {"G1 X61.064 Y12.092"},
  {"G1 X60.949 Y12.150"},
  {"G1 X60.833 Y12.206"},
  {"G1 X60.715 Y12.260"},
  {"G1 X60.596 Y12.313"},
  {"G1 X60.476 Y12.363"},
  {"G1 X60.355 Y12.412"},
  {"G1 X60.232 Y12.459"},
  {"G1 X60.109 Y12.504"},
  {"G1 X59.984 Y12.547"},
  {"G1 X59.858 Y12.588"},
  {"G1 X59.731 Y12.627"},
  {"G1 X59.602 Y12.665"},
  {"G1 X59.473 Y12.700"},
  {"G1 X59.343 Y12.733"},
  {"G1 X59.212 Y12.764"},
  {"G1 X59.079 Y12.793"},
  {"G1 X58.946 Y12.820"},
  {"G1 X58.812 Y12.845"},
  {"G1 X58.677 Y12.868"},
  {"G1 X58.541 Y12.889"},
  {"G1 X58.404 Y12.907"},
  {"G1 X58.267 Y12.924"},
  {"G1 X58.128 Y12.938"},
  {"G1 X57.989 Y12.949"},
  {"G1 X57.849 Y12.959"},
  {"G1 X57.708 Y12.966"},
  {"G1 X57.567 Y12.971"},
  {"G1 X57.425 Y12.974"},
  {"G1 X57.510 Y12.975"},
  {"G1 X57.568 Y12.975"},
  {"G1 X57.682 Y12.976"},
  {"G1 X57.796 Y12.979"},
  {"G1 X57.909 Y12.983"},
  {"G1 X58.022 Y12.989"},
  {"G1 X58.135 Y12.996"},
  {"G1 X58.247 Y13.004"},
  {"G1 X58.359 Y13.013"},
  {"G1 X58.471 Y13.024"},
  {"G1 X58.582 Y13.036"},
  {"G1 X58.693 Y13.050"},
  {"G1 X58.803 Y13.065"},
  {"G1 X58.913 Y13.081"},
  {"G1 X59.022 Y13.098"},
  {"G1 X59.131 Y13.117"},
  {"G1 X59.239 Y13.137"},
  {"G1 X59.347 Y13.158"},
  {"G1 X59.455 Y13.181"},
  {"G1 X59.562 Y13.204"},
  {"G1 X59.668 Y13.229"},
  {"G1 X59.774 Y13.256"},
  {"G1 X59.880 Y13.283"},
  {"G1 X59.985 Y13.312"},
  {"G1 X60.089 Y13.341"},
  {"G1 X60.193 Y13.372"},
  {"G1 X60.296 Y13.405"},
  {"G1 X60.399 Y13.438"},
  {"G1 X60.501 Y13.473"},
  {"G1 X60.603 Y13.508"},
  {"G1 X60.704 Y13.545"},
  {"G1 X60.804 Y13.583"},
  {"G1 X60.953 Y13.643"},
  {"G1 X61.101 Y13.704"},
  {"G1 X61.248 Y13.768"},
  {"G1 X61.393 Y13.835"},
  {"G1 X61.537 Y13.904"},
  {"G1 X61.679 Y13.976"},
  {"G1 X61.820 Y14.050"},
  {"G1 X61.959 Y14.126"},
  {"G1 X62.096 Y14.205"},
  {"G1 X62.232 Y14.286"},
  {"G1 X62.366 Y14.369"},
  {"G1 X62.498 Y14.455"},
  {"G1 X62.629 Y14.542"},
  {"G1 X62.758 Y14.632"},
  {"G1 X62.885 Y14.724"},
  {"G1 X63.010 Y14.818"},
  {"G1 X63.134 Y14.915"},
  {"G1 X63.256 Y15.013"},
  {"G1 X63.375 Y15.114"},
  {"G1 X63.493 Y15.216"},
  {"G1 X63.609 Y15.321"},
  {"G1 X63.723 Y15.427"},
  {"G1 X63.835 Y15.535"},
  {"G1 X63.944 Y15.646"},
  {"G1 X64.052 Y15.758"},
  {"G1 X64.158 Y15.872"},
  {"G1 X64.261 Y15.988"},
  {"G1 X64.363 Y16.105"},
  {"G1 X64.462 Y16.225"},
  {"G1 X64.559 Y16.346"},
  {"G1 X64.653 Y16.469"},
  {"G1 X64.746 Y16.593"},
  {"G1 X64.836 Y16.720"},
  {"G1 X64.924 Y16.848"},
  {"G1 X65.009 Y16.977"},
  {"G1 X65.092 Y17.108"},
  {"G1 X65.172 Y17.241"},
  {"G1 X65.250 Y17.375"},
  {"G1 X65.326 Y17.510"},
  {"G1 X65.399 Y17.647"},
  {"G1 X65.470 Y17.786"},
  {"G1 X65.538 Y17.925"},
  {"G1 X65.603 Y18.067"},
  {"G1 X65.666 Y18.209"},
  {"G1 X65.726 Y18.353"},
  {"G1 X65.783 Y18.498"},
  {"G1 X65.838 Y18.645"},
  {"G1 X65.889 Y18.792"},
  {"G1 X65.938 Y18.941"},
  {"G1 X65.985 Y19.091"},
  {"G1 X66.014 Y19.192"},
  {"G1 X66.042 Y19.293"},
  {"G1 X66.069 Y19.395"},
  {"G1 X66.094 Y19.497"},
  {"G1 X66.118 Y19.600"},
  {"G1 X66.141 Y19.703"},
  {"G1 X66.163 Y19.806"},
  {"G1 X66.183 Y19.910"},
  {"G1 X66.202 Y20.015"},
  {"G1 X66.219 Y20.120"},
  {"G1 X66.235 Y20.225"},
  {"G1 X66.250 Y20.331"},
  {"G1 X66.264 Y20.437"},
  {"G1 X66.276 Y20.543"},
  {"G1 X66.286 Y20.650"},
  {"G1 X66.296 Y20.758"},
  {"G1 X66.303 Y20.865"},
  {"G1 X66.310 Y20.973"},
  {"G1 X66.315 Y21.082"},
  {"G1 X66.319 Y21.191"},
  {"G1 X66.321 Y21.300"},
  {"G1 X66.321 Y21.409"},
  {"G1 X66.321 Y21.519"},
  {"G1 X66.319 Y21.628"},
  {"G1 X66.315 Y21.737"},
  {"G1 X66.310 Y21.845"},
  {"G1 X66.303 Y21.953"},
  {"G1 X66.295 Y22.061"},
  {"G1 X66.286 Y22.168"},
  {"G1 X66.276 Y22.275"},
  {"G1 X66.263 Y22.382"},
  {"G1 X66.250 Y22.488"},
  {"G1 X66.235 Y22.593"},
  {"G1 X66.219 Y22.699"},
  {"G1 X66.202 Y22.804"},
  {"G1 X66.183 Y22.908"},
  {"G1 X66.162 Y23.012"},
  {"G1 X66.141 Y23.116"},
  {"G1 X66.118 Y23.219"},
  {"G1 X66.094 Y23.321"},
  {"G1 X66.068 Y23.423"},
  {"G1 X66.042 Y23.525"},
  {"G1 X66.014 Y23.626"},
  {"G1 X65.984 Y23.727"},
  {"G1 X65.954 Y23.827"},
  {"G1 X65.906 Y23.976"},
  {"G1 X65.855 Y24.125"},
  {"G1 X65.801 Y24.271"},
  {"G1 X65.744 Y24.417"},
  {"G1 X65.685 Y24.561"},
  {"G1 X65.623 Y24.704"},
  {"G1 X65.559 Y24.846"},
  {"G1 X65.492 Y24.986"},
  {"G1 X65.422 Y25.125"},
  {"G1 X65.350 Y25.263"},
  {"G1 X65.275 Y25.399"},
  {"G1 X65.198 Y25.533"},
  {"G1 X65.118 Y25.666"},
  {"G1 X65.036 Y25.798"},
  {"G1 X64.951 Y25.928"},
  {"G1 X64.864 Y26.056"},
  {"G1 X64.775 Y26.183"},
  {"G1 X64.683 Y26.308"},
  {"G1 X64.589 Y26.432"},
  {"G1 X64.493 Y26.553"},
  {"G1 X64.394 Y26.673"},
  {"G1 X64.294 Y26.792"},
  {"G1 X64.191 Y26.908"},
  {"G1 X64.086 Y27.023"},
  {"G1 X63.979 Y27.135"},
  {"G1 X63.870 Y27.246"},
  {"G1 X63.759 Y27.355"},
  {"G1 X63.645 Y27.463"},
  {"G1 X63.530 Y27.568"},
  {"G1 X63.413 Y27.671"},
  {"G1 X63.294 Y27.772"},
  {"G1 X63.173 Y27.871"},
  {"G1 X63.050 Y27.968"},
  {"G1 X62.925 Y28.063"},
  {"G1 X62.798 Y28.156"},
  {"G1 X62.670 Y28.246"},
  {"G1 X62.540 Y28.335"},
  {"G1 X62.408 Y28.421"},
  {"G1 X62.274 Y28.505"},
  {"G1 X62.139 Y28.587"},
  {"G1 X62.002 Y28.666"},
  {"G1 X61.864 Y28.743"},
  {"G1 X61.724 Y28.818"},
  {"G1 X61.582 Y28.890"},
  {"G1 X61.439 Y28.960"},
  {"G1 X61.294 Y29.028"},
  {"G1 X61.148 Y29.093"},
  {"G1 X61.001 Y29.156"},
  {"G1 X60.852 Y29.216"},
  {"G1 X60.752 Y29.254"},
  {"G1 X60.651 Y29.292"},
  {"G1 X60.550 Y29.328"},
  {"G1 X60.448 Y29.363"},
  {"G1 X60.346 Y29.397"},
  {"G1 X60.243 Y29.430"},
  {"G1 X60.139 Y29.462"},
  {"G1 X60.035 Y29.492"},
  {"G1 X59.931 Y29.521"},
  {"G1 X59.826 Y29.549"},
  {"G1 X59.720 Y29.576"},
  {"G1 X59.614 Y29.602"},
  {"G1 X59.507 Y29.626"},
  {"G1 X59.400 Y29.649"},
  {"G1 X59.292 Y29.671"},
  {"G1 X59.184 Y29.692"},
  {"G1 X59.075 Y29.711"},
  {"G1 X58.966 Y29.729"},
  {"G1 X58.857 Y29.746"},
  {"G1 X58.747 Y29.761"},
  {"G1 X58.636 Y29.775"},
  {"G1 X58.526 Y29.788"},
  {"G1 X58.414 Y29.800"},
  {"G1 X58.303 Y29.810"},
  {"G1 X58.191 Y29.819"},
  {"G1 X58.078 Y29.826"},
  {"G1 X57.965 Y29.833"},
  {"G1 X57.852 Y29.837"},
  {"G1 X57.739 Y29.841"},
  {"G1 X57.625 Y29.843"},
  {"G1 X57.510 Y29.844"},
  {"G1 X57.403 Y29.843"},
  {"G1 X57.296 Y29.841"},
  {"G1 X57.190 Y29.838"},
  {"G1 X57.083 Y29.834"},
  {"G1 X56.977 Y29.829"},
  {"G1 X56.872 Y29.822"},
  {"G1 X56.766 Y29.814"},
  {"G1 X56.661 Y29.805"},
  {"G1 X56.557 Y29.795"},
  {"G1 X56.453 Y29.784"},
  {"G1 X56.349 Y29.771"},
  {"G1 X56.245 Y29.757"},
  {"G1 X56.142 Y29.743"},
  {"G1 X56.039 Y29.727"},
  {"G1 X55.937 Y29.710"},
  {"G1 X55.835 Y29.692"},
  {"G1 X55.734 Y29.672"},
  {"G1 X55.633 Y29.652"},
  {"G1 X55.532 Y29.630"},
  {"G1 X55.432 Y29.608"},
  {"G1 X55.282 Y29.572"},
  {"G1 X55.134 Y29.534"},
  {"G1 X54.987 Y29.493"},
  {"G1 X54.840 Y29.450"},
  {"G1 X54.695 Y29.404"},
  {"G1 X54.551 Y29.357"},
  {"G1 X54.408 Y29.307"},
  {"G1 X54.266 Y29.254"},
  {"G1 X54.126 Y29.200"},
  {"G1 X53.986 Y29.143"},
  {"G1 X53.848 Y29.084"},
  {"G1 X53.711 Y29.022"},
  {"G1 X53.576 Y28.959"},
  {"G1 X53.441 Y28.893"},
  {"G1 X53.308 Y28.826"},
  {"G1 X53.177 Y28.756"},
  {"G1 X53.047 Y28.684"},
  {"G1 X52.918 Y28.610"},
  {"G1 X52.790 Y28.534"},
  {"G1 X52.665 Y28.456"},
  {"G1 X52.540 Y28.376"},
  {"G1 X52.417 Y28.294"},
  {"G1 X52.296 Y28.211"},
  {"G1 X52.176 Y28.125"},
  {"G1 X52.058 Y28.037"},
  {"G1 X51.941 Y27.948"},
  {"G1 X51.826 Y27.856"},
  {"G1 X51.713 Y27.763"},
  {"G1 X51.601 Y27.668"},
  {"G1 X51.491 Y27.572"},
  {"G1 X51.383 Y27.473"},
  {"G1 X51.277 Y27.373"},
  {"G1 X51.172 Y27.271"},
  {"G1 X51.069 Y27.168"},
  {"G1 X50.968 Y27.062"},
  {"G1 X50.869 Y26.956"},
  {"G1 X50.771 Y26.847"},
  {"G1 X50.676 Y26.737"},
  {"G1 X50.582 Y26.626"},
  {"G1 X50.491 Y26.513"},
  {"G1 X50.401 Y26.398"},
  {"G1 X50.314 Y26.282"},
  {"G1 X50.228 Y26.164"},
  {"G1 X50.144 Y26.045"},
  {"G1 X50.063 Y25.925"},
  {"G1 X49.984 Y25.803"},
  {"G1 X49.906 Y25.680"},
  {"G1 X49.831 Y25.555"},
  {"G1 X49.758 Y25.429"},
  {"G1 X49.688 Y25.302"},
  {"G1 X49.619 Y25.174"},
  {"G1 X49.553 Y25.044"},
  {"G1 X49.489 Y24.913"},
  {"G1 X49.427 Y24.781"},
  {"G1 X49.367 Y24.647"},
  {"G1 X49.310 Y24.513"},
  {"G1 X49.256 Y24.377"},
  {"G1 X49.203 Y24.240"},
  {"G1 X49.153 Y24.102"},
  {"G1 X49.106 Y23.964"},
  {"G1 X49.061 Y23.824"},
  {"G1 X49.018 Y23.682"},
  {"G1 X48.978 Y23.540"},
  {"G1 X48.941 Y23.397"},
  {"G1 X48.906 Y23.253"},
  {"G1 X48.873 Y23.109"},
  {"G1 X48.843 Y22.963"},
  {"G1 X48.816 Y22.816"},
  {"G1 X48.792 Y22.669"},
  {"G1 X48.770 Y22.520"},
  {"G1 X48.751 Y22.371"},

  {"G0 X39.280 Y29.757"},
  {"G1 X21.233 Y29.807"},
  {"G1 X35.475 Y13.776"},
  {"G1 X35.557 Y13.669"},
  {"G1 X35.634 Y13.568"},
  {"G1 X35.713 Y13.464"},
  {"G1 X35.793 Y13.358"},
  {"G1 X35.874 Y13.252"},
  {"G1 X35.953 Y13.146"},
  {"G1 X36.032 Y13.040"},
  {"G1 X36.108 Y12.935"},
  {"G1 X36.182 Y12.831"},
  {"G1 X36.254 Y12.726"},
  {"G1 X36.320 Y12.624"},
  {"G1 X36.383 Y12.523"},
  {"G1 X36.443 Y12.420"},
  {"G1 X36.500 Y12.319"},
  {"G1 X36.553 Y12.218"},
  {"G1 X36.605 Y12.118"},
  {"G1 X36.655 Y12.017"},
  {"G1 X36.707 Y11.909"},
  {"G1 X36.760 Y11.796"},
  {"G1 X36.811 Y11.682"},
  {"G1 X36.860 Y11.567"},
  {"G1 X36.907 Y11.452"},
  {"G1 X36.952 Y11.336"},
  {"G1 X36.996 Y11.220"},
  {"G1 X37.037 Y11.103"},
  {"G1 X37.077 Y10.985"},
  {"G1 X37.114 Y10.868"},
  {"G1 X37.150 Y10.750"},
  {"G1 X37.183 Y10.631"},
  {"G1 X37.215 Y10.512"},
  {"G1 X37.245 Y10.393"},
  {"G1 X37.273 Y10.273"},
  {"G1 X37.299 Y10.153"},
  {"G1 X37.323 Y10.033"},
  {"G1 X37.345 Y9.913"},
  {"G1 X37.365 Y9.792"},
  {"G1 X37.383 Y9.671"},
  {"G1 X37.399 Y9.550"},
  {"G1 X37.414 Y9.429"},
  {"G1 X37.426 Y9.307"},
  {"G1 X37.437 Y9.186"},
  {"G1 X37.445 Y9.064"},
  {"G1 X37.452 Y8.942"},
  {"G1 X37.457 Y8.821"},
  {"G1 X37.460 Y8.699"},
  {"G1 X37.460 Y8.577"},
  {"G1 X37.459 Y8.455"},
  {"G1 X37.457 Y8.334"},
  {"G1 X37.452 Y8.212"},
  {"G1 X37.445 Y8.091"},
  {"G1 X37.436 Y7.969"},
  {"G1 X37.426 Y7.848"},
  {"G1 X37.413 Y7.727"},
  {"G1 X37.399 Y7.606"},
  {"G1 X37.383 Y7.485"},
  {"G1 X37.365 Y7.365"},
  {"G1 X37.345 Y7.244"},
  {"G1 X37.323 Y7.124"},
  {"G1 X37.299 Y7.005"},
  {"G1 X37.273 Y6.885"},
  {"G1 X37.245 Y6.766"},
  {"G1 X37.216 Y6.648"},
  {"G1 X37.184 Y6.530"},
  {"G1 X37.151 Y6.412"},
  {"G1 X37.116 Y6.294"},
  {"G1 X37.079 Y6.177"},
  {"G1 X37.040 Y6.061"},
  {"G1 X36.999 Y5.945"},
  {"G1 X36.956 Y5.830"},
  {"G1 X36.911 Y5.715"},
  {"G1 X36.865 Y5.601"},
  {"G1 X36.816 Y5.487"},
  {"G1 X36.766 Y5.374"},
  {"G1 X36.714 Y5.262"},
  {"G1 X36.660 Y5.150"},
  {"G1 X36.604 Y5.039"},
  {"G1 X36.546 Y4.929"},
  {"G1 X36.486 Y4.819"},
  {"G1 X36.424 Y4.710"},
  {"G1 X36.361 Y4.602"},
  {"G1 X36.296 Y4.495"},
  {"G1 X36.221 Y4.379"},
  {"G1 X36.144 Y4.263"},
  {"G1 X36.066 Y4.149"},
  {"G1 X35.986 Y4.037"},
  {"G1 X35.905 Y3.925"},
  {"G1 X35.822 Y3.815"},
  {"G1 X35.738 Y3.707"},
  {"G1 X35.652 Y3.599"},
  {"G1 X35.565 Y3.494"},
  {"G1 X35.476 Y3.389"},
  {"G1 X35.386 Y3.286"},
  {"G1 X35.294 Y3.185"},
  {"G1 X35.201 Y3.084"},
  {"G1 X35.083 Y2.962"},
  {"G1 X34.963 Y2.841"},
  {"G1 X34.840 Y2.723"},
  {"G1 X34.716 Y2.607"},
  {"G1 X34.615 Y2.516"},
  {"G1 X34.512 Y2.427"},
  {"G1 X34.408 Y2.339"},
  {"G1 X34.303 Y2.254"},
  {"G1 X34.197 Y2.169"},
  {"G1 X34.090 Y2.087"},
  {"G1 X33.981 Y2.006"},
  {"G1 X33.871 Y1.927"},
  {"G1 X33.760 Y1.849"},
  {"G1 X33.648 Y1.773"},
  {"G1 X33.535 Y1.699"},
  {"G1 X33.420 Y1.627"},
  {"G1 X33.304 Y1.557"},
  {"G1 X33.188 Y1.488"},
  {"G1 X33.070 Y1.422"},
  {"G1 X32.951 Y1.357"},
  {"G1 X32.831 Y1.294"},
  {"G1 X32.711 Y1.233"},
  {"G1 X32.589 Y1.174"},
  {"G1 X32.466 Y1.117"},
  {"G1 X32.342 Y1.061"},
  {"G1 X32.217 Y1.008"},
  {"G1 X32.091 Y0.957"},
  {"G1 X31.965 Y0.908"},
  {"G1 X31.837 Y0.860"},
  {"G1 X31.709 Y0.815"},
  {"G1 X31.580 Y0.772"},
  {"G1 X31.449 Y0.731"},
  {"G1 X31.318 Y0.692"},
  {"G1 X31.187 Y0.655"},
  {"G1 X31.054 Y0.620"},
  {"G1 X30.921 Y0.588"},
  {"G1 X30.820 Y0.565"},
  {"G1 X30.719 Y0.543"},
  {"G1 X30.618 Y0.522"},
  {"G1 X30.516 Y0.503"},
  {"G1 X30.414 Y0.485"},
  {"G1 X30.311 Y0.468"},
  {"G1 X30.208 Y0.452"},
  {"G1 X30.105 Y0.438"},
  {"G1 X30.001 Y0.425"},
  {"G1 X29.897 Y0.413"},
  {"G1 X29.792 Y0.403"},
  {"G1 X29.688 Y0.394"},
  {"G1 X29.582 Y0.386"},
  {"G1 X29.477 Y0.380"},
  {"G1 X29.371 Y0.374"},
  {"G1 X29.265 Y0.371"},
  {"G1 X29.158 Y0.368"},
  {"G1 X29.052 Y0.367"},
  {"G1 X28.902 Y0.368"},
  {"G1 X28.789 Y0.370"},
  {"G1 X28.676 Y0.374"},
  {"G1 X28.563 Y0.379"},
  {"G1 X28.451 Y0.386"},
  {"G1 X28.339 Y0.394"},
  {"G1 X28.228 Y0.404"},
  {"G1 X28.117 Y0.415"},
  {"G1 X28.006 Y0.428"},
  {"G1 X27.896 Y0.442"},
  {"G1 X27.786 Y0.458"},
  {"G1 X27.676 Y0.475"},
  {"G1 X27.567 Y0.494"},
  {"G1 X27.459 Y0.514"},
  {"G1 X27.351 Y0.535"},
  {"G1 X27.243 Y0.558"},
  {"G1 X27.136 Y0.582"},
  {"G1 X27.029 Y0.607"},
  {"G1 X26.923 Y0.634"},
  {"G1 X26.818 Y0.663"},
  {"G1 X26.713 Y0.692"},
  {"G1 X26.608 Y0.723"},
  {"G1 X26.504 Y0.756"},
  {"G1 X26.400 Y0.789"},
  {"G1 X26.297 Y0.824"},
  {"G1 X26.195 Y0.860"},
  {"G1 X26.093 Y0.898"},
  {"G1 X25.992 Y0.937"},
  {"G1 X25.891 Y0.977"},
  {"G1 X25.791 Y1.018"},
  {"G1 X25.642 Y1.083"},
  {"G1 X25.494 Y1.150"},
  {"G1 X25.348 Y1.220"},
  {"G1 X25.203 Y1.293"},
  {"G1 X25.060 Y1.369"},
  {"G1 X24.918 Y1.447"},
  {"G1 X24.778 Y1.528"},
  {"G1 X24.639 Y1.611"},
  {"G1 X24.501 Y1.697"},
  {"G1 X24.366 Y1.786"},
  {"G1 X24.232 Y1.877"},
  {"G1 X24.100 Y1.971"},
  {"G1 X23.969 Y2.067"},
  {"G1 X23.840 Y2.165"},
  {"G1 X23.713 Y2.266"},
  {"G1 X23.588 Y2.370"},
  {"G1 X23.464 Y2.475"},
  {"G1 X23.343 Y2.583"},
  {"G1 X23.223 Y2.694"},
  {"G1 X23.105 Y2.806"},
  {"G1 X22.989 Y2.921"},
  {"G1 X22.875 Y3.038"},
  {"G1 X22.763 Y3.157"},
  {"G1 X22.653 Y3.278"},
  {"G1 X22.545 Y3.401"},
  {"G1 X22.440 Y3.527"},
  {"G1 X22.336 Y3.654"},
  {"G1 X22.234 Y3.783"},
  {"G1 X22.135 Y3.915"},
  {"G1 X22.038 Y4.048"},
  {"G1 X21.943 Y4.183"},
  {"G1 X21.850 Y4.320"},
  {"G1 X21.760 Y4.459"},
  {"G1 X21.672 Y4.600"},
  {"G1 X21.586 Y4.742"},
  {"G1 X21.503 Y4.887"},
  {"G1 X21.422 Y5.033"},
  {"G1 X21.343 Y5.180"},
  {"G1 X21.267 Y5.329"},
  {"G1 X21.218 Y5.430"},
  {"G1 X21.170 Y5.531"},
  {"G1 X21.122 Y5.633"},
  {"G1 X21.077 Y5.735"},
  {"G1 X21.032 Y5.839"},
  {"G1 X20.988 Y5.943"},
  {"G1 X20.946 Y6.047"},
  {"G1 X20.904 Y6.152"},
  {"G1 X20.864 Y6.258"},
  {"G1 X20.825 Y6.365"},
  {"G1 X20.787 Y6.472"},
  {"G1 X20.751 Y6.580"},
  {"G1 X20.716 Y6.688"},
  {"G1 X20.682 Y6.797"},
  {"G1 X20.649 Y6.906"},
  {"G1 X20.617 Y7.017"},
  {"G1 X20.587 Y7.127"},
  {"G1 X20.557 Y7.238"},
  {"G1 X20.530 Y7.350"},
  {"G1 X20.503 Y7.463"},
  {"G1 X20.478 Y7.576"},
  {"G1 X20.454 Y7.689"},
  {"G1 X20.431 Y7.803"},
  {"G1 X20.410 Y7.917"},
  {"G1 X20.389 Y8.032"},
  {"G1 X20.371 Y8.148"},
  {"G1 X20.353 Y8.264"},
  {"G1 X20.337 Y8.380"},
  {"G1 X20.322 Y8.497"},
  {"G1 X20.309 Y8.614"},
  {"G1 X20.297 Y8.732"},
  {"G1 X20.286 Y8.850"},
  {"G1 X20.277 Y8.969"},
  {"G1 X20.269 Y9.088"},
  {"G1 X20.263 Y9.207"},
  {"G1 X20.258 Y9.327"},
  {"G1 X20.254 Y9.447"},
  {"G1 X20.252 Y9.568"},
  {"G1 X20.251 Y9.689"},

  {"G0 X4.556 Y29.807"},
  {"G1 X4.556 Y0.367"},
  {"G1 X0.250 Y4.074"},
  {"G0 X0.250 Y4.064"},
  {"M3 S0"},
  {"G1 S0"},
  {"M107"},
};

class laser_class
{
public:
  laser_class(void);
  uint16_t get_laser_power(void);
  void set_laser_power(uint16_t power);
  void delay_for_power_up(void);
  void delay_for_power_down(void);
  void set_laser_frequency(uint16_t laser_frequency);
  void set_laser_ocr(uint16_t power);
  void reset(void);
  void update_laser_power(void);
  void set_current_position_zero(void);
  void laser_walking_border(void);
  void show_laser_prepare_focus_page(void);
  void show_laser_prepare_engrave_first_page(void);
  bool is_laser_size_out_range(void);
  void show_laser_prepare_engrave_second_page(pfile_list_t temp);
  void show_laser_prepare_from_pause_page(pfile_list_t temp);
  void show_laser_pause_engrave_page(pfile_list_t temp);
  laser_border_xy_position_t laser_border_xy_position;
  float laser_focus;
  bool synchronize_status;
  uint16_t spindle_pwm;
private:
  uint16_t laser_power;
};

extern laser_class Laser;
#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768
#endif // _LASER_H_
