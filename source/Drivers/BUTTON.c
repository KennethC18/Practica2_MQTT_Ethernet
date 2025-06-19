/*
 * BUTTON.c
 *
 *  Created on: 19 jun. 2025
 *      Author: kenne
 */

#include "BUTTON.h"
#include "board.h"
#include "fsl_io_mux.h"

static volatile bool s_buttonInterruptFlag = false;
static button_callback_t s_buttonCallback = NULL;

void BUTTON_Init(button_callback_t callback)
{
	/* Enables the clock for the GPIO0 module */
	GPIO_PortInit(GPIO, 0);

	gpio_pin_config_t gpio0_pinM2_config = {
		.pinDirection = kGPIO_DigitalInput,
		.outputLogic = 0U
	};
	gpio_interrupt_config_t gpio0_pinM2_int_config = {
		.mode = kGPIO_PinIntEnableEdge,
		.polarity = kGPIO_PinIntEnableLowOrFall
	};
	/* Initialize GPIO functionality on pin PIO0_11 (pin M2)  */
	GPIO_PinInit(GPIO, 0U, 11U, &gpio0_pinM2_config);
	GPIO_SetPinInterruptConfig(GPIO, 0U, 11U, &gpio0_pinM2_int_config);
	GPIO_PinEnableInterrupt(GPIO, 0U, 11U, (uint32_t)kGPIO_InterruptA);
    IO_MUX_SetPinMux(IO_MUX_GPIO11);

    gpio_pin_config_t sw_config = {kGPIO_DigitalInput, 0};
    gpio_interrupt_config_t config = {kGPIO_PinIntEnableEdge, kGPIO_PinIntEnableLowOrFall};

    s_buttonCallback = callback;

	/* Initialize GPIO for button */
	GPIO_PortInit(GPIO, BUTTON_SW_PORT);
	GPIO_PinInit(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, &sw_config);

	/* Enable interrupt */
	EnableIRQ(BUTTON_IRQ);
	GPIO_SetPinInterruptConfig(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, &config);
	GPIO_PinEnableInterrupt(GPIO, BUTTON_SW_PORT, BUTTON_SW_PIN, 0);
	NVIC_EnableIRQ(BUTTON_IRQ);
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
