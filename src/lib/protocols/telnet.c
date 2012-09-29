/*
 * telnet.c
 * Copyright (C) 2009-2011 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#include "ndpi_protocols.h"
#ifdef NDPI_PROTOCOL_TELNET



static void ndpi_int_telnet_add_connection(struct ndpi_detection_module_struct
											 *ndpi_struct)
{
	ndpi_int_add_connection(ndpi_struct, NDPI_PROTOCOL_TELNET, NDPI_REAL_PROTOCOL);
}

	
#if !(defined(HAVE_NTOP) && defined(WIN32))
 static inline
#else
__forceinline static
#endif
	 u8 search_iac(struct ndpi_detection_module_struct *ndpi_struct)
{
	struct ndpi_packet_struct *packet = &ndpi_struct->packet;

	u16 a;

	if (packet->payload_packet_len < 3) {
		return 0;
	}

	if (!(packet->payload[0] == 0xff
		  && packet->payload[1] > 0xf9 && packet->payload[1] != 0xff && packet->payload[2] < 0x28)) {
		return 0;
	}

	a = 3;

	while (a < packet->payload_packet_len - 2) {
		// commands start with a 0xff byte followed by a command byte >= 0xf0 and < 0xff
		// command bytes 0xfb to 0xfe are followed by an option byte <= 0x28
		if (!(packet->payload[a] != 0xff ||
			  (packet->payload[a] == 0xff && (packet->payload[a + 1] >= 0xf0) && (packet->payload[a + 1] <= 0xfa)) ||
			  (packet->payload[a] == 0xff && (packet->payload[a + 1] >= 0xfb) && (packet->payload[a + 1] != 0xff)
			   && (packet->payload[a + 2] <= 0x28)))) {
			return 0;
		}
		a++;
	}

	return 1;
}

/* this detection also works asymmetrically */
void ndpi_search_telnet_tcp(struct ndpi_detection_module_struct
							  *ndpi_struct)
{
//  struct ndpi_packet_struct *packet = &ndpi_struct->packet;
	struct ndpi_flow_struct *flow = ndpi_struct->flow;
//      struct ndpi_id_struct         *src=ndpi_struct->src;
//      struct ndpi_id_struct         *dst=ndpi_struct->dst;

	NDPI_LOG(NDPI_PROTOCOL_TELNET, ndpi_struct, NDPI_LOG_DEBUG, "search telnet.\n");

	if (search_iac(ndpi_struct) == 1) {

		if (flow->l4.tcp.telnet_stage == 2) {
			NDPI_LOG(NDPI_PROTOCOL_TELNET, ndpi_struct, NDPI_LOG_DEBUG, "telnet identified.\n");
			ndpi_int_telnet_add_connection(ndpi_struct);
			return;
		}
		flow->l4.tcp.telnet_stage++;
		NDPI_LOG(NDPI_PROTOCOL_TELNET, ndpi_struct, NDPI_LOG_DEBUG, "telnet stage %u.\n", flow->l4.tcp.telnet_stage);
		return;
	}

	if ((flow->packet_counter < 12 && flow->l4.tcp.telnet_stage > 0) || flow->packet_counter < 6) {
		return;
	} else {
		NDPI_LOG(NDPI_PROTOCOL_TELNET, ndpi_struct, NDPI_LOG_DEBUG, "telnet excluded.\n");
		NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_TELNET);
	}
	return;
}

#endif
