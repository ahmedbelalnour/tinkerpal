/* Copyright (c) 2013, Eyal Birger
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/rom_map.h"
#include "util/debug.h"
#include "drivers/gpio/gpio_platform.h"
#include "drivers/serial/serial_platform.h"
#include "platform/platform.h"
#include "platform/arm/cortex-m.h"
#include "platform/arm/ti/stellaris.h"
#include "platform/arm/ti/lm4f120xl/lm4f120xl.h"

const stellaris_gpio_port_t stellaris_gpio_ports[] = {
    [GPIO_PORT_A] = { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, INT_GPIOA },
    [GPIO_PORT_B] = { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, INT_GPIOB },
    [GPIO_PORT_C] = { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, INT_GPIOC },
    [GPIO_PORT_D] = { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, INT_GPIOD },
    [GPIO_PORT_E] = { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, INT_GPIOE },
    [GPIO_PORT_F] = { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, INT_GPIOF }
};

const stellaris_uart_t stellaris_uarts[] = {
    [UART0] = { SYSCTL_PERIPH_UART0, UART0_BASE, INT_UART0, PA0, PA1 },
    [UART1] = { SYSCTL_PERIPH_UART1, UART1_BASE, INT_UART1, PB0, PB1 },
    [UART2] = { SYSCTL_PERIPH_UART2, UART2_BASE, INT_UART2, PD6, PD7 },
    [UART3] = { SYSCTL_PERIPH_UART3, UART3_BASE, INT_UART3, PC6, PC7 },
    [UART4] = { SYSCTL_PERIPH_UART4, UART4_BASE, INT_UART4, PC4, PC5 },
    [UART5] = { SYSCTL_PERIPH_UART5, UART5_BASE, INT_UART5, PE4, PE5 },
    [UART6] = { SYSCTL_PERIPH_UART6, UART6_BASE, INT_UART6, PD4, PD5 },
    [UART7] = { SYSCTL_PERIPH_UART7, UART7_BASE, INT_UART7, PE0, PE1 },
};

const stellaris_ssi_t stellaris_ssis[] = {
    [SSI0] = { .periph = SYSCTL_PERIPH_SSI0, .base = SSI0_BASE, .clk = PA2, .fss = PA3, .rx = PA4, .tx = PA5, },
    [SSI1] = { .periph = SYSCTL_PERIPH_SSI1, .base = SSI1_BASE, .clk = PD0, .fss = PD1, .rx = PD2, .tx = PD3, },
};

const stellaris_timer_t stellaris_timers[] = {
    [ TIMER0 ] = { SYSCTL_PERIPH_TIMER0, TIMER0_BASE },
    [ TIMER1 ] = { SYSCTL_PERIPH_TIMER1, TIMER1_BASE },
    [ TIMER2 ] = { SYSCTL_PERIPH_TIMER2, TIMER2_BASE },
    [ TIMER3 ] = { SYSCTL_PERIPH_TIMER3, TIMER3_BASE },
    [ TIMER4 ] = { SYSCTL_PERIPH_TIMER4, TIMER4_BASE },
    [ TIMER5 ] = { SYSCTL_PERIPH_TIMER5, TIMER5_BASE },
    [ WTIMER0 ] = { SYSCTL_PERIPH_WTIMER0, WTIMER0_BASE },
    [ WTIMER1 ] = { SYSCTL_PERIPH_WTIMER1, WTIMER1_BASE },
    [ WTIMER2 ] = { SYSCTL_PERIPH_WTIMER2, WTIMER2_BASE },
    [ WTIMER3 ] = { SYSCTL_PERIPH_WTIMER3, WTIMER3_BASE },
    [ WTIMER4 ] = { SYSCTL_PERIPH_WTIMER4, WTIMER4_BASE },
    [ WTIMER5 ] = { SYSCTL_PERIPH_WTIMER5, WTIMER5_BASE }
};

/* Notes:
 * - PC4/5 can be used for either UART1 or UART4. Currently going for 4, 
 * since PB0/1 can be used for UART1
 * - PF timers are shared with PB
 */
const stellaris_gpio_pin_t stellaris_gpio_pins[] = {
    [ PA0 ] = {-1, 0, -1, GPIO_PA0_U0RX, -1},
    [ PA1 ] = {-1, 0, -1, GPIO_PA1_U0TX, -1},
    [ PA2 ] = {-1, 0, -1, -1, GPIO_PA2_SSI0CLK},
    [ PA3 ] = {-1, 0, -1, -1, GPIO_PA3_SSI0FSS},
    [ PA4 ] = {-1, 0, -1, -1, GPIO_PA4_SSI0RX},
    [ PA5 ] = {-1, 0, -1, -1, GPIO_PA5_SSI0TX},
    [ PA6 ] = {-1, 0, -1, -1, -1},
    [ PA7 ] = {-1, 0, -1, -1, -1},
    [ PB0 ] = {TIMER2, GPIO_PB0_T2CCP0, -1, GPIO_PB0_U1RX, -1},
    [ PB1 ] = {TIMER2, GPIO_PB1_T2CCP1, -1, GPIO_PB1_U1TX, -1},
    [ PB2 ] = {TIMER3, GPIO_PB2_T3CCP0, -1, -1, -1},
    [ PB3 ] = {TIMER3, GPIO_PB3_T3CCP1, -1, -1, -1},
    [ PB4 ] = {TIMER1, GPIO_PB4_T1CCP0, ADC_CTL_CH10, -1, -1},
    [ PB5 ] = {TIMER1, GPIO_PB5_T1CCP1, ADC_CTL_CH11, -1, -1},
    [ PB6 ] = {TIMER0, GPIO_PB6_T0CCP0, -1, -1, -1},
    [ PB7 ] = {TIMER0, GPIO_PB7_T0CCP1, -1, -1, -1},
    [ PC0 ] = {TIMER4, GPIO_PC0_T4CCP0, -1, -1, -1},
    [ PC1 ] = {TIMER4, GPIO_PC1_T4CCP1, -1, -1, -1},
    [ PC2 ] = {TIMER5, GPIO_PC2_T5CCP0, -1, -1, -1},
    [ PC3 ] = {TIMER5, GPIO_PC3_T5CCP1, -1, -1, -1},
    [ PC4 ] = {WTIMER0, GPIO_PC4_WT0CCP0, -1, GPIO_PC4_U4RX, -1},
    [ PC5 ] = {WTIMER0, GPIO_PC5_WT0CCP1, -1, GPIO_PC5_U4TX, -1},
    [ PC6 ] = {WTIMER1, GPIO_PC6_WT1CCP0, -1, GPIO_PC6_U3RX, -1},
    [ PC7 ] = {WTIMER1, GPIO_PC7_WT1CCP1, -1, GPIO_PC7_U3TX, -1},
    [ PD0 ] = {WTIMER2, GPIO_PD0_WT2CCP0, ADC_CTL_CH7, -1, GPIO_PD0_SSI1CLK},
    [ PD1 ] = {WTIMER2, GPIO_PD1_WT2CCP1, ADC_CTL_CH6, -1, GPIO_PD1_SSI1FSS},
    [ PD2 ] = {WTIMER3, GPIO_PD2_WT3CCP0, ADC_CTL_CH5, -1, GPIO_PD2_SSI1RX},
    [ PD3 ] = {WTIMER3, GPIO_PD3_WT3CCP1, ADC_CTL_CH4, -1, GPIO_PD3_SSI1TX},
    [ PD4 ] = {WTIMER4, GPIO_PD4_WT4CCP0, -1, GPIO_PD4_U6RX, -1},
    [ PD5 ] = {WTIMER4, GPIO_PD5_WT4CCP1, -1, GPIO_PD5_U6TX, -1},
    [ PD6 ] = {WTIMER5, GPIO_PD6_WT5CCP0, -1, GPIO_PD6_U2RX, -1},
    [ PD7 ] = {WTIMER5, GPIO_PD7_WT5CCP1, -1, GPIO_PD7_U2TX, -1},
    [ PE0 ] = {-1, 0, ADC_CTL_CH3, GPIO_PE0_U7RX, -1},
    [ PE1 ] = {-1, 0, ADC_CTL_CH2, GPIO_PE1_U7TX, -1},
    [ PE2 ] = {-1, 0, ADC_CTL_CH1, -1, -1},
    [ PE3 ] = {-1, 0, ADC_CTL_CH0, -1, -1},
    [ PE4 ] = {-1, 0, ADC_CTL_CH9, GPIO_PE4_U5RX, -1},
    [ PE5 ] = {-1, 0, ADC_CTL_CH8, GPIO_PE5_U5TX, -1},
    [ PE6 ] = {-1, 0, -1, -1, -1},
    [ PE7 ] = {-1, 0, -1, -1, -1},
    [ PF0 ] = {TIMER0, GPIO_PF0_T0CCP0, -1, -1, -1},
    [ PF1 ] = {TIMER0, GPIO_PF1_T0CCP1, -1, -1, -1},
    [ PF2 ] = {TIMER1, GPIO_PF2_T1CCP0, -1, -1, -1},
    [ PF3 ] = {TIMER1, GPIO_PF3_T1CCP1, -1, -1, -1},
    [ PF4 ] = {TIMER2, GPIO_PF4_T2CCP0, -1, -1, -1},
    [ PF5 ] = {-1, 0, -1, -1, -1},
    [ PF6 ] = {-1, 0, -1, -1, -1},
    [ PF7 ] = {-1, 0, -1, -1, -1}
};

#define HALF_TIMER(p) ((p) & 0x1 ? TIMER_B : TIMER_A) /* even pins use TIMER_A, odd pins use TIMER_B */
#define TIMER_SET(p, t) ROM_TimerMatchSet(stellaris_timers[stellaris_gpio_pins[p].timer].base, HALF_TIMER(p), t)

#ifdef CONFIG_GPIO

static void pinmode_pwm(int pin)
{
    unsigned long timer, half_timer;

    timer = stellaris_timers[stellaris_gpio_pins[pin].timer].base;
    half_timer = HALF_TIMER(pin);

    stellaris_periph_enable(stellaris_timers[stellaris_gpio_pins[pin].timer].periph);

    /* Configure GPIO */
    stellaris_pin_mode_timer(pin);

    /* Configure Timer */
    ROM_TimerConfigure(timer, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | 
	TIMER_CFG_B_PWM));
    ROM_TimerPrescaleSet(timer, half_timer, 0); // ~1230 Hz PWM
    /* Timer will load this value on timeout */
    ROM_TimerLoadSet(timer, half_timer, 65279);
    /* Initial duty cycle of 0 */
    TIMER_SET(pin, 65278);
    /* PWM should not be inverted */
    ROM_TimerControlLevel(timer, half_timer, 0);
    
    ROM_TimerEnable(timer, half_timer);
}

static int lm4120xl_set_pin_mode(int pin, gpio_pin_mode_t mode)
{
    /* Anti-brick JTAG Protection */
    if (pin >= PC0 && pin <= PC3) 
	return -1;

    if (mode == GPIO_PM_OUTPUT_ANALOG && stellaris_gpio_pins[pin].timer == -1)
	return -1;

    stellaris_periph_enable(stellaris_gpio_periph(pin));

    switch (mode)
    {
    case GPIO_PM_INPUT:
	stellaris_gpio_input(pin);
	stellaris_pin_config(pin, GPIO_PIN_TYPE_STD);
	break;
    case GPIO_PM_OUTPUT:
	stellaris_pin_mode_output(pin);
	stellaris_pin_config(pin, GPIO_PIN_TYPE_STD);
	break;
    case GPIO_PM_INPUT_PULLUP:
	stellaris_gpio_input(pin);
	stellaris_pin_config(pin, GPIO_PIN_TYPE_STD_WPU);
	break;
    case GPIO_PM_INPUT_PULLDOWN:
	stellaris_gpio_input(pin);
	stellaris_pin_config(pin, GPIO_PIN_TYPE_STD_WPD);
	break;
    case GPIO_PM_INPUT_ANALOG:
	if (stellaris_pin_mode_adc(pin))
	    return -1;
	break;
    case GPIO_PM_OUTPUT_ANALOG:
	pinmode_pwm(pin);
	break;
    }
    return 0;
}

static void stellaris_gpio_analog_write(int pin, double value) 
{
    int int_val;

    int_val = 255 * value;

    if (int_val > 255)
	int_val = 255;
    
    if (int_val == 0)
	int_val = 65278;
    else
	int_val = 65280 - (256 * int_val);

    TIMER_SET(pin, int_val);
}

#endif

static int stellaris_serial_enable(int u, int enabled)
{
    stellaris_uart_enable(u, enabled);
    return 0;
}

static void buttons_init(void)
{
#ifdef CONFIG_GPIO
    /* Enable the GPIO port that is used for the on-board Buttons*/
    stellaris_periph_enable(SYSCTL_PERIPH_GPIOF);

    /* Unlock PF0 so we can change it to a GPIO input
     * Once we have enabled (unlocked) the commit register then re-lock it
     * to prevent further changes.  PF0 is muxed with NMI thus a special case.
     */
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    lm4120xl_set_pin_mode(PF0, GPIO_PM_INPUT_PULLUP);
    lm4120xl_set_pin_mode(PF4, GPIO_PM_INPUT_PULLUP);
#endif
}

static void lm4f120xl_init(void)
{
    /* Enable lazy stacking for interrupt handlers.  This allows floating-point
     * instructions to be used within interrupt handlers, but at the expense of
     * extra stack usage.
     */
    ROM_FPULazyStackingEnable();

    /* Set the clocking to run directly from the crystal at 80 MHz */
    ROM_SysCtlClockSet(SYSCTL_RCC2_DIV400 | SYSCTL_SYSDIV_2_5 |
	SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
                       
    stellaris_systick_init();
    buttons_init(); 
}

const platform_t platform = {
    .serial = {
	.enable = stellaris_serial_enable,
	.read = buffered_serial_read,
	.write = stellaris_serial_write,
	.irq_enable = stellaris_serial_irq_enable,
    },
#ifdef CONFIG_GPIO
    .gpio = {
	.digital_write = stellaris_gpio_digital_write,
	.digital_read = stellaris_gpio_digital_read,
	.analog_write = stellaris_gpio_analog_write,
	.analog_read = stellaris_gpio_analog_read,
	.set_pin_mode = lm4120xl_set_pin_mode,
	.set_port_val = stellaris_gpio_set_port_val,
	.get_port_val = stellaris_gpio_get_port_val,
    },
#endif
#ifdef CONFIG_SPI
    .spi = {
	.init = stellaris_spi_init,
	.reconf = stellaris_spi_reconf,
	.set_max_speed = stellaris_spi_set_max_speed,
	.send = stellaris_spi_send,
	.receive = stellaris_spi_receive,
    },
#endif
    .init = lm4f120xl_init,
    .meminfo = cortex_m_meminfo,
    .panic = cortex_m_panic,
    .select = stellaris_select,
    .get_ticks_from_boot = cortex_m_get_ticks_from_boot,
    .get_system_clock = stellaris_get_system_clock,
    .msleep = stellaris_msleep,
};