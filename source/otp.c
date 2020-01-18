#include <ogc/machine/processor.h>
#include <unistd.h>
#include <string.h>

#include "otp.h"

#define HW_OTP_COMMAND  (*(vu32*)0xCD8001EC)
#define HW_OTP_DATA     (*(vu32*)0xCD8001F0)

#define HW_OTP_BLK_SIZE 4
#define HW_OTP_BLK_CNT  (OTP_SIZE / HW_OTP_BLK_SIZE)

u8 otp_read(void *dst, u8 offset, u8 size)
{
    if (!dst || offset >= OTP_SIZE || !size || (offset + size) > OTP_SIZE) return 0;
    
    u8 i, cur_offset = 0;
    
    u8 *ptr = (u8*)dst;
    u8 val[HW_OTP_BLK_SIZE];
    
    u8 start_addr = (offset / HW_OTP_BLK_SIZE);
    u8 start_addr_offset = (offset % HW_OTP_BLK_SIZE);
    
    u8 end_addr = ((offset + size) / HW_OTP_BLK_SIZE);
    u8 end_addr_size = ((offset + size) % HW_OTP_BLK_SIZE);
    
    if (!end_addr_size)
    {
        end_addr--;
        end_addr_size = HW_OTP_BLK_SIZE;
    }
    
    if (end_addr == start_addr) end_addr_size -= start_addr_offset;
    
    for(i = start_addr; i <= end_addr; i++)
    {
        if (cur_offset >= size) break;
        
        HW_OTP_COMMAND = (0x80000000 | i);
        *((u32*)val) = HW_OTP_DATA;
        
        if (start_addr == end_addr)
        {
            memcpy(ptr + cur_offset, val + start_addr_offset, end_addr_size);
            cur_offset += end_addr_size;
        } else {
            if (i == start_addr)
            {
                memcpy(ptr + cur_offset, val + start_addr_offset, HW_OTP_BLK_SIZE - start_addr_offset);
                cur_offset += (HW_OTP_BLK_SIZE - start_addr_offset);
            } else
            if (i == end_addr)
            {
                memcpy(ptr + cur_offset, val, end_addr_size);
                cur_offset += end_addr_size;
            } else {
                memcpy(ptr + cur_offset, val, HW_OTP_BLK_SIZE);
                cur_offset += HW_OTP_BLK_SIZE;
            }
        }
    }
    
    return cur_offset;
}
