#include "LoginPacketHandler.h"
#include "../../User/UserManager.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"

// 패킷 지연 시간 테스트
void NLogin::Test(Connection& _conn, PacketReader& _packet)
{
	int64 ticks = _packet.GetInt64();
	Packet pkt;
	pkt.Add<PacketType>((PacketType)eServer::Test);
	//Sleep(20);
	_conn.Send(pkt);
}

void NLogin::LoginReq(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pNickname = _packet.GetWString();

	uint8 ipBytes[4] = { 0 };
	ipBytes[0] = _packet.GetUInt8();
	ipBytes[1] = _packet.GetUInt8();
	ipBytes[2] = _packet.GetUInt8();
	ipBytes[3] = _packet.GetUInt8();

	User* pUser = UserManager::GetInst()->Create(pNickname);
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	PacketType type;
	Packet p;

	if(pUser->IsLogin())
	{
		type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
		p.Add<PacketType>(type);
	}
	else // 해당아이디로 누군가 로그인
	{
		if (pLobby->GetUserCount() >= USER_LOBBY_MAX)
		{
			type = (PacketType)eServer::LoginFailure_Full;
			p.Add<PacketType>(type);
		}
		else
		{
			type = (PacketType)eServer::LoginSuccess;
			p.Add<PacketType>(type);

			_conn.SetPrivateIP(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
			UserManager::GetInst()->Connect(_conn.GetId(), pNickname);
			pLobby->Enter(_conn, pUser);
		}
	}

	wprintf(L"%s님 [%d] : ", pNickname, (int32)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	_conn.Send(p);
}