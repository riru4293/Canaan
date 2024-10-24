/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
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
void prime_calc(void *nouse);

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

    stdio_init_all();

    printf("hello.\n");

    /* Creates a tasks */
    result = xTaskCreate(heartbeatTask, "Heartbeat", configMINIMAL_STACK_SIZE,
                         NULL, TASK_HEARTBEAT_PRIORITY, &gHeartbeatTask);

    result = result && xTaskCreate(prime_calc, "a", configMINIMAL_STACK_SIZE * 2,
                                   NULL, TASK_HEARTBEAT_PRIORITY, NULL);

    result = result && xTaskCreate(prime_calc, "b", configMINIMAL_STACK_SIZE * 2,
                                   NULL, TASK_HEARTBEAT_PRIORITY, NULL);

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

void prime_calc(void *nouse)
{
    TickType_t start = xTaskGetTickCount();
    int count = 0;
    int i, j;
    for (i = 2; i <= 10000; i++)
    {
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
                break;
        }
        if (i == j)
        {
            count++;
        }
    }
    TickType_t end = xTaskGetTickCount();
#if configUSE_CORE_AFFINITY == 1
    int core = (vTaskCoreAffinityGet(NULL) != (1 << 0)) ? 1 : 0;
    printf("\n%d prime numbers found!\n", count);
    printf("Finish! %d Tick (Core %d)\n", end - start, core);
#else
    printf("Finish! %d Tick(%d, %d)\n", end - start, start, end);
#endif
    while (true)
    {
        vTaskDelay(HEARTBEAT_INTERVAL_MS);
    }
}
