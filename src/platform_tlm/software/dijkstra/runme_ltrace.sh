#!/bin/sh
ltrace -e malloc -e free ./dijkstra_small input.dat > output_small.dat > dijkstra_small.log 2>&1
ltrace -e malloc -e free ./dijkstra_large input.dat > output_large.dat > dijkstra_large.log 2>&1