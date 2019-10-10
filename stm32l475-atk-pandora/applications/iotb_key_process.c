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
#include <stdint.h>
#include <rtdevice.h>
#include <board.h>
#include "iotb_event.h"
#include "flexible_button.h"
#include "iotb_key_process.h"

#undef DBG_SECTION_NAME
#undef DBG_LEVEL
#undef DBG_COLOR
#undef DBG_ENABLE

#define DBG_ENABLE
#define DBG_SECTION_NAME               "IOTB_KEY"
#ifdef IOTB_KEY_DEBUG
#define DBG_LEVEL                      DBG_LOG
#else
#define DBG_LEVEL                      DBG_INFO /* DBG_ERROR */
#endif 
#define DBG_COLOR
#include <rtdbg.h>

#define PIN_WK_UP  GET_PIN(C, 13)
#define PIN_KEY0   GET_PIN(D, 10)
#define PIN_KEY1   GET_PIN(D, 9)
#define PIN_KEY2   GET_PIN(D, 8)

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_1,
    USER_BUTTON_2,
    USER_BUTTON_3,
    USER_BUTTON_MAX
} user_button_t;

static flex_button_t user_button[USER_BUTTON_MAX];
static iotb_event_msg_t key_msg = {IOTB_EVENT_SRC_NONE, IOTB_EVENT_TYPE_NONE};

static void btn_0_cb(flex_button_t *btn)
{
    LOG_D("btn_0_cb");

    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            LOG_D("button[KEY0] is pressed!");
            break;

        case FLEX_BTN_PRESS_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] click event!");
            break;

        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] double click event!");
            break;

        case FLEX_BTN_PRESS_SHORT_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] short press start event!");
            break;

        case FLEX_BTN_PRESS_SHORT_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] short press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] long press start event!");
            break;

        case FLEX_BTN_PRESS_LONG_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] long press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] long hold event!");
            break;
        
        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY0;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY0] long hold up event!");
            break;
    }
}

static void btn_1_cb(flex_button_t *btn)
{
    LOG_D("btn_1_cb");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            LOG_D("button[KEY1] is pressed!");
            break;

        case FLEX_BTN_PRESS_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] click event!");
            break;

        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] double click event!");
            break;

        case FLEX_BTN_PRESS_SHORT_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] short press start event!");
            break;

        case FLEX_BTN_PRESS_SHORT_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] short press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] long press start event!");
            break;

        case FLEX_BTN_PRESS_LONG_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] long press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] long hold event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY1;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY1] long hold up event!");
            break;
    }
}

static void btn_2_cb(flex_button_t *btn)
{
    LOG_D("btn_2_cb");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            LOG_D("button[KEY2] is pressed!");
            break;

        case FLEX_BTN_PRESS_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] click event!");
            break;

        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] double click event!");
            break;

        case FLEX_BTN_PRESS_SHORT_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] short press start event!");
            break;

        case FLEX_BTN_PRESS_SHORT_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] short press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] long press start event!");
            break;

        case FLEX_BTN_PRESS_LONG_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] long press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] long hold event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEY2;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[KEY2] long hold up event!");
            break;
    }
}

static void btn_wkup_cb(flex_button_t *btn)
{
    LOG_D("btn_wkup_cb");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            LOG_D("button[WK_UP]] is pressed!");
            break;

        case FLEX_BTN_PRESS_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] click event!");
            break;

        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] double click event!");
            break;
            
        case FLEX_BTN_PRESS_SHORT_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] short press start event!");
            break;

        case FLEX_BTN_PRESS_SHORT_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] short press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_START:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] long press start event!");
            break;

        case FLEX_BTN_PRESS_LONG_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] long press up event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] long hold event!");
            break;

        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            key_msg.event.event_src = IOTB_EVENT_SRC_KEYWKUP;
            key_msg.event.event_type = IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP;
            if (iotb_event_put(&key_msg) != RT_EOK)
                LOG_E("iotb event put failed!");
            LOG_I("button[WK_UP] long hold up event!");
            break;
    }
}

static uint8_t button_key0_read(void)
{
    return rt_pin_read(PIN_KEY0);
}

static uint8_t button_key1_read(void)
{
    return rt_pin_read(PIN_KEY1);
}

static uint8_t button_key2_read(void)
{
    return rt_pin_read(PIN_KEY2);
}

static uint8_t button_keywkup_read(void)
{
    return rt_pin_read(PIN_WK_UP);
}

static void button_scan(void *arg)
{
    while(1)
    {
        flex_button_scan();
        rt_thread_mdelay(10);
    }
}

static void user_button_init(void)
{
    int i;
    
    rt_memset(&user_button[0], 0x0, sizeof(user_button));

    user_button[USER_BUTTON_0].usr_button_read = button_key0_read;
    user_button[USER_BUTTON_0].cb = (flex_button_response_callback)btn_0_cb;

    user_button[USER_BUTTON_1].usr_button_read = button_key1_read;
    user_button[USER_BUTTON_1].cb = (flex_button_response_callback)btn_1_cb;

    user_button[USER_BUTTON_2].usr_button_read = button_key2_read;
    user_button[USER_BUTTON_2].cb = (flex_button_response_callback)btn_2_cb;
    
    user_button[USER_BUTTON_3].usr_button_read = button_keywkup_read;
    user_button[USER_BUTTON_3].cb = (flex_button_response_callback)btn_wkup_cb;

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT);  /* set KEY pin mode to input */
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT);  /* set KEY pin mode to input */
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT);  /* set KEY pin mode to input */
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT); /* set KEY pin mode to input */

    for (i = 0; i < USER_BUTTON_MAX; i ++)
    {
        user_button[i].pressed_logic_level = 0;
        user_button[i].click_start_tick = 20;
        user_button[i].short_press_start_tick = 100;
        user_button[i].long_press_start_tick = 200;
        user_button[i].long_hold_start_tick = 300;

        if (i == USER_BUTTON_3)
        {
            user_button[USER_BUTTON_3].pressed_logic_level = 1;
        }

        flex_button_register(&user_button[i]);
    }
}

rt_err_t flex_button_main(void)
{
    rt_thread_t tid = RT_NULL;

    user_button_init();

    /* Create background ticks thread */
    tid = rt_thread_create("flex_btn", button_scan, RT_NULL, 1024, 10, 10);
    if(tid != RT_NULL)
        rt_thread_startup(tid);

    return RT_EOK;
}
