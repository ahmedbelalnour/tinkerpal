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
#ifndef __PACKET_H__
#define __PACKET_H__

#include "util/tp_types.h"
#include "util/debug.h"

typedef struct {
    u8 *head;
    u8 *tail;
    u8 *ptr;
    int length;
} packet_t;

/* All network transactions are synchronous. So we only need one packet
 * at a time.
 */
extern packet_t g_packet;

static inline void *packet_push(packet_t *pkt, int len)
{
    tp_assert(pkt->ptr - pkt->head >= len);
    pkt->ptr -= len;
    pkt->length += len;
    return pkt->ptr;
}

static inline void *packet_pull(packet_t *pkt, int len)
{
    tp_assert(pkt->length >= len);
    pkt->ptr += len;
    pkt->length += len;
    return pkt->ptr;
}

static inline void packet_reset(packet_t *pkt, int head)
{
    if (head)
    {
	pkt->ptr = pkt->head;
	pkt->length = pkt->tail - pkt->head;
    }
    else
    {
	pkt->ptr = pkt->tail;
	pkt->length = 0;
    }
}

#endif
