/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <pico/stdlib.h>
#include <bsp/board_api.h>
#include <tusb.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <timers.h>
#include <stream_buffer.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LED_PORT (25U)

#define HEARTBEAT_PRIORITY (1U)
#define HEARTBEAT_STACK_LEN (configMINIMAL_STACK_SIZE)
#define HEARTBEAT_PERIOD_MS (250U)

#define USBD_PRIORITY (3U)
#define USBD_STACK_LEN (3 * configMINIMAL_STACK_SIZE / 2) * (CFG_TUSB_DEBUG ? 2 : 1)

#define CDC_PRIORITY (2U)
#define CDC_STACK_LEN (configMINIMAL_STACK_SIZE)
#define CDC_RECV_LEN (64U)

#define CMD_RECV_PRIORITY (1U)
#define CMD_RECV_STACK_LEN (configMINIMAL_STACK_SIZE)

#define UART_RECV_BUFF_LEN (1024U)
#define UART_RECV_BUFF_TRIGGER_LV (1U)

#define NO_DELAY (0U)

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void heartbeatTask(void *nouse);
static void usbdTask(void *nouse);
static void cdcTask(void *nouse);
static void cmdRecvTask(void *nouse);

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static TaskHandle_t gHbTaskHndl = NULL;
static TaskHandle_t gUsbdTaskHndl = NULL;
static TaskHandle_t gCdcTaskHndl = NULL;
static TaskHandle_t gCmdRecvHndl = NULL;

static StaticTask_t gHbTaskDef;
static StaticTask_t gUsbdTaskDef;
static StaticTask_t gCdcTaskDef;
static StaticTask_t gCmdRecvDef;

static StackType_t gHbStack[HEARTBEAT_STACK_LEN];
static StackType_t gUsbdStack[USBD_STACK_LEN];
static StackType_t gCdcStack[CDC_STACK_LEN];
static StackType_t gCmdRecvStack[CMD_RECV_STACK_LEN];

static StreamBufferHandle_t gUartRecvBuff = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
int main(void)
{
    /* Initialize board. */
    board_init();

    /* Initialize LED. */
    gpio_init(LED_PORT);
    gpio_set_dir(LED_PORT, GPIO_OUT);

    /* Initialize UART recv buffer. */
    gUartRecvBuff = xStreamBufferCreate(UART_RECV_BUFF_LEN, UART_RECV_BUFF_TRIGGER_LV);

    /* Creates a tasks. */
    gHbTaskHndl = xTaskCreateStatic(heartbeatTask, "hb", HEARTBEAT_STACK_LEN, NULL,
                                    HEARTBEAT_PRIORITY, gHbStack, &gHbTaskDef);

    gUsbdTaskHndl = xTaskCreateStatic(usbdTask, "usbd", USBD_STACK_LEN, NULL,
                                      USBD_PRIORITY, gUsbdStack, &gUsbdTaskDef);

    gCdcTaskHndl = xTaskCreateStatic(cdcTask, "cdc", CDC_STACK_LEN, NULL,
                                     CDC_PRIORITY, gCdcStack, &gCdcTaskDef);

    gCmdRecvHndl = xTaskCreateStatic(cmdRecvTask, "cmdRecv", CMD_RECV_STACK_LEN, NULL,
                                     CMD_RECV_PRIORITY, gCmdRecvStack, &gCmdRecvDef);

    /* Start task scheduking. */
    vTaskStartScheduler();

    while (true)
    {
        /* Unreachable */
    }
}

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
static void heartbeatTask(void *nouse)
{
    static bool isLit = false;

    while (true)
    {
        /* Flip the lighting flag */
        isLit = !isLit;

        /* Change the LED's lighting state */
        gpio_put(LED_PORT, isLit);

        /* Delay 500ms */
        vTaskDelay(HEARTBEAT_PERIOD_MS);
    }
}

static void usbdTask(void *nouse)
{
    /* Init device stack on configured roothub port.                                        */
    /* This should be called after scheduler/kernel is started.                             */
    /* Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API. */
    tud_init(BOARD_TUD_RHPORT);
    board_init_after_tusb();

    while (true)
    {
        /* Put this thread to waiting state until there is new events. */
        tud_task();

        /* Following code only run if tud_task() process at least 1 event. */
        tud_cdc_write_flush();

        /* CFG_TUSB_OS=OPT_OS_FREERTOSに設定できれば、tud_taskがFreeRTOSの   */
        /* タスク制御を行うが、pico-sdk(2.0.0)のissueにより設定できない。    */
        /* そのため無限ループ化するので、1msループとなるようにしている。     */
        /* Issue解消後は、本1ms待機を削除すること。                          */
        vTaskDelay(1);
    }
}

static void cdcTask(void *nouse)
{
    while (true)
    {
        /* Connected check for DTR bit.                                      */
        /* Most but not all terminal client set this when making connection. */
        if (tud_cdc_connected())
        {
            /* There are data available. */
            while (tud_cdc_available() &&
                   (xStreamBufferSpacesAvailable(gUartRecvBuff) >= CDC_RECV_LEN))
            {
                uint8_t buff[CDC_RECV_LEN];

                /* Read a characters. */
                uint32_t n = tud_cdc_read(buff, CDC_RECV_LEN);

                xStreamBufferSend(gUartRecvBuff, buff, n, NO_DELAY);

                /* Echo back. */
                // tud_cdc_write(buff, n);
            }

            // tud_cdc_write_flush();
        }

        vTaskDelay(1);
    }
}

static void cmdRecvTask(void *nouse)
{
    while (true)
    {
        uint8_t b;
        xStreamBufferReceive(gUartRecvBuff, &b, 1, portMAX_DELAY);
        tud_cdc_write(&b, 1);
        tud_cdc_write_flush();
    }
}
