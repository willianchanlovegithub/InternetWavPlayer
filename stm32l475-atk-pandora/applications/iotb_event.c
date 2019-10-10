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
#include "iotb_event.h"
#include "iotb_lcd_process.h"
#include <rthw.h>

#define IOTB_EVENT_MSG_NUM  10

// #define IOTB_EVENT_DEBUG

#define DBG_ENABLE
#define DBG_TAG             "IOTB_EVENT"
#ifdef IOTB_EVENT_DEBUG
#define DBG_LVL             DBG_LOG
#else
#define DBG_LVL             DBG_INFO /* DBG_ERROR */
#endif 
#define DBG_COLOR
#include <rtdbg.h>

static struct rt_messagequeue iotb_event_mq;
iotb_event_msg_t event_msg[IOTB_EVENT_MSG_NUM];
static uint16_t iotb_event_thr_cycle = IOTB_EVENT_THR_CYCLE;

static rt_err_t iotb_event_init(void)
{
    rt_mq_init(&iotb_event_mq,
               "event_mq",
               &event_msg[0],
               sizeof(iotb_event_msg_t),
               sizeof(event_msg),
               RT_IPC_FLAG_FIFO);

    return RT_EOK;
}

static rt_bool_t iotb_event_put_enable = RT_TRUE;

void iotb_event_put_set_enable(void)
{
    rt_base_t level = rt_hw_interrupt_disable();
    iotb_event_put_enable = RT_TRUE;
    rt_hw_interrupt_enable(level);
}

void iotb_event_put_set_disable(void)
{
    rt_base_t level = rt_hw_interrupt_disable();
    iotb_event_put_enable = RT_FALSE;
    rt_hw_interrupt_enable(level);
}

rt_err_t iotb_event_put(iotb_event_msg_t *msg)
{
    rt_err_t rst = RT_EOK;
    if (iotb_event_put_enable == RT_FALSE)
    {
        LOG_I("event put is disable!");
        return rst;
    }

    rst = rt_mq_send(&iotb_event_mq, msg, sizeof(iotb_event_msg_t));
    if (rst == -RT_EFULL)
    {
        LOG_I("iotb event queue is full");
        rt_thread_mdelay(100);
        rst = -RT_EFULL;
    }
    return rst;
}

rt_err_t iotb_event_get(iotb_event_msg_t *msg, uint32_t timeout)
{
    rt_err_t rst = RT_EOK;

    rst = rt_mq_recv(&iotb_event_mq, msg, sizeof(iotb_event_msg_t), timeout);
    if ((rst != RT_EOK) && (rst != -RT_ETIMEOUT))
    {
        LOG_E("iotb event get failed! Errno: %d", rst);
    }
    return rst;
}
#include <msh.h>
#include <finsh.h>
static rt_err_t iotb_event_process(iotb_event_msg_t *msg)
{
    rt_err_t rst = RT_EOK;

    switch (msg->event.event_src)
    {
        case IOTB_EVENT_SRC_KEY2:
            LOG_I("response KEY2 event!");
            switch (msg->event.event_type)
            {
                case IOTB_EVENT_TYPE_KEY_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY2_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY2_DOUBLE_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY2_SHORT_PRESSED);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY2_LONG_HOLD);
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP:
                    break;
                default:
                    LOG_E("unhandled event!");
                    break;
            }
            break;

        case IOTB_EVENT_SRC_KEY1:
            LOG_I("response KEY1 event!");
            switch (msg->event.event_type)
            {
                case IOTB_EVENT_TYPE_KEY_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY1_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY1_SHORT_PRESSED);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY1_LONG_HOLD);
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP:
                    break;
                default:
                    LOG_E("unhandled event!");
                    break;
            }
            break;

        case IOTB_EVENT_SRC_KEY0:
            LOG_I("response KEY0 event!");
            switch (msg->event.event_type)
            {
                case IOTB_EVENT_TYPE_KEY_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_MUSIC_PLAY);
                    break;
                case IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_NEXT);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY0_SHORT_PRESSED);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_KEY0_LONG_HOLD);
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP:
                    break;
                default:
                    LOG_E("unhandled event!");
                    break;
            }
            break;
        
        case IOTB_EVENT_SRC_KEYWKUP:
            LOG_I("response KEYWKUP event!");
            switch (msg->event.event_type)
            {
                case IOTB_EVENT_TYPE_KEY_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_WKUP_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_DOUBLE_CLICK:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_WKUP_SHORT_PRESSED);
                    break;
                case IOTB_EVENT_TYPE_KEY_SHORT_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_PRESSED_UP:
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD:
                    iotb_lcd_event_put(IOTB_LCD_EVENT_WKUP_LONG_HOLD);
                    break;
                case IOTB_EVENT_TYPE_KEY_LONG_HOLD_UP:
                    break;
                default:
                    LOG_E("unhandled event!");
                    break;
            }
            break;

        default:
            LOG_E("unhandled event!");
            break;
    }

    return rst;
}

void iotb_event_thr_set_cycle(uint16_t time)
{
    rt_base_t level = rt_hw_interrupt_disable();
    iotb_event_thr_cycle = time;
    rt_hw_interrupt_enable(level);
}

static void iotb_event_handler(void* arg)
{
    rt_err_t rst = RT_EOK;
    iotb_event_msg_t msg;

    while(1)
    {
        rst = iotb_event_get(&msg, iotb_event_thr_cycle);
        if (rst == RT_EOK)
        {
            LOG_D("event src: %s; event type: %s",
                  iotb_event_src_string[msg.event.event_src],
                  iotb_event_type_string[msg.event.event_type]);
            iotb_event_process(&msg);
        }
    }
}

rt_err_t iotb_event_start(void)
{
    rt_thread_t iotb_event_tid;

    iotb_event_init();

    iotb_event_tid = rt_thread_create("event_thr", iotb_event_handler, RT_NULL,
                                      1024, RT_THREAD_PRIORITY_MAX/2-4, 20);
    if (iotb_event_tid != RT_NULL)
        rt_thread_startup(iotb_event_tid);
    
    return RT_EOK;
}
