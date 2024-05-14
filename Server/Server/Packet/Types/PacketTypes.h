#pragma once

#include "../../Types.h"

//LS2C_ROOM_ENTER_ANS
//C2LS_ROOM_CREATE

enum class eServer : PacketType
{
	None = 0,
	Test,
	LoginFailure_AlreadyLoggedIn,
	LoginFailure_Full,
	LoginSuccess,
	LobbyChat,
	LobbyUpdateInfo_UserList,
	LobbyUpdateInfo_RoomList,
	CreateRoom_Success,
	CreateRoom_Fail,
	EnterRoom_Success,
	EnterRoom_Full,
	EnterRoom_InGame,
	EnterRoom_NoRoom,
	ExitRoom,
	StartGame_Success,
	StartGame_Fail,
	NotifyRoomUserEnter,
	NotifyRoomUserExit,
	RoomChat,
	RoomUsersInfo,
	RoomReady,
	RoomReady_Fail,
	RoomStandby,
	RoomStandby_Fail,
	RoomMapChoice,
	RoomCharacterChoice,
	ResInitInfo,
	BeginMove,
	Moving,
	EndMove,
	MonsterAttack,
	BeginMoveMonster,
	InGameExit,
	Attack,
	AttackReq,
	RangedAttack,
	RangedAttackReq,
	GameOver,
	Awake,
	AllCreaturesInfo,
	Ready,
	Start,
	NextStage,
	MapClear,
	StageClear,
	Annihilated,
	PlayerHit,
	Max,
};

enum class eClient : PacketType
{
	None = 0,
	Test,
	LoginReq,
	LobbyChat,
	LobbyUpdateInfo,
	UserListGetPageInfo,
	RoomListGetPageInfo,
	CreateRoom,
	EnterRoom,
	ExitRoom,
	StartGame,
	RoomReady,
	RoomStandby,
	RoomChat,
	ReqRoomUsersInfo,
	RoomMapChoice,
	RoomCharacterChoice,
	ReqInitInfo,
	BeginMove,
	EndMove,
	MonsterAttack,
	BeginMoveMonster,
	InGameExit,
	Attack,
	RangedAttack,
	GameOver,
	Max,
};