/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 * Andy Kobyljanec
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#define TOGGLE_LED 01
#define LOG_STRING 02

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

TimerHandle_t led_2hz_timer, led_4hz_timer;
BaseType_t xTimer1Start, xTimer2Start, xtask3;

TaskHandle_t task_3_handle;
QueueHandle_t queue_handle;
uint32_t g_ui32SysClock;
int a=0,b=0;


typedef struct msg_data
{
    char msg[50];
    TickType_t ticks_n;
}data_pika;

void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

void func_led_2hz_timer(TimerHandle_t led_2hz_timer)
{

    //UARTprintf("Toggle 2hz \n");
    xTaskNotify(task_3_handle, TOGGLE_LED, eSetBits);
}

void func_led_4hz_timer(TimerHandle_t led_4hz_timer)
{
    data_pika data_send;
    strcpy(data_send.msg, "Notification from Task 2");
    data_send.ticks_n = xTaskGetTickCount();

    queue_handle = xQueueCreate(10, sizeof(data_pika));

    xQueueSend(queue_handle, &data_send, 500);

//    UARTprintf("Toggle 4hz \n");
    xTaskNotify(task_3_handle, LOG_STRING, eSetBits);
}

void func_led_2hz(void *pvParameters)
{
    led_2hz_timer = xTimerCreate("LED2HzTimer", pdMS_TO_TICKS(250), pdTRUE, (void *)pvTimerGetTimerID(led_2hz_timer), func_led_2hz_timer);

    if (led_2hz_timer != NULL)
    {
        xTimerStart(led_2hz_timer, 250);

    }
    else
    {
        UARTprintf("Timer 1 Creation Failed\n");
    }

    while(1)
    {

    }
}

void func_led_4hz(void *pvParameters)
{
    led_4hz_timer = xTimerCreate("LED4HzTimer", pdMS_TO_TICKS(125), pdTRUE, (void *)pvTimerGetTimerID(led_4hz_timer), func_led_4hz_timer);

        if (led_4hz_timer != NULL)
        {
            xTimerStart(led_4hz_timer, 125);

        }
        else
        {
            UARTprintf("Timer 2 Creation Failed\n");
        }

        while(1)
        {

        }
}

void func_task_3(void *pvParameters)
{
    int val_recv;
    data_pika data_recv;
    while(1)
    {
        xtask3 = xTaskNotifyWait(0, 0xFF, &val_recv, portMAX_DELAY);
        if(xtask3 == pdTRUE)
        {
            if(val_recv & TOGGLE_LED)
            {
                if(a==0)
                {
                        LEDWrite(CLP_D1, 1);
                        a=1;
                }
                else if(a==1)
                {
                      LEDWrite(CLP_D1, 0);
                        a=0;
                }
                UARTprintf("Message from Task 1\n");
            }
            if(val_recv & LOG_STRING)
            {
                xQueueReceive(queue_handle, &data_recv, 500);
                UARTprintf("Data Received : %s, Ticks : %d \n",data_recv.msg, data_recv.ticks_n);
            }

        }
    }
}


// Main function
int main(void)
{
    // Initialize system clock to 120 MHz

    g_ui32SysClock = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(g_ui32SysClock == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    //
    // Enable the GPIO pins for the LED D1 (PN1).
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    ConfigureUART();

    xTaskCreate(func_led_2hz, (const portCHAR *)"LED2HzTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(func_led_4hz, (const portCHAR *)"LED4HzTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(func_task_3, (const portCHAR *)"Task3", configMINIMAL_STACK_SIZE, NULL , 1, &task_3_handle);

    UARTprintf("Project for Sharanjeet Singh Mago  Date :4/7/18\n");

    vTaskStartScheduler();
    return 0;
}



/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
