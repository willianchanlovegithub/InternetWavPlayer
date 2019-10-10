/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2019-08-28     WillianChan   first version
 */

#ifndef __IOTB_LCD_PROCESS__
#define __IOTB_LCD_PROCESS__
#include <rtthread.h>
#include <stdint.h>

#define IOTB_LCD_MENU_MAX   (3)     /* 目录个数 */
#define IOTB_LCD_THR_CYCLE  (50)

#define MUSIC_PATH_MAX_LEN  (128u)  /* 音乐路径最大长度 */
#define MUSIC_PATH          "/"     /* 音乐打开目录 */

#define WEBSERVER_URL       "http://wuhanstudio.online:1234/dir"    /* 服务器JSON */
#define DOWNLOAD_URL        "http://123.207.116.104/ctw/"           /* 服务器IP地址 */
//#define DOWNLOAD_URL        "http://wuhanstudio.online:9003/"       /* 服务器域名地址 */
#define GET_HEADER_BUFSZ    1024                                    /* 头部大小 */
#define GET_RESP_BUFSZ      1024                                    /* 响应缓冲区大小 */
#define GET_URL_LEN_MAX     128                                     /* 网址最大长度 */

typedef void (*iotb_lcd_handle)(void*);

typedef enum
{
    IOTB_LCD_CTL_OPEN = 0,
    IOTB_LCD_CTL_SHUTDOWN,
    IOTB_LCD_CTL_POWERSAVE_ENTER,
    IOTB_LCD_CTL_POWERSAVE_EXIT
} iotb_lcd_ctl_t;

typedef enum
{
    IOTB_LCD_CONTENT_NONE = 0,
    IOTB_LCD_STATIC_CONTENT,
    IOTB_LCD_DYNAMIC_CONTENT
} iotb_lcd_content_type_t;

typedef enum
{
    IOTB_LCD_EVENT_KEY2_CLICK            = (1u << 0u),
    IOTB_LCD_EVENT_KEY1_CLICK            = (1u << 1u),
    IOTB_LCD_EVENT_KEY0_CLICK            = (1u << 2u),
    IOTB_LCD_EVENT_WKUP_CLICK            = (1u << 3u),
    IOTB_LCD_EVENT_KEY2_DOUBLE_CLICK     = (1u << 4u),
    IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK     = (1u << 5u),
    IOTB_LCD_EVENT_KEY0_DOUBLE_CLICK     = (1u << 6u),
    IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK     = (1u << 7u),
    IOTB_LCD_EVENT_KEY2_SHORT_PRESSED    = (1u << 8u),
    IOTB_LCD_EVENT_KEY1_SHORT_PRESSED    = (1u << 9u),
    IOTB_LCD_EVENT_KEY0_SHORT_PRESSED    = (1u << 10u),
    IOTB_LCD_EVENT_WKUP_SHORT_PRESSED    = (1u << 11u),
    IOTB_LCD_EVENT_KEY2_LONG_HOLD        = (1u << 12u),
    IOTB_LCD_EVENT_KEY1_LONG_HOLD        = (1u << 13u),
    IOTB_LCD_EVENT_KEY0_LONG_HOLD        = (1u << 14u),
    IOTB_LCD_EVENT_WKUP_LONG_HOLD        = (1u << 15u),
    IOTB_LCD_EVENT_NONE                  = (1u << 16u),
    IOTB_LCD_EVENT_ENTER                 = (1u << 17u),
    IOTB_LCD_EVENT_EXIT                  = (1u << 18u),
    IOTB_LCD_EVENT_NEXT                  = (1u << 19u),
    IOTB_LCD_EVENT_NEXTSONG              = (1u << 20u),
    IOTB_LCD_EVENT_LASTSONG              = (1u << 21u),
    IOTB_LCD_EVENT_DOWNLOAD              = (1u << 22u),
    IOTB_LCD_EVENT_MUSIC_PLAY            = (1u << 23u),
    IOTB_LCD_EVENT_MAX                   = (1u << 24u)
} iotb_lcd_event_t;


typedef struct
{
    uint16_t menu;
    uint16_t refresh_time; /* unit: os tick cnt */
    iotb_lcd_content_type_t content_type;
    iotb_lcd_event_t current_event;
    iotb_lcd_handle lcd_handle;
} iotb_lcd_menu_t;

rt_err_t iotb_lcd_start(void);
void iotb_lcd_event_put(iotb_lcd_event_t event);
rt_err_t iotb_lcd_event_get(uint32_t set, uint32_t *event, uint8_t clr, uint32_t timeout);
void iotb_lcd_update_menu_index(uint8_t menu_index);
uint8_t iotb_lcd_get_menu_index(void);
#endif
