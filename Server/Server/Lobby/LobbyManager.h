#pragma once

#include "Lobby.h"

class LobbyManager
{
private:
	Lobby m_lobby;

public:
	Lobby* GetLobby() { return &m_lobby; }

public:

#pragma region Singleton
private:
	static LobbyManager* s_pInst;

public:
	static LobbyManager* GetInst()
	{
		if (!s_pInst)
			s_pInst = new LobbyManager;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	LobbyManager();
	~LobbyManager();
#pragma endregion 
};

