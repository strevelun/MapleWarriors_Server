#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <set>

#define BUFFER_MAX					1024

#define PACKET_MAX_SIZE				512
#define PACKET_HEADER_SIZE			4

#define NICKNAME_LEN				11

#define USER_NOT_CONNECTED			0

#define TEST_LOCK_THREADS_MAX		2

#define USER_LOBBY_MAX				300
#define ROOM_USER_MAX				4

#define LOBBY_USERLIST_PAGE			10

enum class eLoginState
{
	Logout,
	Login
};

enum class eSceneState
{
	None,
	Login,
	Lobby,
	Room,
	InGame,
};