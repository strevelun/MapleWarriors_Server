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

#define BUFFER_MAX					4096

#define PACKET_MAX_SIZE				512
#define PACKET_HEADER_SIZE			4

#define NICKNAME_LEN				11 // 10 + 1
#define ROOMTITLE_LEN				16 // 15 + 1

#define USER_NOT_CONNECTED			0

#define TEST_LOCK_THREADS_MAX		2

#define USER_LOBBY_MAX				300
#define ROOM_MAX					300
#define ROOM_USER_MAX				4

#define LOBBY_USERLIST_PAGE			10
#define LOBBY_ROOMLIST_PAGE			4

#define USER_NOT_IN_THE_ROOM		INT_MAX
#define ROOM_ID_NOT_FOUND			INT_MAX

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

enum class eEnterRoomResult
{
	None,
	Success,
	Full,
	InGame,
	NoRoom // 갱신되기 전 없어진 방을 클릭했을 때
};

enum class eGameMap 
{
	None,
	Test,
};