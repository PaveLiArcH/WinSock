// 3 # 9 Telnet client
#include <tchar.h>
#include <WinSock2.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <iostream>
#include <string>

SOCKET connectionSocket;

HANDLE _mutex, _communicationEnded;

const int c_BufSize=1024;
const std::string c_CrLf="\r\n";

void exitFunc()
{
	if (connectionSocket!=INVALID_SOCKET)
	{
		closesocket(connectionSocket);
	}
	int _cleanupResult=WSACleanup();
}


void printThreadSafe(const char *a_string, void *a_param)
{
	WaitForSingleObject(_mutex, INFINITE);
	printf(a_string, a_param);
	ReleaseMutex(_mutex);
}

void readUserReactions(void *)
{
	std::string _str;
	while (true)
	{
		std::getline(std::cin, _str);
		_str+=c_CrLf;
		send(connectionSocket, _str.c_str(), _str.length(), 0);
	}
}

void getTerminalReactions(void *)
{
	WaitForSingleObject(_communicationEnded, INFINITE);
	char buf[c_BufSize+1];
	bool _allOk=true;
	while (_allOk)
	{
		int _recvState=recv(connectionSocket,buf,c_BufSize,0);
		switch (_recvState)
		{
		case SOCKET_ERROR:
			_tprintf(_T("Socket error: %x\n"),WSAGetLastError());
			_allOk=false;
			break;
		case NULL:
			_tprintf(_T("Socket closed gracefully by server\n"));
			_allOk=false;
			break;
		default:
			int i=0;
			while (i<_recvState)
			{
				switch (buf[i])
				{
				case 0xFF:
					// IAC
					i++;
					if (i<_recvState)
					{
						/*switch (buf[i])
						{
						default:

						}*/
						i++;
						if (i<_recvState)
						{
							char _c=buf[i];
							bool _multiByte=(_c==0xFA);
							while (_multiByte&&(i<_recvState))
							{
								_multiByte=(buf[i]!=0xF0);
								i++;
							}
						}
					}
					break;
				default:
					printf("%c",buf[i]);
					i++;
					break;
				}
			}
			break;
		}
	}
	ReleaseMutex(_communicationEnded);
}

int _tmain(int argC, TCHAR *argV[])
{
	connectionSocket=INVALID_SOCKET;

	WSADATA _wsaData;

	_mutex=CreateMutex(NULL, TRUE, NULL);
	ReleaseMutex(_mutex);
	_communicationEnded=CreateMutex(NULL, TRUE, NULL);
	ReleaseMutex(_communicationEnded);

	int _initResult=WSAStartup(MAKEWORD(2,2), &_wsaData);
	if (_initResult==0)
	{
		_tprintf(_T("WSA started\n"));
		// IPv4, STREAM, TCP
		connectionSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connectionSocket!=INVALID_SOCKET)
		{
			_tprintf(_T("Socket %x generated\n"), connectionSocket);
			_tprintf(_T("Enter listen address (empty for loop): "));
			char _address[256];
			//scanf("%s",_address);
			fgets(_address, 256, stdin);
			sockaddr_in _socketAddress;
			_socketAddress.sin_family=AF_INET;
			_socketAddress.sin_port=htons(23);
			if ((strlen(_address)-1)>0)
			{
				_socketAddress.sin_addr.S_un.S_addr=inet_addr(_address);
			} else
			{
				_socketAddress.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
			}
			atexit(exitFunc);
			int _connectResult=connect(connectionSocket, (SOCKADDR *)&_socketAddress, sizeof(_socketAddress));
			if (_connectResult==0)
			{
				_tprintf(_T("Socket connected\n"));

				/*int _fail=ioctlsocket(connectionSocket, FIONBIO, &_notBlock);
				if (_fail)
				{
					printf("Error: %x: ", _fail);
					printf("Socket error %x\n", WSAGetLastError());
					_needContinue=false;
				}*/

				int _lastError;
				int _sendState,_recvState;
				fd_set _recvSet, _sendSet;
				FD_ZERO(&_recvSet);
				FD_ZERO(&_sendSet);
				FD_SET(connectionSocket, &_recvSet);
				FD_SET(connectionSocket, &_sendSet);
				_beginthread((void (__cdecl *)(void *))readUserReactions, NULL, NULL);
				_beginthread((void (__cdecl *)(void *))getTerminalReactions, NULL, NULL);
				Sleep(5000);
				WaitForSingleObject(_communicationEnded, INFINITE);
				closesocket(connectionSocket);
				connectionSocket=INVALID_SOCKET;
			}
			else
			{
				_tprintf(_T("Failed to connect"));
				return 3;
			}
		} else
		{
			_tprintf(_T("Failed to create socket"));
			return 2;
		}
	} else
	{
		_tprintf(_T("Failed to WSAStartup\n"));
		return 1;
	}
	return 0;
}