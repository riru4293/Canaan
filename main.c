/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <stdlib.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LED_PORT (25U)
#define HEARTBEAT_INTERVAL_MS (250U)
#define TASK_HEARTBEAT_PRIORITY (1U)

/* -------------------------------------------------------------------------- */
/* Type definition                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Prototype                                                                  */
/* -------------------------------------------------------------------------- */
static void heartbeatTask(void *nouse);

/* -------------------------------------------------------------------------- */
/* Global                                                                     */
/* -------------------------------------------------------------------------- */
static TaskHandle_t gHeartbeatTask = NULL;

/* -------------------------------------------------------------------------- */
/* Public function                                                            */
/* -------------------------------------------------------------------------- */
int main(void)
{
    BaseType_t result;

    /* Initialize LED */
    gpio_init(LED_PORT);
    gpio_set_dir(LED_PORT, GPIO_OUT);

    /* Creates a tasks */
    result = xTaskCreate(heartbeatTask, "Heartbeat", configMINIMAL_STACK_SIZE,
                         NULL, TASK_HEARTBEAT_PRIORITY, &gHeartbeatTask);

    if (pdPASS == result)
    {
        vTaskStartScheduler();

        while (true)
        {
            /* Unreachable */
        }
    }

    exit(EXIT_FAILURE);
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
        vTaskDelay(HEARTBEAT_INTERVAL_MS);
    }
}
