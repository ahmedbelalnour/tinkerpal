MK_OBJS+=net.o netif.o net_utils.o
MK_OBJS+=$(if $(CONFIG_JS),js_netif.o)
MK_OBJS+=$(if $(CONFIG_PACKET),packet.o)
MK_OBJS+=$(if $(CONFIG_ETHERIF),etherif.o)
MK_OBJS+=$(if $(CONFIG_ETHERNET),ether.o)
MK_OBJS+=$(if $(CONFIG_ARP),arp.o)
MK_OBJS+=$(if $(CONFIG_IPV4),ipv4.o)
MK_OBJS+=$(if $(CONFIG_ICMP),icmp.o)
MK_OBJS+=$(if $(CONFIG_UDP),udp.o)
MK_OBJS+=$(if $(CONFIG_DHCP_CLIENT),dhcpc.o)
MK_OBJS+=$(if $(CONFIG_NET_DEBUG),net_debug.o)

MK_DESCS=netif.desc
