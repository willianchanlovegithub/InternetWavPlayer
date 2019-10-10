/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2019-08-28     WillianChan   first version
 */

#ifndef __IOTB_KEY_PROCESS__
#define __IOTB_KEY_PROCESS__
#include <rtthread.h>
#include "flexible_button.h"

#define IOTB_KEY_SCAN_CYCLE (20)
rt_err_t flex_button_main(void);
void iotb_key_thr_set_cycle(uint16_t time);
void iotb_btn_scan_enable(void);
void iotb_btn_scan_disable(void);

#endif
