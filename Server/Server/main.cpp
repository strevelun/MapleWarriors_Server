﻿#include "ServerApp.h"

#pragma comment( lib, "ws2_32.lib")

int32 main()
{
	ServerApp app;

	if (app.Init("192.168.219.173", 30001, SOMAXCONN) == true)
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