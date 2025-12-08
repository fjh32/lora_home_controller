#include <stdint.h>
#include <stddef.h>

typedef uint8_t NodeId;

#define LORA_NODE_BROADCAST_ID ((NodeId)0xFF)
#define LORA_STREAM_MAX_CHUNK_SIZE 48

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
} LoraDataPayload;

typedef struct {
    LoraDataType data_type;
} LoraDataReq;

typedef struct {
    LoraDataType data_type;
    LoraDataPayload payload;
} LoraDataResp;

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
    CommandStatus command_status;
} LoraCommandResp;

/**
    Message Type:
        STREAM (sequence transfer)
*/
typedef struct {
    uint8_t _reserved;
} LoraStreamReq;

typedef struct {
    uint16_t sequence_id;
    uint16_t total_packets;
    uint32_t total_bytes;
} LoraStreamAnnounce;

typedef struct {
    uint16_t sequence_id;     // which sequence this packet belongs to
    uint16_t packet_index;    // 0 .. total_packets-1
    uint8_t  chunk_len;       // number of valid bytes in chunk[]
    uint8_t  chunk[LORA_STREAM_MAX_CHUNK_SIZE];
} LoraStreamData;

typedef enum {
    LORA_STREAM_STATUS_OK = 0,
    LORA_STREAM_STATUS_ERROR = 1,
} LoraStreamStatus;

typedef struct {
    uint16_t sequence_id;
    LoraStreamStatus status;
    // bitmask of missing packets?
} LoraStreamSequenceAck;


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
    LORA_DATA_RESPONSE = 4, // eg request sensor data

    LORA_COMMAND_REQUEST = 5, // eg close chicken coop door
    LORA_COMMAND_RESPONSE = 6,

    LORA_STREAM_REQUEST = 7,
    LORA_STREAM_ANNOUNCE_RESPONSE = 8,
    LORA_STREAM_DATA = 9,
    LORA_STREAM_SEQUENCE_ACK = 10,
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
    LoraDataResp    data_resp;

    LoraCommandReq  command_req;
    LoraCommandResp command_resp;
    
    LoraStreamRequest stream_req;
    LoraStreamAnnounceResp stream_announce_resp;
    LoraStreamDataResp stream_data_resp;
    LoraStreamSequenceAck stream_seq_ack;
} LoraPayload;

typedef struct {
    LoraMessageType message_type;
    LoraMetadata metadata;
    LoraPayload payload;
} LoraMessage;