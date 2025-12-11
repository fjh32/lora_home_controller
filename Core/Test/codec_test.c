#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "lora_message_types.h"
#include "lora_codec.h"

#define TEST_STREAM_SIZE 1028

static int test_ping()
{
    LoraMessage msg = {0};
    msg.message_type = LORA_PING_REQUEST;
    msg.metadata.source = 1;
    msg.metadata.dest   = 2;

    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t encoded = lora_encode(&msg, buf, sizeof(buf));

    if (encoded == 0) {
        printf("PING encode FAILED\n");
        return -1;
    }

    LoraMessage decoded = {0};
    if (lora_decode(buf, encoded, &decoded) != 0) {
        printf("PING decode FAILED\n");
        return -1;
    }

    if (decoded.message_type != LORA_PING_REQUEST ||
        decoded.metadata.source != 1 ||
        decoded.metadata.dest != 2) {

        printf("PING test MISMATCH\n");
        return -1;
    }

    printf("PING test PASSED\n");
    return 0;
}


static int test_data()
{
    LoraMessage msg = {0};
    msg.message_type = LORA_DATA;
    msg.metadata.source = 5;
    msg.metadata.dest   = 6;

    msg.payload.data.data_type = LORA_DATA_TYPE_CLIMATE;
    msg.payload.data.payload.climate_data.temperature_tenths = 253;  // 25.3°C
    msg.payload.data.payload.climate_data.humidity_tenths = 512;    // 51.2%

    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t encoded = lora_encode(&msg, buf, sizeof(buf));

    if (encoded == 0) {
        printf("DATA encode FAILED\n");
        return -1;
    }

    LoraMessage decoded = {0};
    if (lora_decode(buf, encoded, &decoded) != 0) {
        printf("DATA decode FAILED\n");
        return -1;
    }

    if (decoded.payload.data.payload.climate_data.temperature_tenths != 253 ||
        decoded.payload.data.payload.climate_data.humidity_tenths != 512) {

        printf("DATA test MISMATCH\n");
        return -1;
    }

    printf("DATA test PASSED\n");
    return 0;
}


static int test_command()
{
    LoraMessage msg = {0};
    msg.message_type = LORA_COMMAND_REQUEST;
    msg.metadata.source = 3;
    msg.metadata.dest   = 99;

    msg.payload.command_req.command_type = LORA_COMMAND_SET_VALUE;
    msg.payload.command_req.command_value.value = 42;

    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t encoded = lora_encode(&msg, buf, sizeof(buf));

    if (encoded == 0) {
        printf("COMMAND encode FAILED\n");
        return -1;
    }

    LoraMessage decoded = {0};
    if (lora_decode(buf, encoded, &decoded) != 0) {
        printf("COMMAND decode FAILED\n");
        return -1;
    }

    if (decoded.payload.command_req.command_type != LORA_COMMAND_SET_VALUE ||
        decoded.payload.command_req.command_value.value != 42) {

        printf("COMMAND test MISMATCH\n");
        return -1;
    }

    printf("COMMAND test PASSED\n");
    return 0;
}


static int test_stream()
{
    printf("STREAM test starting…\n");

    // Create 256 bytes of repeating A..Z letters
    uint8_t big_data[TEST_STREAM_SIZE];
    for (int i = 0; i < TEST_STREAM_SIZE; ++i) {
        big_data[i] = 'A' + (i % 26);
    }

    const int packets = TEST_STREAM_SIZE / LORA_STREAM_MAX_CHUNK_SIZE
                        + ((TEST_STREAM_SIZE % LORA_STREAM_MAX_CHUNK_SIZE) ? 1 : 0);

    uint16_t seq = 12;  // arbitrary test sequence number
    uint8_t stream_id = 7;

    for (int p = 0; p < packets; ++p) {

        LoraMessage msg = {0};
        msg.message_type = LORA_STREAM_SEQUENCE;
        msg.metadata.source = 8;
        msg.metadata.dest   = 9;

        LoraStreamSequence *ss = &msg.payload.stream_sequence;
        ss->stream_type = LORA_STREAM_JPEG;
        ss->stream_id = stream_id;
        ss->sequence_number = seq;
        ss->packet_index = p;
        ss->packets_in_sequence = packets;

        // Fill chunk
        size_t offset = p * LORA_STREAM_MAX_CHUNK_SIZE;
        size_t remaining = TEST_STREAM_SIZE - offset;
        if (remaining > LORA_STREAM_MAX_CHUNK_SIZE)
            remaining = LORA_STREAM_MAX_CHUNK_SIZE;

        ss->chunk_len = remaining;
        memcpy(ss->chunk, &big_data[offset], remaining);

        // Encode
        uint8_t buf[LORA_MAX_ENCODED_SIZE];
        size_t encoded = lora_encode(&msg, buf, sizeof(buf));
        if (encoded == 0) {
            printf("STREAM encode FAILED on packet %d\n", p);
            return -1;
        }

        // Decode
        LoraMessage decoded = {0};
        if (lora_decode(buf, encoded, &decoded) != 0) {
            printf("STREAM decode FAILED on packet %d\n", p);
            return -1;
        }

        // Validate fields
        LoraStreamSequence *ds = &decoded.payload.stream_sequence;

        if (ds->stream_id != stream_id ||
            ds->sequence_number != seq ||
            ds->packet_index != p ||
            ds->packets_in_sequence != packets ||
            ds->chunk_len != remaining) {

            printf("STREAM header mismatch on packet %d\n", p);
            return -1;
        }

        if (memcmp(ds->chunk, &big_data[offset], remaining) != 0) {
            printf("STREAM chunk mismatch on packet %d\n", p);
            return -1;
        }
    }

    printf("STREAM test PASSED with %d packets\n", packets);
    return 0;
}



int main(void)
{
    int failures = 0;

    failures += test_ping();
    failures += test_data();
    failures += test_command();
    failures += test_stream();

    if (failures == 0) {
        printf("\nALL TESTS PASSED!\n");
    } else {
        printf("\nTESTS FAILED: %d failures\n", failures);
    }

    return failures;
}
