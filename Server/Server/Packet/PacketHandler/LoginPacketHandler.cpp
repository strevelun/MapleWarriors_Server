#include "LoginPacketHandler.h"
#include "../../UserManager.h"

void Login::Test(Connection& _conn, PacketReader& _packet)
{
}

void Login::LoginReq(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pNickname = _packet.GetWString();
	User* pUser = UserManager::GetInst()->Create(pNickname);
	pUser->HandleLogin(_conn);

	PacketType type = PacketType(_conn.GetUser() ? Server::LoginSuccess : Server::LoginFailure);

	wprintf(L"%s님 [%d] : ", pNickname, (int)_conn.GetSocket());
	if ((Server)type == Server::LoginSuccess) std::cout << "로그인 성공!" << '\n';
	else std::cout << "로그인 실패!" << '\n';

	Packet p;
	p.Add<PacketType>(type);
	_conn.Send(p);

	printf("LoginReq\n");
}