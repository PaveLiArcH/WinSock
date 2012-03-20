// 2 # 9
#include <WinSock2.h>
#include <stdio.h>
#include <process.h>

const int c_StrLen=72;
const int c_CrLfLen=2;

SOCKET usedSocket;
//SOCKET acceptSocket;
char sendString[c_StrLen+c_CrLfLen+1];
unsigned int seed;
int sendedCount;

//Примечание: Стандарт (RFC 864) не устанавливает спецификацию на возвращаемые символы, но дает следующие рекомендации. 
//			Берутся первые 95 печатных символов из ASCII-набора (32-127). 
//			Они пронумеровываются от 0 до 94. 
//			Множество этих символов закольцовывается (т.е. расширяется до бесконечности по модулю таким образом, что символ №193 - это символ под номером 193 mod 95). 
//			Из этого множества последовательно строятся строки, каждая из кото-рых заканчивается комбинацией CrLf. 
//			Нулевая строка содержит символы с 0 до 71. Следующая строка содержит символы с 1 до 72, и т.д. Таким образом, N-ая строка содержит символы от N до N mod 95.


void exitFunc()
{
	if (usedSocket!=INVALID_SOCKET)
	{
		closesocket(usedSocket);
	}
	/*if (acceptSocket!=INVALID_SOCKET)
	{
		closesocket(acceptSocket);
	}*/
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

void threadedSend(SOCKET a_socket)
{
	printf("Client connected\n");
	seed=0;
	sendedCount=0;
	bool _send=true;
	while (_send)
	{
		int _sendState=0;
		regenerateString();
		_sendState=send(a_socket,sendString,sizeof(sendString),0);
		sendedCount++;
		int _lastError;
		switch (_sendState)
		{
		case SOCKET_ERROR:
			_lastError=WSAGetLastError();
			printf("Socket error: %x, client disconnected\n",_lastError);
			_send=false;
			break;
		case NULL:
			printf("Socket closed gracefully by client, client disconnected\n");
			_send=false;
			break;
		default:
			break;
		}
	}
	closesocket(a_socket);
	a_socket=INVALID_SOCKET;
}

void main()
{
	usedSocket=INVALID_SOCKET;
	//acceptSocket=INVALID_SOCKET;
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
				//// слушаем, очередь соединений настолько большая, насколько возможно
				//int _listenResult=listen(usedSocket, SOMAXCONN);
				// слушаем, очередь соединений для 1 клиента
				int _listenResult=listen(usedSocket, 1);
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