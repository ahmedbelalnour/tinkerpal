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
#ifndef __ETHERIF_H__
#define __ETHERIF_H__

#include "util/event.h"
#include "util/tp_types.h"
#include "drivers/resources.h"

typedef enum {
    ETHERIF_EVENT_PORT_CHANGE = 0,
    ETHERIF_EVENT_PACKET_RECEIVED = 1,
    ETHERIF_EVENT_PACKET_XMITTED = 2,
    ETHERIF_EVENT_COUNT
} etherif_event_t;

#define ETHERIF_RES(ethif, event) \
    RES(ETHERIF_RESOURCE_ID_BASE, (ethif)->id, event)

typedef struct etherif_t etherif_t;

typedef struct {
    int (*link_status)(etherif_t *ethif);
    int (*packet_size)(etherif_t *ethif);
    int (*packet_recv)(etherif_t *ethif, u8 *buf, int size);
    void (*packet_xmit)(etherif_t *ethif, u8 *buf, int size);
} etherif_ops_t;

struct etherif_t {
    etherif_t *next;
    int id;
    const etherif_ops_t *ops;
    int watches[ETHERIF_EVENT_COUNT];
};

void etherif_uninit(etherif_t *ethif);
void etherif_init(etherif_t *ethif, const etherif_ops_t *ops);

etherif_t *etherif_get_by_id(int id);

static inline void etherif_on_event_set(etherif_t *ethif, etherif_event_t event,
    event_t *ev)
{
    ethif->watches[event] = event_watch_set(ETHERIF_RES(ethif, event), ev);
}

static inline void etherif_event_trigger(etherif_t *ethif,
    etherif_event_t event)
{
    event_watch_trigger(ETHERIF_RES(ethif, event));
}

#define etherif_on_port_change_event_set(ethif, ev) \
    etherif_on_event_set(ethif, ETHERIF_EVENT_PORT_CHANGE, ev)
#define etherif_port_changed(ethif) \
    etherif_event_trigger(ethif, ETHERIF_EVENT_PORT_CHANGE);

#define etherif_on_packet_received_event_set(ethif, ev) \
    etherif_on_event_set(ethif, ETHERIF_EVENT_PACKET_RECEIVED, ev)
#define etherif_packet_received(ethif) \
    etherif_event_trigger(ethif, ETHERIF_EVENT_PACKET_RECEIVED);

#define etherif_on_packet_xmit_event_set(ethif, ev) \
    etherif_on_event_set(ethif, ETHERIF_EVENT_PACKET_XMITTED, ev)
#define etherif_packet_xmitted(ethif) \
    etherif_event_trigger(ethif, ETHERIF_EVENT_PACKET_XMITTED);

#endif
