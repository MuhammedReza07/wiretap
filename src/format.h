/* format.h - various formatting utility functions */

#include <stdint.h>
#include <time.h>

#ifndef FORMAT_H
#define FORMAT_H

// Defined to fit "ICMPv6" plus the trailing null byte.
#define IP_PROTOSTRLEN 7
#define MAC_ADDRSTRLEN 18
#define ISO_8601_TIMESTRLEN 28
#define ISO_8601_MS_OFFSET 19

/* Convert a MAC address given as a 48-bit (6 byte) value to a string
   of length MAC_ADDRSTRLEN stored at dest.*/
void mac_ntop(uint8_t* src, uint8_t* dest);

/* Convert UNIX epoch to an ISO 8601 formatted timestamp with microseconds.*/
void time_to_iso8601(struct timespec* time, uint8_t* buffer);

/* Convert select IP protocol numbers to a string such as "TCP".
   Yields "???" for unimplemented protocols. Returns 0 if the
   protocol has been implemented, -1 otherwise.
   
   Note that dest should have a capacity of at least
   IP_PROTOSTRLEN bytes.*/
int8_t ip_proto_to_string(uint8_t ip_proto, uint8_t* dest);

#endif