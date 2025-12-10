#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t NodeId;

#define LORA_NODE_BROADCAST_ID ((NodeId)0xFF)
#define LORA_STREAM_MAX_CHUNK_SIZE 128
#define LORA_STREAM_MAX_PACKETS_PER_SEQ 32

/**
    Message Type:
        PING
*/
typedef struct {
    uint8_t _reserved; 
} LoraPingReq;
typedef struct {
    uint8_t _reserved; 
} LoraPingResp;

/**
    Message Type:
        DATA
 */
typedef struct {
    int16_t temperature_tenths;
    int16_t humidity_tenths;
} ClimateData;

typedef enum  {
    LORA_DATA_TYPE_CLIMATE = 1,
} LoraDataType;

typedef union {
    ClimateData climate_data;
    // will add more data types later, so keep the union here
} LoraDataPayload;

typedef struct {
    LoraDataType data_type;
} LoraDataReq;

typedef struct {
    LoraDataType data_type;
    LoraDataPayload payload;
} LoraData;

/**
    Message Type:
        COMMAND
*/
typedef enum {
    LORA_COMMAND_TRIGGER,
    LORA_COMMAND_SET_VALUE
} CommandType;

typedef enum {
    LORA_COMMAND_ERROR = 0,
    LORA_COMMAND_SUCCESS = 1
} CommandStatus;

typedef union {
    uint8_t value;
} CommandValue;

typedef struct {
    CommandType command_type;
    CommandValue command_value;
} LoraCommandReq;

typedef struct {
    CommandType command_type;
    CommandStatus command_status;
} LoraCommandResp;

/**
    Message Type:
        STREAM (sequence transfer)

        // Think of a proper state machine here for an asynchronous stream
            - 1 stream has n sequences
            - ACK on a sequence
*/
typedef enum {
    LORA_STREAM_JPEG,
    LORA_STREAM_RAW
} LoraStreamType;

// Client sends this to request a stream of LoraStreamType
typedef struct {
    LoraStreamType stream_type;
} LoraStreamRequest;

// Sent either arbitrarily as a broadcast or as a response to a LoraStreamRequest
typedef struct {
    LoraStreamType stream_type;
    uint8_t stream_id;
    uint16_t sequence_number; // announce a sequence number
    uint8_t packets_in_sequence;
} LoraStreamAnnounce;

typedef struct {
    uint8_t stream_id;
    uint16_t sequence_number; // May be an undefined number of sequences at this point.
} LoraStreamAnnounceAck;

// The data is contained in these messages. 
// Each packet represents a series of bytes indexed in stream_id via sequence_number and packet_index.
typedef struct {
    LoraStreamType stream_type;
    uint8_t stream_id;
    uint16_t sequence_number;     // which sequence this packet belongs to
    uint8_t packet_index;    // 0 .. packets_insequence - 1
    uint8_t packets_in_sequence;
    uint8_t  chunk_len;       // number of valid bytes in chunk[]
    uint8_t  chunk[LORA_STREAM_MAX_CHUNK_SIZE];
} LoraStreamSequence;

typedef enum {
    LORA_STREAM_STATUS_OK = 0,
    LORA_STREAM_STATUS_ERROR = 1,
} LoraStreamStatus;

typedef struct {
    uint8_t stream_id;
    uint16_t sequence_number;
    LoraStreamStatus status;
    uint32_t missing_bitmap; // bit i = 1 means packet i is missing
} LoraStreamSequenceAck;

// sent by sender when entire stream contents has been sent
typedef struct {
    uint8_t stream_id;
} LoraStreamComplete;

/**
    LoraMessage
        - Message Type 
        - Message Metadata (ie source, dest)
        - Message Payload (ie some request or response data)
 */
typedef enum {
    LORA_RAW = 0,

    LORA_PING_REQUEST = 1,
    LORA_PING_RESPONSE = 2,

    LORA_DATA_REQUEST = 3,
    LORA_DATA = 4, // eg sensor data

    LORA_COMMAND_REQUEST = 5, // eg close chicken coop door
    LORA_COMMAND_RESPONSE = 6,

    LORA_STREAM_REQUEST = 7,
    LORA_STREAM_ANNOUNCE = 8,
    LORA_STREAM_ANNOUNCE_ACK = 9,
    LORA_STREAM_SEQUENCE = 10,
    LORA_STREAM_SEQUENCE_ACK = 11,
    LORA_STREAM_COMPLETE = 12,
} LoraMessageType;

typedef struct {
    NodeId source;
    NodeId dest;
} LoraMetadata;

 typedef union {
    uint8_t         raw[LORA_STREAM_MAX_CHUNK_SIZE];

    LoraPingReq     ping_req;
    LoraPingResp    ping_resp;

    LoraDataReq     data_req;
    LoraData    data;

    LoraCommandReq  command_req;
    LoraCommandResp command_resp;
    
    LoraStreamRequest stream_req;
    LoraStreamAnnounce stream_announce;
    LoraStreamAnnounceAck stream_announce_ack;
    LoraStreamSequence stream_sequence;
    LoraStreamSequenceAck stream_seq_ack;
    LoraStreamComplete stream_complete;
} LoraPayload;

typedef struct {
    LoraMessageType message_type;
    LoraMetadata metadata;
    LoraPayload payload;
} LoraMessage;