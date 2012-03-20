// 1 # 9 Client
#include <WinSock2.h>
#include <stdio.h>
#include <conio.h>

const int c_BufSize=1024;

SOCKET connectionSocket;
char buf[c_BufSize+1];

//����������: �������� (RFC 864) �� ������������� ������������ �� ������������ ���-����, �� ���� ��������� ������������. 
//			������� ������ 95 �������� �������� �� ASCII-������ (32-127). 
//			��� ����������������� �� 0 �� 94. 
//			��������� ���� �������� ���������������� (�.�. ����������� �� ������������� �� ������ ����� �������, ��� ������ �193 - ��� ������ ��� ������� 193 mod 95). 
//			�� ����� ��������� ��������������� �������� ������, ������ �� ����-��� ������������� ����������� CrLf. 
//			������� ������ �������� ������� � 0 �� 71. ��������� ������ �������� ������� � 1 �� 72, � �.�. ����� �������, N-�� ������ �������� ������� �� N �� N mod 95.


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
			printf("Socket generated\n");
			printf("Enter listen address: ");
			char _address[256];
			scanf("%s",_address);
			sockaddr_in _socketAddress;
			_socketAddress.sin_family=AF_INET;
			_socketAddress.sin_addr.S_un.S_addr=inet_addr(_address);
			_socketAddress.sin_port=htons(19);
			atexit(exitFunc);
			int _connectResult=connect(connectionSocket, (SOCKADDR *)&_socketAddress, sizeof(_socketAddress));
			if (_connectResult==0)
			{
				printf("Socket connected\n");
				bool _needContinue=true;
				int _lastError;
				while (_needContinue)
				{
					int _recvState=recv(connectionSocket,buf,c_BufSize,0);
					switch (_recvState)
					{
					case SOCKET_ERROR:
						_lastError=WSAGetLastError();
						printf("Socket error: %x\n",_lastError);
						_needContinue=false;
						break;
					case NULL:
						printf("Socket closed gracefully by server\n");
						_needContinue=false;
						break;
					default:
						//buf[_recvState]='\0';
						//printf("%s",buf);
						for (int i=0; i<_recvState; i++)
						{
							printf("%c",buf[i]);
						}
						_needContinue=!kbhit();
						break;
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