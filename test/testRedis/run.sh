#!/bin/bash
g++ -std=c++11 -I/home/manjie/414chatroom/include/ -o testRedis testRedis.cc /usr/lib/aarch64-linux-gnu/lib/libredis++.a /usr/lib/aarch64-linux-gnu/libhiredis.a -pthread