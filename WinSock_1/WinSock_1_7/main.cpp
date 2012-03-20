// 1 # 7
#include <WinSock2.h>
#include <stdio.h>

const int c_StrLen=72;
const int c_CrLfLen=2;

SOCKET usedSocket;
SOCKET acceptSocket;
char sendString[c_StrLen+c_CrLfLen+1];
unsigned int seed;
int sendedCount;

//����������: �������� (RFC 864) �� ������������� ������������ �� ������������ ���-����, �� ���� ��������� ������������. 
//			������� ������ 95 �������� �������� �� ASCII-������ (32-127). 
//			��� ����������������� �� 0 �� 94. 
//			��������� ���� �������� ���������������� (�.�. ����������� �� ������������� �� ������ ����� �������, ��� ������ �193 - ��� ������ ��� ������� 193 mod 95). 
//			�� ����� ��������� ��������������� �������� 72 ������, ������ �� ����-��� ������������� ����������� CrLf. 
//			������� ������ �������� ������� � 0 �� 71. ��������� ������ �������� ������� � 1 �� 72, � �.�. ����� �������, N-�� ������ �������� ������� �� N �� N mod 95.


void exitFunc()
{
	if (usedSocket!=INVALID_SOCKET)
	{
		closesocket(usedSocket);
	}
	if (acceptSocket!=INVALID_SOCKET)
	{
		closesocket(acceptSocket);
	}
	int _cleanupResult=WSACleanup();
}

char translate(unsigned int a_charNum)
{
	char _retChar;
	a_charNum%=95;
	_retChar=a_charNum+32;
	return _retChar;
}

void regenerateString()
{
	for (int i=0; i<c_StrLen; i++)
	{
		sendString[i]=translate(i+seed);
	}
	sendString[c_StrLen]='\r';
	sendString[c_StrLen+1]='\n';
	sendString[c_StrLen+2]='\0';
	seed++;
}

void main()
{
	usedSocket=INVALID_SOCKET;
	acceptSocket=INVALID_SOCKET;
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
			printf("Enter bind address: ");
			char _address[256];
			scanf("%s",_address);
			sockaddr_in _socketAddress;
			_socketAddress.sin_family=AF_INET;
			_socketAddress.sin_addr.S_un.S_addr=inet_addr(_address);
			_socketAddress.sin_port=htons(19);
			atexit(exitFunc);
			int _bindResult=bind(usedSocket, (SOCKADDR *)&_socketAddress, sizeof(_socketAddress));
			if (_bindResult==0)
			{
				printf("Socket binded\n");
				//// �������, ������� ���������� ��������� �������, ��������� ��������
				//int _listenResult=listen(usedSocket, SOMAXCONN);
				// �������, ������� ���������� ��� 1 �������
				int _listenResult=listen(usedSocket, 1);
				if (_listenResult==0)
				{
					printf("Socket listening\n");
					while (true)
					{
						acceptSocket=accept(usedSocket, NULL, NULL);
						if (acceptSocket!=INVALID_SOCKET)
						{
							printf("Client connected\n");
							seed=0;
							sendedCount=0;
							//char _buf[1];
							bool _send=true;
							while (_send)
							{
								//recv(acceptSocket,_buf,1,0);
								int _sendState=0;
								if (sendedCount<72)
								{
									regenerateString();
									_sendState=send(acceptSocket,sendString,sizeof(sendString),0);
									sendedCount++;
									_send=(_sendState!=SOCKET_ERROR)||(printf("Client disconnected\n")&false);
								} else
								{
									printf("Data sended\n");
									_send=false;
								}
							}
							closesocket(acceptSocket);
							acceptSocket=INVALID_SOCKET;
						} else
						{
							MessageBoxA(NULL, "Error while trying to accept data on socket", "Error", 0);
							return;
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