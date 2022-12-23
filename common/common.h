#pragma once
#define MAX_CLIENT_COUNT 5

struct message
{
    uint8_t len_h; 
    uint8_t len_l; 
    uint8_t sum_h; 
    uint8_t sum_l; 
    uint8_t data[0];
};
