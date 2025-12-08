#include "lora_message_types.h"

/**
* Returns 1 if valid, 0 if not
*/
int check_valid_lora_message(LoraMessage *lora_message);

/**
* Decode a byte buffer into a LoraMessage
* Returns -1 on error
*/
int lora_decode_message(const uint8_t *decode_buffer, size_t len, LoraMessage *decoded_msg);

/**
* Encode a LoraMessage struct into a byte buffer
* Returns -1 on error
*/
int lora_encode_message(const LoraMessage *msg, uint8_t *encode_buffer, size_t len);