// 2 # 9
#include <WinSock2.h>
#include <stdio.h>
#include <process.h>

const int c_BufSize=32768;

SOCKET usedSocket;
char sendString[c_BufSize];
char recvString[c_BufSize];

fd_set socketSet;

HANDLE _mutex;

void socketAddThreadSafe(SOCKET a_socket)
{
	WaitForSingleObject(_mutex, INFINITE);
	FD_SET(a_socket,&socketSet);
	ReleaseMutex(_mutex);
}

void socketDelThreadSafe(SOCKET a_socket)
{
	WaitForSingleObject(_mutex, INFINITE);
	FD_CLR(a_socket,&socketSet);
	ReleaseMutex(_mutex);
}

void printThreadSafe(const char *a_string, void *a_param)
{
	WaitForSingleObject(_mutex, INFINITE);
	printf(a_string, a_param);
	ReleaseMutex(_mutex);
}

void exitFunc()
{
	if (usedSocket!=INVALID_SOCKET)
	{
		closesocket(usedSocket);
	}
	for (unsigned int i=0; i<socketSet.fd_count;i++)
	{
		closesocket(socketSet.fd_array[i]);
	}
	int _cleanupResult=WSACleanup();
}

void threadedSend(SOCKET a_socket)
{
	socketAddThreadSafe(a_socket);
	bool _send=true;
	unsigned long _notBlock=true;
	printThreadSafe("Client %x connected\n",(void *)a_socket);
	/*int _fail=ioctlsocket(a_socket, FIONBIO, &_notBlock);
	if (_fail)
	{
		printThreadSafe("Error: %x: ", (void *)_fail);
		printThreadSafe("Socket error %x\n", (void *)WSAGetLastError());
		_send=false;
	}*/
	fd_set _recvSet, _sendSet;
	FD_ZERO(&_recvSet);
	FD_ZERO(&_sendSet);
	FD_SET(a_socket,&_recvSet);
	FD_SET(a_socket,&_sendSet);
	timeval _timeout = {5, 1};
	while (_send)
	{
		FD_SET(a_socket,&_recvSet);
		FD_SET(a_socket,&_sendSet);
		select(NULL, &_recvSet, &_sendSet, NULL, &_timeout);
		bool _sendable=_sendSet.fd_count;
		bool _recvable=_recvSet.fd_count;
		int _lastError;
		if (_sendable)
		{
			int _sendState=send(a_socket,sendString,sizeof(sendString),0);
			switch (_sendState)
			{
			case SOCKET_ERROR:
				_lastError=WSAGetLastError();
				if (_lastError&&(_lastError!=WSAEWOULDBLOCK))
				{
					printThreadSafe("Socket error: %x",(void *)_lastError);
					printThreadSafe(", client %x disconnected\n",(void *)a_socket);
					_send=false;
				}
				break;
			case NULL:
				printThreadSafe("Socket closed gracefully by client",NULL);
				printThreadSafe(", client %x disconnected\n",(void *)a_socket);
				_send=false;
				break;
			default:
				break;
			}
		}
		if (_recvable)
		{
			int _recvState=recv(a_socket,recvString,sizeof(recvString),0);
			if (_send)
			{
				switch (_recvState)
				{
				case SOCKET_ERROR:
					_lastError=WSAGetLastError();
					if (_lastError&&(_lastError!=WSAEWOULDBLOCK))
					{
						printThreadSafe("Socket error: %x",(void *)_lastError);
						printThreadSafe(", client %x disconnected\n",(void *)a_socket);
						_send=false;
					}
					break;
				case NULL:
					printThreadSafe("Socket closed gracefully by client",NULL);
					printThreadSafe(", client %x disconnected\n",(void *)a_socket);
					_send=false;
					break;
				default:
					break;
				}
			}
		}
	}
	closesocket(a_socket);
	socketDelThreadSafe(a_socket);
	a_socket=INVALID_SOCKET;
}

void main()
{
	FD_ZERO(&socketSet);
	_mutex=CreateMutex(NULL,TRUE,NULL);
	ReleaseMutex(_mutex);
	usedSocket=INVALID_SOCKET;
	WSADATA _wsaData;
	int _initResult=WSAStartup(MAKEWORD(1,1), &_wsaData);
	if (_initResult==0)
	{
		printf("WSA started\n");
		// IPv4, STREAM, TCP
		usedSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (usedSocket!=INVALID_SOCKET)
		{
			printf("Socket generated\n");
			printf("Enter bind address (empty for any): ");
			char _address[256];
			//scanf("%s",_address);
			fgets(_address, 256, stdin);
			sockaddr_in _socketAddress;
			_socketAddress.sin_family=AF_INET;
			_socketAddress.sin_port=htons(19);
			if ((strlen(_address)-1)>0)
			{
				_socketAddress.sin_addr.S_un.S_addr=inet_addr(_address);
			} else
			{
				_socketAddress.sin_addr.S_un.S_addr=INADDR_ANY;
			}
			atexit(exitFunc);
			int _bindResult=bind(usedSocket, (SOCKADDR *)&_socketAddress, sizeof(_socketAddress));
			if (_bindResult==0)
			{
				printf("Socket binded\n");
				// слушаем, очередь соединений настолько большая, насколько возможно
				int _listenResult=listen(usedSocket, SOMAXCONN);
				if (_listenResult==0)
				{
					printf("Socket listening\n");
					while (true)
					{
						SOCKET acceptSocket=accept(usedSocket, NULL, NULL);
						if (acceptSocket!=INVALID_SOCKET)
						{
							_beginthread((void (__cdecl *)(void *))threadedSend,sizeof(SOCKET),(void *)acceptSocket);
						} else
						{
							MessageBoxA(NULL, "Error while trying to accept data on socket", "Error", 0);
							continue;
						}
					}
				} else
				{
					MessageBoxA(NULL, "Error while trying to listen on socket", "Error", 0);
					return;
				}
			}
		} else
		{
			MessageBoxA(NULL, "INVALID_SOCKET", "Error", 0);
			return;
		}
	} else
	{
		MessageBoxA(NULL, "Unsuccessful init", "Error", 0);
	}
}