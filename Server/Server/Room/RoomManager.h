#pragma once

//#include "Room.h"

class RoomManager
{
private:
	//std::vector<Room*> m_vecRoom;

public:


#pragma region Singleton
private:
	static RoomManager* s_pInst;

public:
	static RoomManager* GetInst()
	{
		if (!s_pInst)
			s_pInst = new RoomManager;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	RoomManager();
	~RoomManager();
#pragma endregion 
};

