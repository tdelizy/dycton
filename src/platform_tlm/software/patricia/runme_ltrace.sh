#!/bin/sh
ltrace -e malloc -e free ./crc ../adpcm/data/large.pcm > output_large.txt 2> patricia_small.log
ltrace -e malloc -e free ./crc ../adpcm/data/large.pcm > output_small.txt 2> patricia_large.log