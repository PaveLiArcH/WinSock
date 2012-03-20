// 2 # 9 Client
#include <WinSock2.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

const int c_BufSize=32768;

SOCKET connectionSocket;
char sendString[c_BufSize];
char recvString[c_BufSize];

const int c_ShowPeriod=1024;
int needDisplay;
long receivedSize;
long sendedSize;
long long timeSpent;
HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;

void exitFunc()
{
	if (connectionSocket!=INVALID_SOCKET)
	{
		closesocket(connectionSocket);
	}
	int _cleanupResult=WSACleanup();
}

void main()
{
	hConsole=INVALID_HANDLE_VALUE;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	connectionSocket=INVALID_SOCKET;
	WSADATA _wsaData;
	int _initResult=WSAStartup(MAKEWORD(1,1), &_wsaData);
	if (_initResult==0)
	{
		printf("WSA started\n");
		// IPv4, STREAM, TCP
		connectionSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		bool _linger=false;
		setsockopt(connectionSocket,SOL_SOCKET,SO_LINGER,(char *)&_linger,sizeof(bool));
		if (connectionSocket!=INVALID_SOCKET)
		{
			printf("Socket %x generated\n", connectionSocket);
			printf("Enter listen address (empty for loop): ");
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
				_socketAddress.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
			}
			atexit(exitFunc);
			bool _upDownOneTime=false;
			printf("1. Test up and down separately (default)\n");
			printf("2. Test up and down at one time\n");
			int _char=getch();
			printf("%c\n",_char);
			_upDownOneTime=(_char=='2');
			int _connectResult=connect(connectionSocket, (SOCKADDR *)&_socketAddress, sizeof(_socketAddress));
			if (_connectResult==0)
			{
				printf("Socket connected\n");

				bool _needContinue=true;

				unsigned long _notBlock=_upDownOneTime;
				int _fail=ioctlsocket(connectionSocket, FIONBIO, &_notBlock);
				if (_fail)
				{
					printf("Error: %x: ", _fail);
					printf("Socket error %x\n", WSAGetLastError());
					_needContinue=false;
				}

				int _lastError;
				timeSpent=0;
				receivedSize=0;
				sendedSize=0;

				printf("Estimated speed Down: ");
				GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo);
				COORD _consoleCursorDown={consoleScreenBufferInfo.dwCursorPosition.X,consoleScreenBufferInfo.dwCursorPosition.Y};

				printf("\nEstimated speed Up: ");
				GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo);
				COORD _consoleCursorUp={consoleScreenBufferInfo.dwCursorPosition.X,consoleScreenBufferInfo.dwCursorPosition.Y};

				printf("\n");
				GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo);
				COORD _consoleCursorEnd={consoleScreenBufferInfo.dwCursorPosition.X,consoleScreenBufferInfo.dwCursorPosition.Y};

				DWORD _start=timeGetTime(),_end;
				bool _send=false;
				int _sendState,_recvState;

				fd_set _recvSet, _sendSet;
				FD_ZERO(&_recvSet);
				FD_ZERO(&_sendSet);
				FD_SET(connectionSocket,&_recvSet);
				FD_SET(connectionSocket,&_sendSet);
				while (_needContinue)
				{
					if (_upDownOneTime)
					{
						timeval _timeout = {5, 1};
						FD_SET(connectionSocket,&_sendSet);
						FD_SET(connectionSocket,&_recvSet);
						select(NULL, &_recvSet, &_sendSet, NULL, &_timeout);
					}
					if (_upDownOneTime||_send)
					{
						_sendState=send(connectionSocket,sendString,sizeof(sendString),0);
						switch (_sendState)
						{
						case SOCKET_ERROR:
							_lastError=WSAGetLastError();
							if (_lastError&&(_lastError!=WSAEWOULDBLOCK))
							{
								SetConsoleCursorPosition(hConsole,_consoleCursorEnd);
								printf("Socket error: %x, can't send - disconnected\n", _lastError);
								_needContinue=false;
							}
							break;
						case NULL:
							SetConsoleCursorPosition(hConsole,_consoleCursorEnd);
							printf("Socket closed gracefully by server\n");
							_needContinue=false;
							break;
						default:
							sendedSize+=_sendState;
							break;
						}
					}
					if (_upDownOneTime||!_send)
					{
						_recvState=recv(connectionSocket,recvString,sizeof(recvString),0);
						switch (_recvState)
						{
						case SOCKET_ERROR:
							_lastError=WSAGetLastError();
							if (_lastError&&(_lastError!=WSAEWOULDBLOCK))
							{
								SetConsoleCursorPosition(hConsole,_consoleCursorEnd);
								printf("Socket error: %x\n",_lastError);
								_needContinue=false;
							}
							break;
						case NULL:
							SetConsoleCursorPosition(hConsole,_consoleCursorEnd);
							printf("Socket closed gracefully by server\n");
							_needContinue=false;
							break;
						default:
							receivedSize+=_recvState;
							//_needContinue=!kbhit();
							break;
						}
					}

					_end=timeGetTime();
					if ((_end-_start)>500)
					{
						timeSpent+=_end-_start;
						if (_upDownOneTime||_send)
						{
							float _speedUp=((sendedSize*1000.0f/timeSpent)/1024.0);
							SetConsoleCursorPosition(hConsole,_consoleCursorUp);
							printf("%8.3f kb/s",_speedUp);
						}
						if (_upDownOneTime||!_send)
						{
							float _speedDown=((receivedSize*1000.0f/timeSpent)/1024.0);
							SetConsoleCursorPosition(hConsole,_consoleCursorDown);
							printf("%8.3f kb/s",_speedDown);
						}
						timeSpent=0;
						receivedSize=0;
						sendedSize=0;
						_start=timeGetTime();
						_send=!_send;
					}
				}
				closesocket(connectionSocket);
				connectionSocket=INVALID_SOCKET;
			}
			else
			{
				MessageBoxA(NULL, "Error while trying to connect to socket", "Error", 0);
				return;
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