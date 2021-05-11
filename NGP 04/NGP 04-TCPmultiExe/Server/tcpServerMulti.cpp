#include <stdio.h>
#include <WinSock2.h>

#define MAX_CLIENT 3

void main()
{
	WSADATA wsaData;
	SOCKET socketListen, socketTemp;
	SOCKET socketClient[MAX_CLIENT];
	struct sockaddr_in serverAddr;
	int  k;


	//  네트워크를 초기화 한다.
	::WSAStartup( 0x202, &wsaData );

    for(k=0;k<MAX_CLIENT;k++)
		socketClient[k] = INVALID_SOCKET;


	socketListen = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socketListen == INVALID_SOCKET )
	{
		printf( "Socket create error !!\n" );
		return;
	}

	::memset( &serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_addr.s_addr	= ::htonl( INADDR_ANY ); // ::inet_addr( "165.194.115.25" ); //::htonl( INADDR_LOOPBACK   INADDR_ANY );
	serverAddr.sin_port			= ::htons( 8600 );

	if( ::bind( socketListen, ( struct sockaddr* )&serverAddr, sizeof( serverAddr ) ) == SOCKET_ERROR )
	{
		printf( "bind failed!! : %d\n", ::WSAGetLastError() );
		return;
	}

	if( ::listen( socketListen, SOMAXCONN ) == SOCKET_ERROR )
	{
		printf( "listen failed!! : %d\n", ::WSAGetLastError() );
		return;
	}

	printf("****** Server Start with Maximum %d at %s ***\n", MAX_CLIENT, ::inet_ntoa( serverAddr.sin_addr ) );

	
//////////////////////////////////////////////////////////////////// server loop 	
	while( 1 )
	{
		fd_set fds;
		struct timeval tv = { 0, 100 };		//  0.0001 초


		FD_ZERO(&fds);

		FD_SET(socketListen, &fds); // socketListen, socketClient[k]
		for (k = 0; k < MAX_CLIENT; k++) {
			if (socketClient[k] != INVALID_SOCKET)
				FD_SET(socketClient[k], &fds);
		}
		::select(MAX_CLIENT+2, &fds, 0, 0, &tv); 
		
		if( FD_ISSET(socketListen, &fds) )  // check there is new client on socketListen
		{
			struct sockaddr_in fromAddr;
			int size = sizeof( fromAddr ); //새로 들어온 주소 확인


			for(k=0;k<MAX_CLIENT;k++) // 연결 안되고 비어있는 소켓이 있는지 확인
				if(socketClient[k] == INVALID_SOCKET)
					break;

// if number of clients reaches maximum
			if(k==MAX_CLIENT){
				socketTemp = ::accept(socketListen, (struct sockaddr*)&fromAddr, &size);
				::shutdown(socketTemp, SD_BOTH);
				::closesocket(socketTemp);
				socketTemp = INVALID_SOCKET;
			}
// othwise accept it
			else
			{
				socketClient[k] = ::accept(socketListen, (struct sockaddr*)&fromAddr, &size); //hmm
			}
		}

// if not new client, already accepted client 
		else
		{
			for(k=0;k<MAX_CLIENT;k++)
				if(socketClient[k] != INVALID_SOCKET && FD_ISSET( socketClient[k], &fds ))
				{
					//  데이터를 받은 후에는 Echo 통신을 한다.
					char recvBuffer[127];
					int recvBytes;

					recvBytes = ::recv(socketClient[k], recvBuffer, 127, 0);
					if(recvBytes<1){
						//  통신이 끝난 후에는 클라이언트의 접속을 해제한다.
						printf( "*** Closed the client : %d(%d)\n", k, socketClient[k] );

						::shutdown( socketClient[k], SD_BOTH );
						::closesocket( socketClient[k] );
						socketClient[k] = INVALID_SOCKET;
					}
					else{
						printf( "%d> %d bytes received : %s\n", k, recvBytes, recvBuffer );
						::send(socketClient[k], recvBuffer, recvBytes, 0 );
					}
				}
		}

	}
//////////////////////////////////////////////////////////////////// end of server
	
	::WSACleanup();
}