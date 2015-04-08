
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// karl@zeilhofer.co.at
	// this module costs on an ATmega328p
	// 3972 bytes in flash
	// 376 bytes in ram (with static memory allocation)
	// numbers got from this command:
	// avr-size -C -x firmware.elf



// struct so we can return it (it's 5x32bit)
typedef struct _SHA1_Digest
{
    uint8_t digest[20];
} SHA1_Digest;

// SHA1_Digest SHA1_Digest_fromStr (const char* src);
// void        SHA1_Digest_toStr   (const SHA1_Digest* digest, char* dst);
SHA1_Digest SHA1_get (const void* msg, uint64_t bytes);


#ifdef __cplusplus
}
#endif