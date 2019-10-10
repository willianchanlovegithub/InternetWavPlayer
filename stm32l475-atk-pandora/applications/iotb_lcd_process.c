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
#include <stdio.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_lcd.h>
#include <wavplayer.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <rthw.h>
#include <webclient.h>
#include "cJSON.h"
#include "iotb_lcd_process.h"
#include "iotb_event.h"

#undef DBG_SECTION_NAME
#undef DBG_LEVEL
#undef DBG_COLOR
#undef DBG_ENABLE

// #define IOTB_LCD_DEBUG

#define DBG_ENABLE
#define DBG_SECTION_NAME               "IOTB_LCD"
#ifdef IOTB_LCD_DEBUG
#define DBG_LEVEL                      DBG_LOG
#else
#define DBG_LEVEL                      DBG_INFO /* DBG_ERROR */
#endif
#define DBG_COLOR
#include <rtdbg.h>

#define IOTB_LCD_THREAD_STACK_SIZE    2048
#define IOTB_ENTER_PM_WAIT_TICK       20

static rt_mutex_t lcd_mutex = RT_NULL;
static struct rt_event lcd_event;

static volatile uint8_t iotb_lcd_menu_index = 1;
static volatile uint8_t iotb_lcd_event_enter = 0;
static volatile uint8_t iotb_lcd_event_exit = 0;
static volatile uint8_t iotb_lcd_status_busy = 0;

static void iotb_lcd_show_httpwebserver(iotb_lcd_menu_t *lcd_menu); /* menu1  */
static void iotb_lcd_show_music(iotb_lcd_menu_t *lcd_menu);         /* menu2  */
static void iotb_lcd_show_sdcard(iotb_lcd_menu_t *lcd_menu);        /* menu3 */

static const uint16_t menu_refresh_time[IOTB_LCD_MENU_MAX + 1] =
{
    5,  /* menu1  show httpwebserver file */
    5,  /* menu2  show music, play wave file */
    5,  /* menu3  show SD card content */
};

static volatile iotb_lcd_menu_t lcd_instance[IOTB_LCD_MENU_MAX + 1];
static const iotb_lcd_handle iotb_lcd_handle_array[IOTB_LCD_MENU_MAX + 1] = 
{
    RT_NULL,
    (iotb_lcd_handle)iotb_lcd_show_httpwebserver,
    (iotb_lcd_handle)iotb_lcd_show_music,
    (iotb_lcd_handle)iotb_lcd_show_sdcard
};

/* Function declaration */
static void iotb_lcd_show(void *arg);

void iotb_lcd_event_put(iotb_lcd_event_t event)
{
    rt_event_send(&lcd_event, (rt_uint32_t)event);
}

/** iotb_lcd_event_get
 * @param event_t: lcd event type
 * @param   event: the event value getting from system
 * @param     clr: 0 - do not clear event flag; 1 - clear event flag
 * @param timeout: set operate time
 * @return RT_EOK - success; others - failed
*/
rt_err_t iotb_lcd_event_get(uint32_t set, uint32_t *event, uint8_t clr, uint32_t timeout)
{
    rt_err_t rst = RT_EOK;
    
    if (clr)
        rst = rt_event_recv(&lcd_event, set, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                            timeout, (rt_uint32_t *)event);
    else
        rst = rt_event_recv(&lcd_event, set, RT_EVENT_FLAG_OR,
                            timeout, (rt_uint32_t *)event);

    if (rst == RT_EOK)
        LOG_D("recv event 0x%x", *event);
    
    return rst;
}

static uint16_t iotb_lcd_thr_cycle = IOTB_LCD_THR_CYCLE;
void iotb_lcd_thr_set_cycle(uint16_t time)
{
    rt_base_t level = rt_hw_interrupt_disable();
    iotb_lcd_thr_cycle = time;
    rt_hw_interrupt_enable(level);
}

static void iotb_lcd_show(void *arg)
{
    uint8_t menu_index = 1;
    uint16_t cnt = 0;
    uint32_t event = 0;
    
    lcd_instance[menu_index].content_type = IOTB_LCD_STATIC_CONTENT;
    lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_NONE;
    lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);

    while (1)
    {
        if (iotb_lcd_event_get((~IOTB_LCD_EVENT_NONE), &event, 1, 0) == RT_EOK)
        {
            if (event & IOTB_LCD_EVENT_NEXT)
            {
                menu_index = iotb_lcd_get_menu_index();
                lcd_instance[menu_index].content_type = IOTB_LCD_CONTENT_NONE;
                lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_EXIT;
                /* �˳���ǰĿ¼��������һ��Ŀ¼ */
                lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                
                menu_index++;
                /* �������ֲ���ҳ�� */
                if (menu_index == 2)
                {
                    menu_index++;
                }
                menu_index = menu_index > IOTB_LCD_MENU_MAX ? 1 : menu_index;
                iotb_lcd_update_menu_index(menu_index);

                lcd_instance[menu_index].content_type = IOTB_LCD_STATIC_CONTENT;
                lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_ENTER;
                lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);

                cnt = lcd_instance[menu_index].refresh_time;
            }
            else if (event & IOTB_LCD_EVENT_MUSIC_PLAY)
            {
                /* ����menu2 */
                if (menu_index == 1)
                {
                    menu_index = iotb_lcd_get_menu_index();
                    lcd_instance[menu_index].content_type = IOTB_LCD_CONTENT_NONE;
                    lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_EXIT;
                    /* �˳���ǰĿ¼��������һ��Ŀ¼ */
                    lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                    
                    menu_index++;
                    iotb_lcd_update_menu_index(menu_index);
                    
                    lcd_instance[menu_index].content_type = IOTB_LCD_STATIC_CONTENT;
                    lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_ENTER;
                    lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                    
                    cnt = lcd_instance[menu_index].refresh_time;
                }
            }
            else if (event & IOTB_LCD_EVENT_EXIT)
            {
                lcd_instance[menu_index].content_type = IOTB_LCD_CONTENT_NONE;
                lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_EXIT;
                lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                /* set current menu event as NONE, and set show NONE */
                lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_NONE;
                cnt = 0;
            }
            /* KEY2,KEY1,WKUP�ĵ����¼����� */
            else if ((event & IOTB_LCD_EVENT_KEY2_CLICK) ||
                    (event & IOTB_LCD_EVENT_KEY1_CLICK) ||
                    (event & IOTB_LCD_EVENT_WKUP_CLICK))
            {
                if (menu_index == 1)
                {
                    lcd_instance[menu_index].content_type = IOTB_LCD_DYNAMIC_CONTENT;
                    if (event & IOTB_LCD_EVENT_KEY2_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_DOWNLOAD;
                    else if (event & IOTB_LCD_EVENT_KEY1_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_NEXTSONG;
                    else if (event & IOTB_LCD_EVENT_WKUP_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_LASTSONG;
                }
                if (menu_index == 2)
                {
                    lcd_instance[menu_index].content_type = IOTB_LCD_DYNAMIC_CONTENT;
                    /* ����menu1 */
                    if (event & IOTB_LCD_EVENT_KEY2_CLICK)
                    {
                        menu_index = iotb_lcd_get_menu_index();
                        lcd_instance[menu_index].content_type = IOTB_LCD_CONTENT_NONE;
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_EXIT;
                        /* �˳���ǰĿ¼��������һ��Ŀ¼ */
                        lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                        
                        menu_index--;
                        iotb_lcd_update_menu_index(menu_index);
                        
                        lcd_instance[menu_index].content_type = IOTB_LCD_STATIC_CONTENT;
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_ENTER;
                        lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
                        
                        cnt = lcd_instance[menu_index].refresh_time;
                    }
                    else if (event & IOTB_LCD_EVENT_KEY1_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_KEY1_CLICK;
                    else if (event & IOTB_LCD_EVENT_WKUP_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_WKUP_CLICK;
                }
            }
            /* KEY2,KEY1,WKUP��˫���¼����� */
            else if ((event & IOTB_LCD_EVENT_KEY2_DOUBLE_CLICK) ||
                    (event & IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK) ||
                    (event & IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK))
            {
                lcd_instance[menu_index].content_type = IOTB_LCD_DYNAMIC_CONTENT;
                if (event & IOTB_LCD_EVENT_KEY2_DOUBLE_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_KEY2_DOUBLE_CLICK;
                    else if (event & IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK;
                    else if (event & IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK)
                        lcd_instance[menu_index].current_event = IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK;
            }
        }

        /* TODO Need to wait until the start event runs, First check if is sho NONE, if yes skip */
        if ((lcd_instance[menu_index].content_type == IOTB_LCD_DYNAMIC_CONTENT) &&
            (cnt >= lcd_instance[menu_index].refresh_time))
        {
            cnt = 0;
            lcd_instance[menu_index].lcd_handle((void*)&lcd_instance[menu_index]);
        }
        rt_thread_mdelay(iotb_lcd_thr_cycle);
        if (lcd_instance[menu_index].content_type == IOTB_LCD_DYNAMIC_CONTENT)
        {
            cnt ++;
        }
    }
}

static char **songlist;         /* �������赥 */
static int item_num;            /* JSON����Ԫ�ظ��� */
static int songlist_num = 0;    /* ��ǰ�赥��� */

/* webserver���ݽ��� */
void webserver_data_parse(rt_uint8_t *data)
{
    cJSON *root = RT_NULL;
    cJSON *item_name = RT_NULL;
    cJSON *array = RT_NULL;
    
    root = cJSON_Parse((const char *)data);
    if (!root)
    {
        rt_kprintf("No memory for cJSON root!\n");
        return;
    }
    
    array = cJSON_GetObjectItem(root, "internet_radio");
    if (array != RT_NULL)
    {
        /* ͳ��JSON����Ԫ�ظ������磺{"internet_radio":["1.wav","2.wav","3.wav"]}����3��Ԫ�� */
        item_num = cJSON_GetArraySize(array);
        if (item_num > 6)
            item_num = 6;
        if (item_num > 0)
        {
            /* �����Seaching���ַ��� */
            lcd_show_string(48, 80, 24, "            ");
            rt_kprintf("item_num = %d\n", item_num);
            /* ���赥����ռ� */
            songlist = (char **)rt_malloc((sizeof(char *)) * item_num);
            for (int cnt = 0; cnt < item_num; cnt++)
            {
                /* ��ȡ��������ʾ��LCD�� */
                item_name = cJSON_GetArrayItem(array, cnt);
                lcd_set_color(WHITE, BLACK);
                lcd_show_string(8, 52 + 31 * cnt, 24, item_name->valuestring);
                lcd_set_color(WHITE, GRAY240);
                lcd_draw_line(8, 52 + 31 * cnt - 4, 240 - 8, 52 + 31 * cnt - 4);
                /* ��������д������󳤶�30���ַ� */
                songlist[cnt] = (char *)rt_malloc((sizeof(char)) * 30);
                /* ����赥 */
                rt_memcpy(songlist[cnt], item_name->valuestring, 30);
                rt_kprintf("Song%d:%s\n", cnt, songlist[cnt]);
            }
            /* ѡ�е�ǰ���� */
            lcd_draw_line(8, 52 + 31 * item_num - 4, 240 - 8, 52 + 31 * item_num - 4);
            lcd_set_color(BLUE, WHITE);
            lcd_show_string(0, 52 + 31 * songlist_num, 24, "                    ");
            lcd_show_string(8, 52 + 31 * songlist_num, 24, songlist[songlist_num]);
        }
        else
        {
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(48, 80, 24, "            ");
            lcd_show_string(78, 80, 24, "No Data");
        }
    }
    
    if (root != RT_NULL)
    {
        cJSON_Delete(root);
    }
}

/* menu1 */
void iotb_lcd_show_httpwebserver(iotb_lcd_menu_t *lcd_menu)
{
    rt_uint8_t *buffer = RT_NULL;
    char *webserver_url = RT_NULL;
    struct webclient_session *session = RT_NULL;
    int resp_status;
    int content_length = -1;
    int bytes_read = 0;
    int content_pos = 0;
    
    /* ����ҳ����ʾ */
    if (lcd_menu->content_type == IOTB_LCD_STATIC_CONTENT)
    {
        LOG_I("show [webserver page]");
        lcd_clear(WHITE);
        lcd_set_color(WHITE, BLACK);
        lcd_show_string(40, 8, 32, "Web Server");
        lcd_show_string(48, 80, 24, "Searching...");
    }
    
    /* ���ӷ�������ȡ���� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_ENTER)
    {
        /* Ϊwebserver_url����ռ� */
        webserver_url = rt_calloc(1, GET_URL_LEN_MAX);
        if (webserver_url == RT_NULL)
        {
            rt_kprintf("No memory for weather_url!\n");
            goto __exit;
        }
        rt_snprintf(webserver_url, GET_URL_LEN_MAX, WEBSERVER_URL);
        /* �����Ự����������Ӧ�Ĵ�С */
        session = webclient_session_create(GET_HEADER_BUFSZ);
        if (session == RT_NULL)
        {
            rt_kprintf("No memory for get header!\n");
            goto __exit;
        }
        /* ����GET����ʹ��Ĭ�ϵ�ͷ�� */
        if ((resp_status = webclient_get(session, webserver_url)) != 200)
        {
            rt_kprintf("webclient GET request failed, response(%d) error.\n", resp_status);
            goto __exit;
        }
        /* �������ڴ�Ž������ݵĻ��� */
        buffer = rt_calloc(1, GET_RESP_BUFSZ);
        if(buffer == RT_NULL)
        {
            rt_kprintf("No memory for data receive buffer!\n");
            goto __exit;
        }
        content_length = webclient_content_length_get(session);
        if (content_length < 0)
        {
            /* ���ص������Ƿֿ鴫��� */
            do
            {
                bytes_read = webclient_read(session, buffer, GET_RESP_BUFSZ);
                if (bytes_read <= 0)
                {
                    break;
                }
            } while (1);
        }
        else
        {
            do
            {
                bytes_read = webclient_read(session, buffer, 
                                            content_length - content_pos > GET_RESP_BUFSZ ?
                                            GET_RESP_BUFSZ : content_length - content_pos);
                if (bytes_read <= 0)
                {
                    break;
                }
                content_pos += bytes_read;
            } while (content_pos < content_length);
        }
        
        /* ��ӡԭʼJSON���� */
        rt_kprintf("JSON_data = %s\n", buffer);
        
        /* webserver��JSONԭʼ���ݽ��� */
        webserver_data_parse(buffer);
    }
    
    /* ѡ�裺��һ�׸� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_NEXTSONG)
    {
        if (songlist_num <= (item_num - 1))
        {
            songlist_num++;
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(0, 52 + 31 * (songlist_num - 1), 24, "                    ");
            lcd_show_string(8, 52 + 31 * (songlist_num - 1), 24, songlist[songlist_num - 1]);
            lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
        }
        if (songlist_num > (item_num - 1))
        {
            songlist_num = 0;
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(0, 52 + 31 * (item_num - 1), 24, "                    ");
            lcd_show_string(8, 52 + 31 * (item_num - 1), 24, songlist[item_num - 1]);
        }
        rt_kprintf("this song = %s\n", songlist[songlist_num]);
        lcd_set_color(BLUE, WHITE);
        lcd_show_string(0, 52 + 31 * songlist_num, 24, "                    ");
        lcd_show_string(8, 52 + 31 * songlist_num, 24, songlist[songlist_num]);
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* ѡ�裺��һ�׸� */
    if (lcd_menu->current_event  == IOTB_LCD_EVENT_LASTSONG)
    {
        if (songlist_num >= 0)
        {
            songlist_num--;
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(0, 52 + 31 * (songlist_num + 1), 24, "                    ");
            lcd_show_string(8, 52 + 31 * (songlist_num + 1), 24, songlist[songlist_num + 1]);
        }
        if (songlist_num < 0)
        {
            songlist_num = item_num - 1;
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(0, 52 + 31 * songlist_num, 24, "                    ");
            lcd_show_string(8, 52 + 31 * songlist_num, 24, songlist[songlist_num]);
        }
        rt_kprintf("this song = %s\n", songlist[songlist_num]);
        
        lcd_set_color(BLUE, WHITE);
        lcd_show_string(0, 52 + 31 * songlist_num, 24, "                    ");
        lcd_show_string(8, 52 + 31 * songlist_num, 24, songlist[songlist_num]);
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* �л�ҳ��ֹͣ��ǰ���� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_EXIT)
    {
        wavplayer_stop();
    }
    
__exit:
    /* �ͷ���ַ�ռ� */
    if (webserver_url != RT_NULL)
    {
        rt_free(webserver_url);
    }
    /* �رջỰ */
    if (session != RT_NULL)
    {
        webclient_close(session);
    }
    /* �ͷŻ������ռ� */
    if (buffer != RT_NULL)
    {
        rt_free(buffer);
    }
}

static char iotb_music_is_play = 0;     /* ֹͣ/��ʼ���ű�־λ */
static char iotb_music_is_resume = 0;   /* ��ͣ/�ָ����ű�־λ */
static int volume_value = 55;           /* Ĭ������ */

extern const unsigned char gImage_fangjian[];
extern const unsigned char gImage_gaobaiqiqiu[];
extern const unsigned char gImage_jiandanai[];
extern const unsigned char gImage_mingmingjiu[];
extern const unsigned char gImage_liuruiqi[];
extern const unsigned char gImage_erke[];

/* menu2 */
static void iotb_lcd_show_music(iotb_lcd_menu_t *lcd_menu)
{
    char *download_url = RT_NULL;
    
    if (lcd_menu->content_type == IOTB_LCD_STATIC_CONTENT)
    {
        LOG_I("show [music page]");
        lcd_clear(WHITE);
        lcd_set_color(WHITE, BLACK);
        lcd_show_string(40, 8, 32, "Wav Player");
        lcd_set_color(WHITE, BLACK);
        lcd_show_num(224, 224, volume_value, RT_NULL, 16);
        /* ��ʾ��ǰ������ */
        lcd_show_string(0, 224, 16, "Volume:");
        lcd_set_color(WHITE, RED);
        for (int i = 0; i < (100 / 5 - 1); i++)
        {
            if (i == (volume_value / 5))
            {
                lcd_set_color(WHITE, GRAY240);
            }
            lcd_show_string(64 + 8 * i, 224, 16, "=");
        }
    }
    
    if (lcd_menu->current_event == IOTB_LCD_EVENT_ENTER)
    {
        /* ר��ͼƬ��ʾ */
        if (strcmp(songlist[songlist_num], "fangjian.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_fangjian);
        }
        else if (strcmp(songlist[songlist_num], "gaobaiqiqiu.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_gaobaiqiqiu);
        }
        else if (strcmp(songlist[songlist_num], "jiandanai.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_jiandanai);
        }
        else if (strcmp(songlist[songlist_num], "mingmingjiu.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_mingmingjiu);
        }
        else if (strcmp(songlist[songlist_num], "xiatiandefeng.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_liuruiqi);
        }
        else if (strcmp(songlist[songlist_num], "yuebanxiaoyequ.wav") == 0)
        {
            lcd_show_image(45, 48, 150, 150, gImage_erke);
        }
        else
        {
            lcd_draw_rectangle(45, 48, 195, 198);
            lcd_draw_line(45, 48, 195, 198);
            lcd_draw_line(195, 48, 45, 198);
            lcd_show_string(72, 111, 24, "No Album");
        }
        /* ������������Ѵ��ڱ��أ�ֱ�Ӳ��� */
        if (open(songlist[songlist_num], O_RDONLY) >= 0)
        {
            wavplayer_play(songlist[songlist_num]);
            iotb_music_is_play = 1;
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(((240 - strlen(songlist[songlist_num]) * 12) / 2), 199, 24, songlist[songlist_num]);
        }
        /* ����ͨ��webclient���浽�����ٲ��� */
        else
        {
            lcd_set_color(WHITE, BLACK);
            lcd_show_string(60, 199, 24, "Caching...");
            /* ƴ����ַ */
            download_url = (char *) malloc(strlen(DOWNLOAD_URL) + strlen(songlist[songlist_num]));
            rt_kprintf("strlen(songlist[songlist_num]) = %d\n", strlen(songlist[songlist_num]));
            rt_kprintf("strlen(DOWNLOAD_URL) = %d\n", strlen(DOWNLOAD_URL));
            sprintf(download_url, "%s%s", DOWNLOAD_URL, songlist[songlist_num]);
            rt_kprintf("download_url = %s\n", download_url);
            /* ͨ��webclient�����ļ������� */
            if (webclient_get_file(download_url, songlist[songlist_num]) == RT_EOK)
            {
                wavplayer_play(songlist[songlist_num]);
                iotb_music_is_play = 1;
                lcd_set_color(WHITE, BLACK);
                lcd_show_string(0, 199, 24, "                    ");
                lcd_show_string(((240 - strlen(songlist[songlist_num]) * 12) / 2), 199, 24, songlist[songlist_num]);
            }
        }
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* ֹͣ/��ʼ���� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_WKUP_DOUBLE_CLICK)
    {
        if (iotb_music_is_play)
        {
            wavplayer_stop();
            iotb_music_is_play = 0;
        }
        else
        {
            wavplayer_play(songlist[songlist_num]);
            iotb_music_is_play = 1;
        }
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* ��ͣ/�ָ����� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_KEY1_DOUBLE_CLICK)
    {
        if (iotb_music_is_resume)
        {
            wavplayer_pause();
            iotb_music_is_resume = 0;
            LOG_I("wavplayer_pause");
            lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
        }
        else
        {
            wavplayer_resume();
            iotb_music_is_resume = 1;
            LOG_I("wavplayer_resume");
            lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
        }
    }
    /* �������� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_KEY1_CLICK)
    {
        if (volume_value > 10 )
        {
            volume_value = volume_value - 5;
            wavplayer_volume_set(volume_value);
            /* ������- */
            lcd_set_color(WHITE, GRAY240);
            lcd_show_string(224 - 8 * ((100 / 5) - (volume_value / 5)), 224, 16, "=");
            lcd_set_color(WHITE, BLACK);
            lcd_show_num(224, 224, volume_value, RT_NULL, 16);
            rt_kprintf("volume value is:%d\n", volume_value);
        }
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* �������� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_WKUP_CLICK)
    {
        if(volume_value < 95)
        {
            volume_value = volume_value + 5;
            wavplayer_volume_set(volume_value);
            /* ������+ */
            lcd_set_color(WHITE, RED);
            lcd_show_string(64 + 8 * (volume_value / 5 - 1), 224, 16, "=");
            lcd_set_color(WHITE, BLACK);
            lcd_show_num(224, 224, volume_value, RT_NULL, 16);
            rt_kprintf("volume value is:%d\n", volume_value);
        }
        lcd_menu->current_event = IOTB_LCD_EVENT_NONE;
    }
    /* �л�ҳ��ֹͣ���� */
    if (lcd_menu->current_event == IOTB_LCD_EVENT_EXIT)
    {
        iotb_music_is_play = 0;
        wavplayer_stop();
    }
}

/* menu3 */
static void iotb_lcd_show_sdcard(iotb_lcd_menu_t *lcd_menu)
{
    struct dirent *d;
    DIR *dirp = RT_NULL;
    uint8_t line = 0;
    char buf[16];

    if (lcd_menu->content_type == IOTB_LCD_STATIC_CONTENT)
    {
        LOG_I("show [sdcard page]");
        lcd_clear(WHITE);
        lcd_set_color(WHITE, BLACK);
        lcd_show_string(32, 8, 32, "File System");
    }

    dirp = opendir("/");
    if (dirp == RT_NULL)
    {
        lcd_show_string(24, 52 + 27 * 2, 24, " Insert SD card");
        lcd_show_string(24, 52 + 27 * 3, 24, " before power-on");
        LOG_E("open directory error!");
        return;
    }
    else
    {
        /* readdirÿ��ȡһ��Ŀ¼��Ŀ¼����ָ��λ�ý��Զ��������1��λ�� */
        while ((d = readdir(dirp)) != RT_NULL)
        {
            LOG_D(">>> %s, type:%d", d->d_name, d->d_type);
            lcd_set_color(WHITE, BLACK);

            rt_memset(buf, 0x0, sizeof(buf));
            /* show the null-terminated file name */
            rt_snprintf(buf, sizeof(buf), "%-16s", d->d_name);
            lcd_show_string(8, 52 + 31 * line, 24, buf);
            /* the type of the file is 1 */
            if (d->d_type == 1)
            {
                lcd_set_color(WHITE, GRAY240);
                lcd_show_string(240 - 48,  52 + 31 * line, 24, "file");
            }
            /* the type of the dir is 2 */
            else if (d->d_type == 2)
            {
                lcd_set_color(WHITE, GRAY240);
                lcd_show_string(240 - 48,  52 + 31 * line, 24, "dir ");
            }

            lcd_set_color(WHITE, GRAY240);
            lcd_draw_line(8, 52 + 31 * line - 4, 240 - 8, 52 + 31 * line - 4);
            line++;
            lcd_draw_line(8, 52 + 31 * line - 4, 240 - 8, 52 + 31 * line - 4);
            /* this line controls to show how many file and dir */
            if (line > 5)
            {
                break;
            }
        }
    }
    closedir(dirp);
    dirp = RT_NULL;
}

void iotb_lcd_update_menu_index(uint8_t menu_index)
{
    rt_mutex_take(lcd_mutex, RT_WAITING_FOREVER);
    iotb_lcd_menu_index = menu_index;
    rt_mutex_release(lcd_mutex);
}

uint8_t iotb_lcd_get_menu_index(void)
{
    uint8_t menu_index;
    rt_mutex_take(lcd_mutex, RT_WAITING_FOREVER);
    menu_index = iotb_lcd_menu_index;
    rt_mutex_release(lcd_mutex);
    return menu_index;
}

static void iotb_lcd_menu_init(void)
{
    rt_memset((void*)lcd_instance, 0x0, sizeof(lcd_instance));

    /* iotb_lcd_handle_array */
    for (int i = 1; i < IOTB_LCD_MENU_MAX + 1; i++)
    {
        lcd_instance[i].menu = i;
        lcd_instance[i].refresh_time = menu_refresh_time[i];
        lcd_instance[i].content_type = IOTB_LCD_CONTENT_NONE;
        lcd_instance[i].current_event = IOTB_LCD_EVENT_NONE;
        lcd_instance[i].lcd_handle = iotb_lcd_handle_array[i];
    }
}

rt_err_t iotb_lcd_start(void)
{
    rt_thread_t iotb_lcd_tid;

    iotb_lcd_menu_init();

    lcd_mutex = rt_mutex_create("lcd_mutex", RT_IPC_FLAG_FIFO);
    RT_ASSERT(lcd_mutex != RT_NULL);

    rt_event_init(&lcd_event, "lcd_event", RT_IPC_FLAG_FIFO);
    
    iotb_lcd_tid = rt_thread_create("lcd_thr", iotb_lcd_show, RT_NULL,
                                    IOTB_LCD_THREAD_STACK_SIZE, RT_THREAD_PRIORITY_MAX / 2 - 4, 50);
    if (iotb_lcd_tid != RT_NULL)
        rt_thread_startup(iotb_lcd_tid);
    
    return RT_EOK;
}
