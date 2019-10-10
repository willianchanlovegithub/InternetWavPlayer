/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2019-08-28     WillianChan   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_lcd.h>
#include <rttlogo.h>

static int lcd_index(void)
{
    char buf[25];
    
    lcd_clear(WHITE);
    lcd_show_image(0, 24, 240, 69, image_rttlogo);
    lcd_set_color(WHITE, BLACK);
    lcd_show_string(8, 24 + 69 + 12, 32, "Internet Radio");
    lcd_set_color(WHITE, RED);
    lcd_draw_line(8, 24 + 69 + 12 + 32 + 24, 240 - 8, 24 + 69 + 12 + 32 + 24);
    lcd_set_color(WHITE, GREEN);
    lcd_draw_line(8 + 16, 24 + 69 + 12 + 32 + 24 + 4, 240 - 8 - 16, 24 + 69 + 12 + 32 + 24 + 4);
    lcd_set_color(WHITE, BLUE);
    lcd_draw_line(8 + 16 + 16, 24 + 69 + 12 + 32 + 24 + 4 * 2, 240 - 8 - 16 - 16, 24 + 69 + 12 + 32 + 24 + 4 * 2);
    lcd_set_color(WHITE, MAGENTA);
    lcd_draw_line(8 + 16 + 16 + 16, 24 + 69 + 12 + 32 + 24 + 4 * 3, 240 - 8 - 16 - 16 - 16, 24 + 69 + 12 + 32 + 24 + 4 * 3);
    rt_memset(buf, 0x0, sizeof(buf));
    rt_snprintf(buf, sizeof(buf), "RT-Thread Version: %d.%d.%d", RT_VERSION, RT_SUBVERSION, RT_REVISION);
    lcd_set_color(WHITE, BLACK);
    lcd_show_string(24, 24 + 69 + 12 + 32 + 24 + 4 * 5, 16, buf);
    
    return RT_EOK;
}
INIT_APP_EXPORT(lcd_index);
