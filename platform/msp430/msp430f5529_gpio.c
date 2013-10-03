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
#include <msp430.h>
#include "drivers/gpio/gpio.h"
#include "platform/msp430/msp430f5529_gpio.h"

typedef struct {
    volatile unsigned char *dir; /* 0 - Input, 1 Output */
    volatile unsigned char *out; /* Output */
    volatile unsigned char *in; /* Input */
    volatile unsigned char *sel; /* 0 - I/O function, 1 special function */
    volatile unsigned char *ren; /* 0 - Pullup disabled, 1 Pullup enabled */
} msp430f5529_gpio_t;

static const msp430f5529_gpio_t msp430f5529_gpio_ports[] = {
#define P(p) { \
    .dir = (volatile unsigned char *)&p##DIR, \
    .out = (volatile unsigned char *)&p##OUT, \
    .in = (volatile unsigned char *)&p##IN, \
    .sel = (volatile unsigned char *)&p##SEL, \
    .ren = (volatile unsigned char *)&p##REN, \
}
    [GPIO_PORT_A] = P(P1),
    [GPIO_PORT_B] = P(P2),
    [GPIO_PORT_C] = P(P3),
    [GPIO_PORT_D] = P(P4),
    [GPIO_PORT_E] = P(P5),
    [GPIO_PORT_F] = P(P6),
    [GPIO_PORT_G] = P(P7),
    [GPIO_PORT_H] = P(P8),
#undef P
};

#define bit_set(x, bit, val) x = (val) ? x | (bit) : x & ~(bit)
#define bit_get(x, bit) (!!((x) & (bit)))

static inline void msp430f5529_gpio_function(int port, int bit, int non_io)
{
    bit_set(*msp430f5529_gpio_ports[port].sel, bit, non_io);
}

static inline void msp430f5529_gpio_dir(int port, int bit, int out)
{
    bit_set(*msp430f5529_gpio_ports[port].dir, bit, out);
}

static inline void msp430f5529_gpio_pullup(int port, int bit, int on)
{
    bit_set(*msp430f5529_gpio_ports[port].ren, bit, on);
}

static inline void msp430f5529_gpio_out(int port, int bit, int out)
{
    bit_set(*msp430f5529_gpio_ports[port].out, bit, out);
}

static inline int msp430f5529_gpio_in(int port, int bit)
{
    return bit_get(*msp430f5529_gpio_ports[port].in, bit);
}

int msp430f5529_gpio_set_pin_mode(int pin, gpio_pin_mode_t mode)
{
    int port = GPIO_PORT(pin), bit = GPIO_BIT(pin);

    msp430f5529_gpio_function(port, bit, 0);

    switch (mode)
    {
    case GPIO_PM_OUTPUT:
	msp430f5529_gpio_dir(port, bit, 1);
	break;
    case GPIO_PM_INPUT:
	msp430f5529_gpio_dir(port, bit, 0);
	break;
    case GPIO_PM_INPUT_PULLUP:
	msp430f5529_gpio_dir(port, bit, 0);
	msp430f5529_gpio_pullup(port, bit, 1);
	break;
    case GPIO_PM_INPUT_PULLDOWN:
	msp430f5529_gpio_dir(port, bit, 0);
	msp430f5529_gpio_pullup(port, bit, 0);
	break;
    case GPIO_PM_INPUT_ANALOG:
    case GPIO_PM_OUTPUT_ANALOG:
    default:
	return -1;
    }

    return 0;
}

void msp430f5529_gpio_digital_write(int pin, int value)
{
    msp430f5529_gpio_out(GPIO_PORT(pin), GPIO_BIT(pin), value);
}

int msp430f5529_gpio_digital_read(int pin)
{
    return msp430f5529_gpio_in(GPIO_PORT(pin), GPIO_BIT(pin));
}