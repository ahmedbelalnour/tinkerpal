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
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "driverlib/interrupt.h"
#include "platform/platform.h"
#include "platform/arm/ti/ti_arm_mcu.h"
#include "usb/usbd_core_platform.h"

static unsigned long ctrl_istat, endp_istat;

static unsigned int ep_map(int ep)
{
    switch (ep)
    {
    case USBD_EP0: return USB_EP_0;
    case USBD_EP1: return USB_EP_1;
    case USBD_EP2: return USB_EP_2;
    }
    return 0;
}

void ti_arm_mcu_usb_set_addr(unsigned short addr)
{
    MAP_USBDevAddrSet(USB0_BASE, addr);
}

void ti_arm_mcu_usb_ep_data_ack(int ep, int data_phase)
{
    MAP_USBDevEndpointDataAck(USB0_BASE, ep_map(ep), data_phase ? false : true);
}

int ti_arm_mcu_usb_ep_data_send(int ep, const unsigned char *data,
    unsigned long len, int last)
{
    unsigned int mapped_ep = ep_map(ep);

    if (len)
    {
        if (MAP_USBEndpointDataPut(USB0_BASE, mapped_ep,
            ((unsigned char *)(unsigned long)data), len))
        {
            return -1;
        }
    }
    
    return MAP_USBEndpointDataSend(USB0_BASE, mapped_ep, last ?
        USB_TRANS_IN_LAST : USB_TRANS_IN);
}

int ti_arm_mcu_usb_ep_data_get(int ep, unsigned char *data, unsigned long len)
{
    if (MAP_USBEndpointDataGet(USB0_BASE, ep_map(ep), data, &len))
        return -1;

    return len;
}

static unsigned int ep_mode_flag(usb_ep_type_t type)
{
    switch (type)
    {
    case USB_EP_TYPE_CTRL: return USB_EP_MODE_CTRL;
    case USB_EP_TYPE_BULK: return USB_EP_MODE_BULK;
    case USB_EP_TYPE_INTERRUPT: return USB_EP_MODE_INT;
    case USB_EP_TYPE_ISOC: return USB_EP_MODE_ISOC;
    }
    return 0;
}

void ti_arm_mcu_usb_ep_cfg(int ep, int max_pkt_size_in, int max_pkt_size_out,
    usb_ep_type_t type)
{
    unsigned int mode_flag = ep_mode_flag(type);

    if (ep == USBD_EP0)
        return;

    MAP_USBDevEndpointConfigSet(USB0_BASE, ep_map(ep), max_pkt_size_in,
        USB_EP_DEV_IN | mode_flag);
    MAP_USBDevEndpointConfigSet(USB0_BASE, ep_map(ep), max_pkt_size_out,
        mode_flag);
}

int ti_arm_mcu_usbd_event_process(void)
{
    unsigned long ctrl, endp;

    MAP_IntDisable(INT_USB0);
    ctrl = ctrl_istat;
    endp = endp_istat;
    ctrl_istat = endp_istat = 0;
    MAP_IntEnable(INT_USB0);

    if (!ctrl && !endp)
        return 0;

    if (ctrl & USB_INTCTRL_RESET)
        usbd_event(0, USB_DEVICE_EVENT_RESET);
    if (endp & USB_INTEP_0)
        usbd_event(USBD_EP0, USB_DEVICE_EVENT_EP_DATA_READY);
    if (endp & USB_INTEP_DEV_OUT_1)
        usbd_event(USBD_EP1, USB_DEVICE_EVENT_EP_DATA_READY);
    if (endp & USB_INTEP_DEV_OUT_2)
        usbd_event(USBD_EP2, USB_DEVICE_EVENT_EP_DATA_READY);
    return 0;
}

void ti_arm_mcu_usb_isr(void)
{
    ctrl_istat |= MAP_USBIntStatusControl(USB0_BASE);
    endp_istat |= MAP_USBIntStatusEndpoint(USB0_BASE);
    if (endp_istat & USB_INTEP_0 &&
        !(MAP_USBEndpointStatus(USB0_BASE, USB_EP_0) & USB_DEV_EP0_OUT_PKTRDY))
    {
        usbd_event(USBD_EP0, USB_DEVICE_EVENT_EP_WRITE_ACK);
        endp_istat &= ~USB_INTEP_0;
    }
    if (endp_istat & USB_INTEP_DEV_IN_1)
        usbd_event(USBD_EP1, USB_DEVICE_EVENT_EP_WRITE_ACK);
    if (endp_istat & USB_INTEP_DEV_IN_2)
        usbd_event(USBD_EP2, USB_DEVICE_EVENT_EP_WRITE_ACK);
    endp_istat &= ~USB_INTEP_DEV_IN;
}

static inline void ti_arm_mcu_pin_mode_usb(int pin)
{
    ti_arm_mcu_periph_enable(ti_arm_mcu_gpio_periph(pin));
    MAP_GPIOPinTypeUSBAnalog(ti_arm_mcu_gpio_base(pin), GPIO_BIT(pin));
}

void ti_arm_mcu_usb_connect(void)
{
    MAP_USBDevConnect(USB0_BASE);
}

int ti_arm_mcu_usb_init(void)
{
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
    ti_arm_mcu_periph_enable(SYSCTL_PERIPH_USB0);
    ti_arm_mcu_pin_mode_usb(ti_arm_mcu_usbd_params.dp_pin);
    ti_arm_mcu_pin_mode_usb(ti_arm_mcu_usbd_params.dm_pin);

    MAP_SysCtlUSBPLLEnable();

    TI_BSP_IFDEF(,MAP_USBClockEnable(USB0_BASE, 8, USB_CLOCK_INTERNAL));
    TI_BSP_IFDEF(,MAP_USBULPIDisable(USB0_BASE));

    MAP_USBDevMode(USB0_BASE);

    TI_BSP_IFDEF(,MAP_USBDevLPMDisable(USB0_BASE));
    TI_BSP_IFDEF(,MAP_USBDevLPMConfig(USB0_BASE, USB_DEV_LPM_NONE));

    /* Clear pending interrupts */
    MAP_USBIntStatusControl(USB0_BASE);
    MAP_USBIntStatusEndpoint(USB0_BASE);

    MAP_USBIntEnableControl(USB0_BASE, USB_INTCTRL_RESET);
    MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_0 | USB_INTEP_DEV_OUT_1 |
        USB_INTEP_DEV_OUT_2);

    MAP_IntEnable(INT_USB0);
    return 0;
}
