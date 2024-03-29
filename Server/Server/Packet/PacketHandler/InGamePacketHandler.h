#pragma once
#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NInGame
{
	void ReqInitInfo(Connection& _conn, PacketReader& _packet);
	void BeginMove(Connection& _conn, PacketReader& _packet);
	void EndMove(Connection& _conn, PacketReader& _packet);
	void MonsterAttack(Connection& _conn, PacketReader& _packet);
	void BeginMoveMonster(Connection& _conn, PacketReader& _packet);
	void Attack(Connection& _conn, PacketReader& _packet);
	void RangedAttack(Connection& _conn, PacketReader& _packet);
	void GameOver(Connection& _conn, PacketReader& _packet);
}