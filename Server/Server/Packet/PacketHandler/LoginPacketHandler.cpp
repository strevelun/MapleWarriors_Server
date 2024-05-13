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
	uint16 port = _packet.GetUShort();
	_conn.SetMyPort(port);

	User* pUser = UserManager::GetInst()->Create(pNickname);
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	PacketType type;
	Packet p;

	if (pUser->GetState() == eLoginState::Logout)
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
			p.Add<uint16>(_conn.GetId());
			UserManager::GetInst()->Connect(_conn.GetId(), pUser);
			pLobby->Enter(_conn, pUser);
		}
	}
	else // 해당아이디로 누군가 로그인
	{
		type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
		p.Add<PacketType>(type);
	}

	wprintf(L"%s님 [%d] : ", pNickname, (int32)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	_conn.Send(p);
}