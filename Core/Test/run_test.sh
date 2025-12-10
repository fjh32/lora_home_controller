#!/bin/bash
gcc -I../Inc/lora ../Src/lora/lora_message_types.c ../Src/lora/lora_codec.c main.c -o lora_test && ./lora_test
