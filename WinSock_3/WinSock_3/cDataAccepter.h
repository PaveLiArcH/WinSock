#pragma once

#include <vector>
#include "cCommand.h"
#include "cEscapeCommand.h"

using std::vector;

class cDataAccepter
{
	// 0 - plain text
	// 1 - command receiving
	int cf_state;
	vector<cCommand *> cf_commands;
	cCommand *cf_currentCommand;
	cEscapeCommandProcessor cf_nonCommandProcessor;
public:
	cDataAccepter()
	{
		cf_state=0;
		cf_currentCommand=NULL;
	}
	
	//	��������		���������	���	������������	��������
	//	Null			NUL			 0	��				��� ��������
	//	Line Feed		LF			10	��				������� ������� �� ��������� ������ � ����������� �������
	//	Carriage Return	CR			13	��				������� ������� � ������ ������� ������
	//	Bell			BEL			 7	���				�������� ��� ����� ������
	//	Back Space		BS			 8	���				������� ������� �� ���������� ������� (�� ����������� � ������ ������)
	//	Horizontal Tab	HT			 9	���				������� ������� �� ��������� ������� �������������� ���������
	//	Vertical Tab	VT			11	���				������� ������� �� ��������� ������� ������������ ���������
	//	Form Feed		FF			12	���				������� ������� �� ������ ��������� �������� � ����������� �������������� �������

	void cm_Read(int a_size, unsigned char *a_data)
	{
		for (int i=0; i<a_size; i++)
		{
			switch (cf_state)
			{
			case 0:
				if (a_data[i]=='\xff')
				{
					cf_state=1;
					cf_currentCommand=new cCommand();
				} else
				{
					if (a_data[i]=='\x0c')
					{
						//system("cls");
						SetPos(0,0);
					} else
					{
						cf_nonCommandProcessor.cm_AppendByte(a_data[i]);
						//printf("%c", a_data[i]);
					}
				}
				break;
			case 1:
				if (cf_currentCommand->cm_AppendByte(a_data[i]))
				{
					cf_commands.push_back(cf_currentCommand);
					cf_state=0;
					cf_currentCommand=NULL;
				}
				break;
			}
		}
	}
	
	~cDataAccepter()
	{
		if (cf_currentCommand!=NULL)
		{
			delete cf_currentCommand;
		}
		for(vector<cCommand *>::iterator _i=cf_commands.begin(); _i!=cf_commands.end(); ++_i)
		{
			delete *_i;
		}
	}
};