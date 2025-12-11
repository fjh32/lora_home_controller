#!/bin/bash
gcc -I../Inc/lora ../Src/lora/lora_message_types.c ../Src/lora/lora_codec.c codec_test.c -o codec_test && ./codec_test
