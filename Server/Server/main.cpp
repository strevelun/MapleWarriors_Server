#include "ServerApp.h"

#pragma comment( lib, "ws2_32.lib")

int32 main()
{
	ServerApp app;
	
	if (app.Init(SERVER_INTERNAL_IP, SERVER_PORT, SOMAXCONN) == true)
	{
		app.Run();
	}

	app.Shutdown();

	printf("종료");
	system("pause");
	return 0;
}