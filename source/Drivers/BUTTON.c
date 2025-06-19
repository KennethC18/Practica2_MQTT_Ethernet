/*
 * BUTTON.c
 *
 *  Created on: 19 jun. 2025
 *      Author: kenne
 */

#include "BUTTON.h"
#include "board.h"

static volatile bool s_buttonInterruptFlag = false;
static button_callback_t s_buttonCallback = NULL;

status_t BUTTON_Init(button_callback_t callback)
{
    gpio_pin_config_t sw_config = {kGPIO_DigitalInput, 0};
    gpio_interrupt_config_t config = {kGPIO_PinIntEnableEdge, kGPIO_PinIntEnableLowOrFall};

    s_buttonCallback = callback;

    GPIO_PortInit(GPIO, BUTTON_SW_PORT);
    GPIO_PinInit(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, &sw_config);

    EnableIRQ(BUTTON_IRQ);
    GPIO_SetPinInterruptConfig(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, &config);
    GPIO_PinEnableInterrupt(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, 0);

    return kStatus_Success;
}

bool BUTTON_IsPressed(void)
{
    return (BUTTON_CONNECTED_LEVEL == GPIO_PinRead(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN));
}

bool BUTTON_GetInterruptFlag(void)
{
    return s_buttonInterruptFlag;
}

void BUTTON_ClearInterruptFlag(void)
{
    s_buttonInterruptFlag = false;
    GPIO_PinClearInterruptFlag(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, 0);
}

void BUTTON_IRQHandler(void)
{
    GPIO_PinClearInterruptFlag(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, 0);
    s_buttonInterruptFlag = true;

    if (s_buttonCallback != NULL)
    {
        s_buttonCallback();
    }

    SDK_ISR_EXIT_BARRIER;
}
