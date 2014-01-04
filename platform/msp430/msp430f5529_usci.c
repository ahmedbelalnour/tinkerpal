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
#include "platform/msp430/msp430f5529.h"
#include "platform/msp430/msp430f5529_usci.h"

static unsigned char dummy_reg;

#define UCB0MCTL dummy_reg /* No modulation on USCI_B */
#define UCB1MCTL dummy_reg /* No modulation on USCI_B */

const msp430f5529_usci_t msp430f5529_uscis[] = {
#define U(uc) { \
    .ctl0 = &uc##CTL0, \
    .ctl1 = &uc##CTL1, \
    .br0 = &uc##BR0, \
    .br1 = &uc##BR1, \
    .mctl = &uc##MCTL, \
    .ie = &uc##IE, \
    .stat = &uc##STAT, \
    .txbuf = &uc##TXBUF, \
    .rxbuf = &uc##RXBUF, \
    .ifg = &uc##IFG, \
}
    [USCIA0] = U(UCA0),
    [USCIA1] = U(UCA1),
    [USCIB0] = U(UCB0),
    [USCIB1] = U(UCB1),
#undef U
};
