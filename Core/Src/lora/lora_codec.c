#include "lora_codec.h"
#include <string.h> // memcpy
#include <stdbool.h>

// Little-endian helpers
static void write_u16_le(uint8_t *dst, uint16_t v)
{
    dst[0] = (uint8_t)(v & 0xFF);
    dst[1] = (uint8_t)((v >> 8) & 0xFF);
}

static uint16_t read_u16_le(const uint8_t *src)
{
    return (uint16_t)(src[0] | ((uint16_t)src[1] << 8));
}

static void write_u32_le(uint8_t *dst, uint32_t v)
{
    dst[0] = (uint8_t)(v & 0xFF);
    dst[1] = (uint8_t)((v >> 8) & 0xFF);
    dst[2] = (uint8_t)((v >> 16) & 0xFF);
    dst[3] = (uint8_t)((v >> 24) & 0xFF);
}

static uint32_t read_u32_le(const uint8_t *src)
{
    return (uint32_t)(
          ((uint32_t)src[0])
        | ((uint32_t)src[1] << 8)
        | ((uint32_t)src[2] << 16)
        | ((uint32_t)src[3] << 24));
}

size_t lora_encode(const LoraMessage *msg, uint8_t *buf, size_t buf_len)
{
    if (!msg || !buf || buf_len < 3) {
        return 0;
    }

    size_t pos = 0;

    // Common header: message_type + metadata
    // FIRST 3 BYTES OF PACKET
    buf[pos++] = (uint8_t)msg->message_type;
    buf[pos++] = msg->metadata.source;
    buf[pos++] = msg->metadata.dest;

    // Encode payload depending on message_type
    switch (msg->message_type) {
        case LORA_RAW:
            // Raw: copy fixed LORA_STREAM_MAX_CHUNK_SIZE bytes
            if (buf_len < pos + LORA_STREAM_MAX_CHUNK_SIZE) {
                return 0;
            }
            memcpy(&buf[pos], msg->payload.raw, LORA_STREAM_MAX_CHUNK_SIZE);
            pos += LORA_STREAM_MAX_CHUNK_SIZE;
            break;

        case LORA_PING_REQUEST:
            // No payload; we ignore _reserved and just send header
            break;

        case LORA_PING_RESPONSE:
            // No payload; ignore _reserved
            break;

        case LORA_DATA_REQUEST: {
            if (buf_len < pos + 1) {
                return 0;
            }
            buf[pos++] = (uint8_t)msg->payload.data_req.data_type;
            break;
        }

        case LORA_DATA: {
            if (buf_len < pos + 1) {
                return 0;
            }
            const LoraData *d = &msg->payload.data;
            buf[pos++] = (uint8_t)d->data_type;

            switch (d->data_type) {
        case LORA_DATA_TYPE_CLIMATE: {
            if (buf_len < pos + 4) {
                return 0;
            }
            write_u16_le(&buf[pos],
                        (uint16_t)d->payload.climate_data.temperature_tenths);
            pos += 2;
            write_u16_le(&buf[pos],
                        (uint16_t)d->payload.climate_data.humidity_tenths);
            pos += 2;
            break;
        }
        default:
            // Unknown data type for now: treat as error
            return 0;
        }
        break;
    }

    case LORA_COMMAND_REQUEST: {
        if (buf_len < pos + 2) {
            return 0;
        }
        buf[pos++] = (uint8_t)msg->payload.command_req.command_type;
        buf[pos++] = msg->payload.command_req.command_value.value;
        break;
    }

    case LORA_COMMAND_RESPONSE: {
        if (buf_len < pos + 2) {
            return 0;
        }
        buf[pos++] = (uint8_t)msg->payload.command_resp.command_type;
        buf[pos++] = (uint8_t)msg->payload.command_resp.command_status;
        break;
    }

    case LORA_STREAM_REQUEST: {
        if (buf_len < pos + 1) {
            return 0;
        }
        buf[pos++] = (uint8_t)msg->payload.stream_req.stream_type;
        break;
    }

    case LORA_STREAM_ANNOUNCE: {
        if (buf_len < pos + 1 + 1 + 2 + 1) {
            return 0;
        }
        const LoraStreamAnnounce *sa = &msg->payload.stream_announce;
        buf[pos++] = (uint8_t)sa->stream_type;
        buf[pos++] = sa->stream_id;
        write_u16_le(&buf[pos], sa->sequence_number);
        pos += 2;
        buf[pos++] = sa->packets_in_sequence;
        break;
    }

    case LORA_STREAM_ANNOUNCE_ACK: {
        if (buf_len < pos + 1 + 2) {
            return 0;
        }
        const LoraStreamAnnounceAck *aa = &msg->payload.stream_announce_ack;
        buf[pos++] = aa->stream_id;
        write_u16_le(&buf[pos], aa->sequence_number);
        pos += 2;
        break;
    }

    case LORA_STREAM_SEQUENCE: {
        const LoraStreamSequence *ss = &msg->payload.stream_sequence;
        uint8_t chunk_len = ss->chunk_len;
        if (chunk_len > LORA_STREAM_MAX_CHUNK_SIZE) {
            return 0;
        }

        if (buf_len < pos + 1 + 1 + 2 + 1 + 1 + 1 + chunk_len) {
            return 0;
        }

        buf[pos++] = (uint8_t)ss->stream_type;
        buf[pos++] = ss->stream_id;
        write_u16_le(&buf[pos], ss->sequence_number);
        pos += 2;
        buf[pos++] = ss->packet_index;
        buf[pos++] = ss->packets_in_sequence;
        buf[pos++] = ss->chunk_len;

        memcpy(&buf[pos], ss->chunk, chunk_len);
        pos += chunk_len;
        break;
    }

    case LORA_STREAM_SEQUENCE_ACK: {
        if (buf_len < pos + 1 + 2 + 1 + 4) {
            return 0;
        }
        const LoraStreamSequenceAck *ack = &msg->payload.stream_seq_ack;
        buf[pos++] = ack->stream_id;
        write_u16_le(&buf[pos], ack->sequence_number);
        pos += 2;
        buf[pos++] = (uint8_t)ack->status;
        write_u32_le(&buf[pos], ack->missing_bitmap);
        pos += 4;
        break;
    }

    case LORA_STREAM_COMPLETE: {
        if (buf_len < pos + 1) {
            return 0;
        }
        buf[pos++] = msg->payload.stream_complete.stream_id;
        break;
    }

    default:
        // Unsupported message type
        return 0;
    }

    return pos;
}

uint8_t lora_decode(const uint8_t *buf, size_t len, LoraMessage *msg)
{
    if (!buf || !msg || len < 3) {
        return -1;
    }

    size_t pos = 0;

    msg->message_type = (LoraMessageType)buf[pos++];
    msg->metadata.source = buf[pos++];
    msg->metadata.dest   = buf[pos++];

    switch (msg->message_type) {
    case LORA_RAW: {
        if (len < pos + LORA_STREAM_MAX_CHUNK_SIZE) {
            return -1;
        }
        memcpy(msg->payload.raw, &buf[pos], LORA_STREAM_MAX_CHUNK_SIZE);
        pos += LORA_STREAM_MAX_CHUNK_SIZE;
        break;
    }

    case LORA_PING_REQUEST:
        msg->payload.ping_req._reserved = 0;
        break;

    case LORA_PING_RESPONSE:
        msg->payload.ping_resp._reserved = 0;
        break;

    case LORA_DATA_REQUEST: {
        if (len < pos + 1) {
            return -1;
        }
        msg->payload.data_req.data_type = (LoraDataType)buf[pos++];
        break;
    }

    case LORA_DATA: {
        if (len < pos + 1) {
            return -1;
        }
        LoraData *d = &msg->payload.data;
        d->data_type = (LoraDataType)buf[pos++];

        switch (d->data_type) {
        case LORA_DATA_TYPE_CLIMATE: {
            if (len < pos + 4) {
                return -1;
            }
            int16_t t = (int16_t)read_u16_le(&buf[pos]); pos += 2;
            int16_t h = (int16_t)read_u16_le(&buf[pos]); pos += 2;
            d->payload.climate_data.temperature_tenths = t;
            d->payload.climate_data.humidity_tenths    = h;
            break;
        }
        default:
            // Unknown data type for now
            return -1;
        }
        break;
    }

    case LORA_COMMAND_REQUEST: {
        if (len < pos + 2) {
            return -1;
        }
        msg->payload.command_req.command_type =
            (CommandType)buf[pos++];
        msg->payload.command_req.command_value.value = buf[pos++];
        break;
    }

    case LORA_COMMAND_RESPONSE: {
        if (len < pos + 2) {
            return -1;
        }
        msg->payload.command_resp.command_type =
            (CommandType)buf[pos++];
        msg->payload.command_resp.command_status =
            (CommandStatus)buf[pos++];
        break;
    }

    case LORA_STREAM_REQUEST: {
        if (len < pos + 1) {
            return -1;
        }
        msg->payload.stream_req.stream_type =
            (LoraStreamType)buf[pos++];
        break;
    }

    case LORA_STREAM_ANNOUNCE: {
        if (len < pos + 1 + 1 + 2 + 1) {
            return -1;
        }
        LoraStreamAnnounce *sa = &msg->payload.stream_announce;
        sa->stream_type = (LoraStreamType)buf[pos++];
        sa->stream_id   = buf[pos++];
        sa->sequence_number = read_u16_le(&buf[pos]); pos += 2;
        sa->packets_in_sequence = buf[pos++];
        break;
    }

    case LORA_STREAM_ANNOUNCE_ACK: {
        if (len < pos + 1 + 2) {
            return -1;
        }
        LoraStreamAnnounceAck *aa = &msg->payload.stream_announce_ack;
        aa->stream_id = buf[pos++];
        aa->sequence_number = read_u16_le(&buf[pos]); pos += 2;
        break;
    }

    case LORA_STREAM_SEQUENCE: {
        if (len < pos + 1 + 1 + 2 + 1 + 1 + 1) {
            return -1;
        }
        LoraStreamSequence *ss = &msg->payload.stream_sequence;
        ss->stream_type       = (LoraStreamType)buf[pos++];
        ss->stream_id         = buf[pos++];
        ss->sequence_number   = read_u16_le(&buf[pos]); pos += 2;
        ss->packet_index      = buf[pos++];
        ss->packets_in_sequence = buf[pos++];
        ss->chunk_len         = buf[pos++];

        if (ss->chunk_len > LORA_STREAM_MAX_CHUNK_SIZE) {
            return -1;
        }
        if (len < pos + ss->chunk_len) {
            return -1;
        }

        memcpy(ss->chunk, &buf[pos], ss->chunk_len);
        pos += ss->chunk_len;
        break;
    }

    case LORA_STREAM_SEQUENCE_ACK: {
        if (len < pos + 1 + 2 + 1 + 4) {
            return -1;
        }
        LoraStreamSequenceAck *ack = &msg->payload.stream_seq_ack;
        ack->stream_id       = buf[pos++];
        ack->sequence_number = read_u16_le(&buf[pos]); pos += 2;
        ack->status          = (LoraStreamStatus)buf[pos++];
        ack->missing_bitmap  = read_u32_le(&buf[pos]); pos += 4;
        break;
    }

    case LORA_STREAM_COMPLETE: {
        if (len < pos + 1) {
            return -1;
        }
        msg->payload.stream_complete.stream_id = buf[pos++];
        break;
    }

    default:
        return -1;
    }

    return 0;
}

