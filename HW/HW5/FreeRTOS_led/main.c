/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 * Andy Kobyljanec
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include <stdint.h>
#include <stdbool.h>
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
BaseType_t xTimer1Start, xTimer2Start;

uint32_t g_ui32SysClock;
int a=0,b=0;

// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

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
    UARTprintf("Toggle 2hz \n");
}

void func_led_4hz_timer(TimerHandle_t led_4hz_timer)
{
    if(b==0)
    {
        LEDWrite(CLP_D2, 2);
        b=1;
    }
    else if(b==1)
    {
        LEDWrite(CLP_D2, 0);
        b=0;
    }
    UARTprintf("Toggle 4hz \n");
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
        UARTprintf("Timer Creation Failed\n");
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
            UARTprintf("Timer Creation Failed\n");
        }

        while(1)
        {

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
