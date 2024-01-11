#include "LoginPacketHandler.h"
#include "../../UserManager.h"
#include "../../Lobby/LobbyManager.h"

void Login::Test(Connection& _conn, PacketReader& _packet)
{
}

void Login::LoginReq(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pNickname = _packet.GetWString();
	User* pUser = UserManager::GetInst()->Create(pNickname);
	if (pUser->GetState() == eLoginState::Logout)
	{
		pUser->SetState(eLoginState::Login);
		_conn.SetUser(pUser);
		_conn.SetSceneState(eSceneState::Lobby);
	}

	PacketType type = PacketType(_conn.GetUser() ? Server::LoginSuccess : Server::LoginFailure);

	wprintf(L"%s�� [%d] : ", pNickname, (int)_conn.GetSocket());
	if ((Server)type == Server::LoginSuccess) std::cout << "�α��� ����!" << '\n';
	else std::cout << "�α��� ����!" << '\n';

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	pLobby->Add(_conn);

	Packet p;
	p.Add<PacketType>(type);
	_conn.Send(p);

	printf("LoginReq\n");
}