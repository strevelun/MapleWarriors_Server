#include "LoginPacketHandler.h"
#include "../../User/UserManager.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"

// ��Ŷ ���� �ð� �׽�Ʈ
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
	_conn.SetMyUDPPort(port);

	uint8 ipBytes[4];
	ipBytes[0] = _packet.GetUInt8();
	ipBytes[1] = _packet.GetUInt8();
	ipBytes[2] = _packet.GetUInt8();
	ipBytes[3] = _packet.GetUInt8();
	_conn.SetPrivateIP(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);

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
	else // �ش���̵�� ������ �α���
	{
		type = (PacketType)eServer::LoginFailure_AlreadyLoggedIn;
		p.Add<PacketType>(type);
	}

	wprintf(L"%s�� [%d] : ", pNickname, (int32)_conn.GetSocket());
	if ((eServer)type == eServer::LoginSuccess) std::cout << "�α��� ����!" << '\n';
	else std::cout << "�α��� ����!" << '\n';

	_conn.Send(p);
}