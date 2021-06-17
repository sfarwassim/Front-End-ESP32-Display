#ifndef _PuraLib_h
#define _PuraLib_h
typedef struct
{
    uint16_t blk1;
    uint16_t blk2;
    uint16_t blk3;
    uint16_t blk4;
    char message[40];
    char verify_d[18];
    char verify_e[18];
} elm_data_t;

extern char rx_buffer[2000];
extern char rx_buffer10[2000];
extern char selectedDropDownZone[64];

#endif
