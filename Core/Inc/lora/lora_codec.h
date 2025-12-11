#include "lora_message_types.h"
#include <stdint.h>

/**
 * Maximum number of bytes that lora_encode() will ever output for a single
 * LoraMessage, given current definitions in lora_message_types.h
 *
 * Layout worst-case:
 *   1 byte  message_type
 *   1 byte  source
 *   1 byte  dest
 *   STREAM_SEQUENCE payload:
 *     1 byte  stream_type
 *     1 byte  stream_id
 *     2 bytes sequence_number
 *     1 byte  packet_index
 *     1 byte  packets_in_sequence
 *     1 byte  chunk_len
 *    128 bytes chunk (LORA_STREAM_MAX_CHUNK_SIZE)
 *  = 138 bytes total
 */
#define LORA_MAX_ENCODED_SIZE (3 + 1 + 1 + 2 + 1 + 1 + 1 + LORA_STREAM_MAX_CHUNK_SIZE)


/**
* Returns 1 if valid, 0 if not
*/
uint8_t check_valid_lora_message(LoraMessage *lora_message);

/**
 * Decode a LoraMessage from a byte buffer.
 *
 * @param buf      Source buffer containing encoded message.
 * @param len      Length of source buffer in bytes.
 * @param msg      Destination for decoded message.
 *
 * @return 0 on success, -1 on error (e.g., too short, malformed, or
 *         unsupported message type / payload).
 */
uint8_t lora_decode(const uint8_t *buf, size_t len, LoraMessage *msg);

/**
 * Encode a LoraMessage into a byte buffer.
 *
 * @param msg      Pointer to message to encode.
 * @param buf      Destination buffer.
 * @param buf_len  Size of destination buffer in bytes.
 *
 * @return Number of bytes written into buf on success, or 0 on error
 *         (e.g., buf_len too small or unsupported message type).
 */
size_t lora_encode(const LoraMessage *msg, uint8_t *buf, size_t buf_len);