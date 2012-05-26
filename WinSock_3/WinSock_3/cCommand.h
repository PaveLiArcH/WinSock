#pragma once

#include <vector>
using std::vector;

class cCommand
{
	// 0 - command code receiving
	// 1 - command args receiving
	// 2 - multibyte args receiving
	// 3 - filled
	int cf_state;
public:
	unsigned char cf_Code;
	vector<unsigned char> cf_Data;
	
	cCommand()
	{
		cf_Code=0;
		cf_state=0;
	}

	bool cm_AppendByte(unsigned char a_byte)
	{
		bool _retBool=false;
		switch (cf_state)
		{
		case 0:
			cf_Code=a_byte;
			cf_state=1;
			break;
		case 1:
			if (a_byte=='\xFA')
			{
				cf_state=2;
			} else
			{
				_retBool=true;
				cf_Data.push_back(a_byte);
			}
			break;
		case 2:
			if (a_byte!='\xf0')
			{
				cf_Data.push_back(a_byte);
			} else
			{
				cf_state=3;
				_retBool=true;
			}
			break;
		default:
			throw 1;
			break;
		}
		return _retBool;
	}
};