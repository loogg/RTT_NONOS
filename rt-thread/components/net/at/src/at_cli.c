/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-04-02     armink       first version
 */

#include <at.h>
#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>

#ifdef AT_USING_CLI

#define AT_CLI_FIFO_SIZE                      256

static struct rt_ringbuffer *console_rx_fifo = RT_NULL;
static rt_err_t (*odev_rx_ind)(rt_device_t dev, rt_size_t size) = RT_NULL;
static rt_uint16_t odev_thread_belong = RT_DEVICE_BELONG_NULL;

#ifdef AT_USING_CLIENT
static struct rt_ringbuffer *client_rx_fifo = RT_NULL;
#endif 

static char console_getchar(void)
{
    char ch = 0;
 
    rt_ringbuffer_getchar(console_rx_fifo, (rt_uint8_t *)&ch);

    return ch;
}

static rt_err_t console_getchar_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch;
    rt_size_t i;

    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            rt_ringbuffer_put_force(console_rx_fifo, &ch, 1);
        }
    }

    return RT_EOK;
}

void at_cli_init(void)
{
    rt_base_t int_lvl;
    rt_device_t console;


    /* create RX FIFO */
    console_rx_fifo = rt_ringbuffer_create(AT_CLI_FIFO_SIZE);
    /* created must success */
    RT_ASSERT(console_rx_fifo);

    int_lvl = rt_hw_interrupt_disable();
    console = rt_console_get_device();
    if (console)
    {
        /* backup RX indicate */
        odev_rx_ind = console->rx_indicate;
        odev_thread_belong = console->thread_belong;
        rt_device_set_rx_indicate(console, console_getchar_rx_ind);
        console->thread_belong = RT_DEVICE_AT_CLI_BELONG;
    }

    rt_hw_interrupt_enable(int_lvl);
}

void at_cli_deinit(void)
{
    rt_base_t int_lvl;
    rt_device_t console;

    int_lvl = rt_hw_interrupt_disable();
    console = rt_console_get_device();
    if (console)
    {
        /* restore RX indicate */
        rt_device_set_rx_indicate(console, odev_rx_ind);
        console->thread_belong = odev_thread_belong;
    }
    
    rt_hw_interrupt_enable(int_lvl);

    rt_ringbuffer_destroy(console_rx_fifo);
}

#ifdef AT_USING_SERVER
static void server_cli_parser(void)
{
    extern at_server_t at_get_server(void);

    at_server_t server = at_get_server();
    rt_base_t int_lvl;
    static rt_device_t device_bak;
    static char (*getchar_bak)(void);
    static char endmark_back[AT_END_MARK_LEN];

    /* backup server device and getchar function */
    {
        int_lvl = rt_hw_interrupt_disable();

        device_bak = server->device;
        getchar_bak = server->get_char;

        memset(endmark_back, 0x00, AT_END_MARK_LEN);
        memcpy(endmark_back, server->end_mark, strlen(server->end_mark));

        /* setup server device as console device */
        server->device = rt_console_get_device();
        server->get_char = console_getchar;

        memset(server->end_mark, 0x00, AT_END_MARK_LEN);
        server->end_mark[0] = '\r';

        rt_hw_interrupt_enable(int_lvl);
    }

    if (server)
    {
        rt_kprintf("======== Welcome to using RT-Thread AT command server cli ========\n");
        rt_kprintf("Input your at command for test server. Press 'ESC' to exit.\n");
        server->parser_entry(server);
    }
    else
    {
        rt_kprintf("AT client not initialized\n");
    }

    /* restore server device and getchar function */
    {
        int_lvl = rt_hw_interrupt_disable();

        server->device = device_bak;
        server->get_char = getchar_bak;

        memset(server->end_mark, 0x00, AT_END_MARK_LEN);
        memcpy(server->end_mark, endmark_back, strlen(endmark_back));

        rt_hw_interrupt_enable(int_lvl);
    }
}
#endif /* AT_USING_SERVER */

#ifdef AT_USING_CLIENT
static char client_getchar(void)
{
    char ch = 0;

    rt_ringbuffer_getchar(client_rx_fifo, (rt_uint8_t *)&ch);

    return ch;
}

static void at_client_entry(void *param)
{
    char ch;
    ch = client_getchar();
    if(ch == 0)
        return;
    rt_kprintf("%c", ch);
}

static rt_err_t client_getchar_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch;
    rt_size_t i;

    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            rt_ringbuffer_put_force(client_rx_fifo, &ch, 1);
        }
    }

    return RT_EOK;
}
static void client_cli_parser(void *param)
{
#define ESC_KEY                 0x1B
#define BACKSPACE_KEY           0x08
#define DELECT_KEY              0x7F

    char ch;
    static char cur_line[FINSH_CMD_SIZE] = { 0 };
    static rt_size_t cur_line_len = 0;
    static rt_err_t (*client_odev_rx_ind)(rt_device_t dev, rt_size_t size) = RT_NULL;
    static rt_uint16_t client_odev_thread_belong = RT_DEVICE_BELONG_NULL;
    static at_status_t client_odev_status;
    static rt_thread_t at_client = RT_NULL;
    static int parser_step = 0;
    rt_base_t int_lvl;
    
    at_client_t client = (at_client_t)param;

    if (client)
    {
        switch (parser_step)
        {
        case 0:
            at_cli_init();
            rt_memset(cur_line, 0, sizeof(cur_line));
            cur_line_len = 0;
            client_odev_rx_ind = RT_NULL;
            client_odev_thread_belong = RT_DEVICE_BELONG_NULL;
            client_odev_status = AT_STATUS_UNINITIALIZED;
            at_client = RT_NULL;
            client_rx_fifo = RT_NULL;
            parser_step++;
            break;
        case 1:
            if (client_rx_fifo == RT_NULL)
            {
                client_rx_fifo = rt_ringbuffer_create(AT_CLI_FIFO_SIZE);
                if (!client_rx_fifo)
                {
                    rt_kprintf("No mem for AT cli client\n");
                    return;
                }
            }

            if (at_client == RT_NULL)
            {
                at_client = rt_thread_create("at_cli", at_client_entry, RT_NULL, 0);
                if (!at_client)
                {
                    rt_kprintf("No mem for AT cli client\n");
                    return;
                }
            }

            /* backup client status */
            {
                client_odev_status = client->status;
                client->status = AT_STATUS_CLI;
            }

            /* backup client device RX indicate */
            {
                int_lvl = rt_hw_interrupt_disable();
                client_odev_rx_ind = client->device->rx_indicate;
                client_odev_thread_belong = client->device->thread_belong;
                rt_device_set_rx_indicate(client->device, client_getchar_rx_ind);
                rt_hw_interrupt_enable(int_lvl);
            }

            rt_kprintf("======== Welcome to using RT-Thread AT command client cli ========\n");
            rt_kprintf("Cli will forward your command to server port(%s). Press 'ESC' to exit.\n", client->device->parent.name);
            rt_thread_startup(at_client);
            parser_step++;
            break;
        case 2:
            /* process user input */
            ch = console_getchar();
            if(ch == 0)
                return;
            if(ch != ESC_KEY)
            {
                if (ch == BACKSPACE_KEY || ch == DELECT_KEY)
                {
                    if (cur_line_len)
                    {
                        cur_line[--cur_line_len] = 0;
                        rt_kprintf("\b \b");
                    }
                    return;
                }
                else if (ch == '\r' || ch == '\n')
                {
                    /* execute a AT request */
                    if (cur_line_len)
                    {
                        rt_kprintf("\n");
                        at_obj_exec_cmd(client, RT_NULL, "%.*s", cur_line_len, cur_line);
                    }
                    cur_line_len = 0;
                }
                else
                {
                    rt_kprintf("%c", ch);
                    cur_line[cur_line_len++] = ch;
                }
            }
            else
            {
                rt_kprintf("======== exit cli ========\n");
                parser_step++;
            }
            break;
        case 3:
            /* restore client status */
            client->status = client_odev_status;

            /* restore client device RX indicate */
            {
                int_lvl = rt_hw_interrupt_disable();
                rt_device_set_rx_indicate(client->device, client_odev_rx_ind);
                client->device->thread_belong = client_odev_thread_belong;
                rt_hw_interrupt_enable(int_lvl);
            }

            rt_thread_delete(at_client);
            rt_ringbuffer_destroy(client_rx_fifo);
            rt_thread_delete(rt_thread_self());
            at_cli_deinit();
            parser_step = 0;
            break;
        default:
            break;
        }
       
    }
    else
    {
        rt_thread_delete(rt_thread_self());
        rt_kprintf("AT client not initialized\n");
    }
}
#endif /* AT_USING_CLIENT */

static void at(int argc, char **argv)
{
    rt_thread_t cli_parser = RT_NULL;
    if (argc != 2 && argc != 3)
    {
        rt_kprintf("Please input '<server|client [dev_name]>' \n");
        return;
    }

    if (!strcmp(argv[1], "server"))
    {
#ifdef AT_USING_SERVER
        server_cli_parser();
#else
        rt_kprintf("Not support AT server, please check your configure!\n");
#endif /* AT_USING_SERVER */
    }
    else if (!strcmp(argv[1], "client"))
    {
#ifdef AT_USING_CLIENT
        at_client_t client = RT_NULL;

        if (argc == 2)
        {
            client = at_client_get_first();
        }
        else if (argc == 3)
        {
            client = at_client_get(argv[2]);
            if (client == RT_NULL)
            {
                rt_kprintf("input AT client device name(%s) error.\n", argv[2]);
                return;
            }
        }
        cli_parser = rt_thread_create("cli_parser", client_cli_parser, client, 0);
        rt_thread_startup(cli_parser);
#else
        rt_kprintf("Not support AT client, please check your configure!\n");
#endif /* AT_USING_CLIENT */
    }
    else
    {
        rt_kprintf("Please input '<server|client [dev_name]>' \n");
    }

}
MSH_CMD_EXPORT(at, RT-Thread AT component cli: at <server|client [dev_name]>);

#endif /* AT_USING_CLI */
