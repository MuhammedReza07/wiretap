/* Tell the compiler to include various definitions that are needed by the program
   like round() and strptime(). Probably makes the program GNU/Linux specific,
   so sorry Windows users... (Not really :3 )*/
#define _GNU_SOURCE 39

#include "format.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define UINT64_STR_BUFFER_LEN 65

void mac_ntop(uint8_t* src, uint8_t* dest) {
    size_t i, j;
    uint8_t hi, lo;

    i = j = 0;
    while (j < MAC_ADDRSTRLEN) {
        hi = (*(src + i) & 0xf0) >> 4;
        lo = *(src + i) & 0xf;

        // Convert hi and lo to ASCII characters.
        *(dest + j) = hi + (hi < 10 ? 0x30 : 0x57);
        *(dest + j + 1) = lo + (lo < 10 ? 0x30 : 0x57);
        *(dest + j + 2) = '-';

        i += 1;
        j += 3;
    }

    *(dest + MAC_ADDRSTRLEN - 1) = '\0';
}

void time_to_iso8601(struct timespec* time, uint8_t* buffer) {
    uint32_t milliseconds;
    double nanoseconds;

    struct tm broken_down_time;
    memset(&broken_down_time, 0, sizeof(broken_down_time));

    /* This buffer should be large enough for storing the
       decimal representation of a 64-bit integer,
       together with the null-byte with bytes left to spare :) */
    uint8_t uint64_str_buffer[UINT64_STR_BUFFER_LEN];

    // Why do I have to do this? :/
    snprintf((char*)uint64_str_buffer, UINT64_STR_BUFFER_LEN, "%ld", time->tv_sec);
    strptime((char*)uint64_str_buffer, "%s", &broken_down_time);

    // Convert nanoseconds to milliseconds.
    nanoseconds = (double)(time->tv_nsec);
    nanoseconds /= 1e6;
    nanoseconds = round(nanoseconds);
    milliseconds = (uint32_t)nanoseconds;

    // Finally, we are two steps away from the ISO 8601 formatted timestamp!
    strftime((char*)buffer, ISO_8601_TIMESTRLEN, "%Y-%m-%dT%H:%M:%S", &broken_down_time);
    snprintf((char*)(buffer + ISO_8601_MS_OFFSET), ISO_8601_TIMESTRLEN - ISO_8601_MS_OFFSET, ".%03dZ", milliseconds);
}