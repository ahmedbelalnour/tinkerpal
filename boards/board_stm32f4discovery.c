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
#include "boards/board.h"
#include "platform/platform.h"

#ifdef CONFIG_ILI93XX
extern ili93xx_db_transport_t stm32_fsmc_ili93xx_trns;
#endif

const board_t board = {
    .desc = "STM32F4Discovery",
    .default_console_id = UART_RES(USART_PORT2),
#ifdef CONFIG_SSD1306
    .ssd1306_params = {
        .i2c_port = I2C_RES(I2C_PORT1),
	.i2c_addr = 0x78,
    },
#endif
#ifdef CONFIG_ENC28J60
    .enc28j60_params = {
        .spi_port = SPI_RES(SPI_PORT1),
        .cs = GPIO_RES(PC5),
        .intr = GPIO_RES(PB1),
    },
#endif
#ifdef CONFIG_ILI93XX
    .ili93xx_params = {
        .trns = &stm32_fsmc_ili93xx_trns,
        .rst = GPIO_RES(PE1),
        .backlight = GPIO_RES(PB10),
    },
#endif
};
