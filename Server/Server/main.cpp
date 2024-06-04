#include "ServerApp.h"

#pragma comment( lib, "ws2_32.lib")

int32 main()
{
	ServerApp app;
	
	if (app.Init(SERVER_INTERNAL_IP, 30002, SOMAXCONN) == true)
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