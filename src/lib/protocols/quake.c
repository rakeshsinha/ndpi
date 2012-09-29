/*
 * quake.c
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


#include "ndpi_utils.h"
#ifdef NDPI_PROTOCOL_QUAKE

static void ndpi_int_quake_add_connection(struct ndpi_detection_module_struct
											*ndpi_struct)
{
	ndpi_int_add_connection(ndpi_struct, NDPI_PROTOCOL_QUAKE, NDPI_REAL_PROTOCOL);
}

void ndpi_search_quake(struct ndpi_detection_module_struct *ndpi_struct)
{
	struct ndpi_packet_struct *packet = &ndpi_struct->packet;
	struct ndpi_flow_struct *flow = ndpi_struct->flow;
//      struct ndpi_id_struct         *src=ndpi_struct->src;
//      struct ndpi_id_struct         *dst=ndpi_struct->dst;

	if ((packet->payload_packet_len == 14
		 && get_u16(packet->payload, 0) == 0xffff && ndpi_mem_cmp(&packet->payload[2], "getInfo", 7) == 0)
		|| (packet->payload_packet_len == 17
			&& get_u16(packet->payload, 0) == 0xffff && ndpi_mem_cmp(&packet->payload[2], "challenge", 9) == 0)
		|| (packet->payload_packet_len > 20
			&& packet->payload_packet_len < 30
			&& get_u16(packet->payload, 0) == 0xffff && ndpi_mem_cmp(&packet->payload[2], "getServers", 10) == 0)) {
		NDPI_LOG(NDPI_PROTOCOL_QUAKE, ndpi_struct, NDPI_LOG_DEBUG, "Quake IV detected.\n");
		ndpi_int_quake_add_connection(ndpi_struct);
		return;
	}

	/* Quake III/Quake Live */
	if (packet->payload_packet_len == 15 && get_u32(packet->payload, 0) == 0xffffffff
		&& memcmp(&packet->payload[4], "getinfo", NDPI_STATICSTRING_LEN("getinfo")) == 0) {
		NDPI_LOG(NDPI_PROTOCOL_QUAKE, ndpi_struct, NDPI_LOG_DEBUG, "Quake III Arena/Quake Live detected.\n");
		ndpi_int_quake_add_connection(ndpi_struct);
		return;
	}
	if (packet->payload_packet_len == 16 && get_u32(packet->payload, 0) == 0xffffffff
		&& memcmp(&packet->payload[4], "getchallenge", NDPI_STATICSTRING_LEN("getchallenge")) == 0) {
		NDPI_LOG(NDPI_PROTOCOL_QUAKE, ndpi_struct, NDPI_LOG_DEBUG, "Quake III Arena/Quake Live detected.\n");
		ndpi_int_quake_add_connection(ndpi_struct);
		return;
	}
	if (packet->payload_packet_len > 20 && packet->payload_packet_len < 30
		&& get_u32(packet->payload, 0) == 0xffffffff
		&& memcmp(&packet->payload[4], "getservers", NDPI_STATICSTRING_LEN("getservers")) == 0) {
		NDPI_LOG(NDPI_PROTOCOL_QUAKE, ndpi_struct, NDPI_LOG_DEBUG, "Quake III Arena/Quake Live detected.\n");
		ndpi_int_quake_add_connection(ndpi_struct);
		return;
	}



	/* ports for startup packet:
	   Quake I        26000 (starts with 0x8000)
	   Quake II       27910
	   Quake III      27960 (increases with each player)
	   Quake IV       27650
	   Quake World    27500
	   Quake Wars     ?????
	 */

	NDPI_LOG(NDPI_PROTOCOL_QUAKE, ndpi_struct, NDPI_LOG_DEBUG, "Quake excluded.\n");
	NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_QUAKE);
}

#endif
