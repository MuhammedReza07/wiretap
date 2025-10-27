/* format.h - various formatting utility functions */

#include <stdint.h>
#include <time.h>

#ifndef FORMAT_H
#define FORMAT_H

#define MAC_ADDRSTRLEN 18
#define ISO_8601_TIMESTRLEN 28
#define ISO_8601_MS_OFFSET 19

/* Convert a MAC address given as a 48-bit (6 byte) value to a string
   of length MAC_ADDRSTRLEN stored at dest.*/
void mac_ntop(uint8_t* src, uint8_t* dest);

/* Convert UNIX epoch to an ISO 8601 formatted timestamp with microseconds.*/
void time_to_iso8601(struct timespec* time, uint8_t* buffer);

#endif