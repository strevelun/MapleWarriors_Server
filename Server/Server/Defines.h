#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <unordered_map>

#define BUFFER_MAX					1024

#define PACKET_MAX_SIZE				128
#define PACKET_HEADER_SIZE			4