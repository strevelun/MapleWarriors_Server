﻿#include "ServerApp.h"

#pragma comment( lib, "ws2_32.lib")

int main()
{
	ServerApp app;

	if (app.Init("192.168.219.107", 30001, SOMAXCONN) == true)
	{
		app.Run();
	}
	else
	{
		app.Shutdown();
	}

	printf("종료");
	system("pause");
	return 0;
}