#pragma once

#include <vector>
#include <windows.h>
using std::vector;

void SetPos(int x, int y)
{ 
    COORD _pos = {x, y};
    HANDLE _output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(_output, _pos);
}

class cEscapeCommandProcessor
{
	// 0 - ordinal data
	// 1 - first byte receiving
	// 2 - second byte receiving
	// 3 - third byte receiving
	// 4 - fourth byte receiving
	// 5 - command finished
	// 6 - wrong command, printing out
	// … - other states
	int cf_state;
public:
	vector<unsigned char> cf_Data;
	
	cEscapeCommandProcessor()
	{
		cf_state=0;
	}

	void cm_AppendByte(unsigned char a_byte)
	{
		switch (cf_state)
		{
		case 0:
			if (a_byte=='\x1b')
			{
				cf_state=1;
				cf_Data.push_back(a_byte);
			} else
			{
				printf("%c", a_byte);
			}
			break;
		case 1:
			switch (a_byte)
			{
			case '\x20':
			case '\x23':
			case '\x28':
			case '\x29':
			case '\x2a':
			case '\x2b':
			case '\x2d':
			case '\x2e':
			case '\x2f':
			case '\x4f':
			case '\x5b':
				cf_state=2;
				break;
			case '\x37': //SC			Save Cursor State
			case '\x38': //RC			Restore Cursor
			case '\x3d': //PAM			App. keypad mode
			case '\x3e': //PNM			Num. keypad mode
			case '\x44': //IND			Index
			case '\x45': //NEL			Next line
			case '\x48': //HTS			Horizontal tab set
			case '\x4d': //RI			Reverse Index
			case '\x4e': //SS2			Single shift 2
			case '\x50': //DCS			Device control string
			case '\x5a': //DA			Device attr request
			case '\x5c': //ST			String terminator
			case '\x60': //DMI			Disable manual input
			case '\x62': //EMI			Enable manual input
			case '\x63': //RIS			Terminal reset
			case '\x6e': //LS2			Lock shift 2
			case '\x6f': //LS3			Lock shift 3
			case '\x7c': //LS3R			Lock shift 3 (R)
			case '\x7d': //LS2R			Lock shift 2 (R)
			case '\x7e': //LS1R			Lock shift 1 (R)
				cf_state=5;
				break;
			default:
				cf_state=6;
				break;
			}
			cf_Data.push_back(a_byte);
			break;
		case 2:
			switch (cf_Data.back())
			{
			case '\x20':
				switch (a_byte)
				{
				case '\x46': //S7C			Select 7bit ctrls
				case '\x47': //S8C			Select 8bit ctrls
					cf_state=5;
					break;
				default:
					cf_state=6;
					break;
				}
				break;
			case '\x23':
				switch (a_byte)
				{
				case '\x33': //DHL			Double height, top
				case '\x34': //DHL			Double height, bottom
				case '\x35': //SWL			Single width/height
				case '\x36': //DWL			Double width
				case '\x38': //ALN			Screen alignment
					cf_state=5;
					break;
				default:
					cf_state=6;
					break;
				}
				break;
			case '\x28':
			case '\x29':
			case '\x2a':
			case '\x2b':
			case '\x2d':
			case '\x2e':
			case '\x2f':
				cf_state=5;
				break;
			case '\x4f':
				switch (a_byte)
				{
				case '\x70': //AK-0			App keypad 0
				case '\x71': //AK-1			App keypad 1
				case '\x72': //AK-2			App keypad 2
				case '\x73': //AK-3			App keypad 3
				case '\x74': //AK-4			App keypad 4
				case '\x75': //AK-5			App keypad 5
				case '\x76': //AK-6			App keypad 6
				case '\x77': //AK-7			App keypad 7
				case '\x78': //AK-8			App keypad 8
				case '\x79': //AK-9			App keypad 9
				case '\x6d': //AK-(-)		App keypad minus
				case '\x6c': //AK-(,)		App keypad comma
				case '\x6e': //AK-(.)		App keypad period
				case '\x4d': //AK-CR		App keypad Enter
				case '\x50': //PF1			App keypad PF1
				case '\x51': //PF2			App keypad PF2
				case '\x52': //PF3			App keypad PF3
				case '\x53': //PF4			App keypad PF4
					cf_state=5;
					break;
				default:
					cf_state=6;
					break;
				}
				break;
			case '\x5b':
				switch (a_byte)
				{
				case '\x40': //ICH			Insert character
				case '\x41': //CUU			Cursor up
				case '\x42': //CUD			Cursor down
				case '\x43': //CUF			Cursor forward
				case '\x44': //CUB			Cursor backward
				case '\x45': //CNL			Cursor Next Line
				case '\x46': //CPL			Cursor Previous Line
				case '\x47': //HPA			Horizontal Pos Abs
				case '\x48': //CUP			Cursor position
				case '\x4a': //ED			Erase data
				case '\x4b': //EL			Erase line
				case '\x4c': //IL			Insert line
				case '\x4d': //DL			Delete line
				case '\x4f': //EA			Erase area
				case '\x50': //DCH			Delete character
				case '\x53': //SU			Scroll Up
				case '\x54': //SD			Scroll Down
				case '\x58': //ECH			Erase character
				case '\x5a': //CBT			Cursor back tab
				case '\x63': //DA			Device attributes
				case '\x66': //HVP			Hor/vertical position
				case '\x67': //TBC			Tab clear
				case '\x68': //SM			Set mode
				case '\x6c': //RM			Reset mode
				case '\x6d': //SGR			Set graphic rendition
				case '\x6e': //DSR			Device status
				case '\x72': //SCR			Scrolling region
				case '\x73': //SC			Save cursor
				case '\x75': //RC			Restore cursor
					cf_state=5;
					break;
				case '\x30':
				case '\x34':
				case '\x35':
				case '\x31':
				case '\x32':
				case '\x33':
					cf_state=3;
					break;
				default:
					cf_state=6;
					break;
				}
				break;
			default:
				cf_state=6;
				break;
			}
			cf_Data.push_back(a_byte);
			break;
		case 3:
			switch (cf_Data.back())
			{
				case '\x30':
					cf_state=(a_byte=='\x69' //MC/0			Print screen
						)?5:6;
					break;
				case '\x34': //MC/4			End host print
							 //SM83			Set Mode 8300
				case '\x35': //MC/5			Start host print
							 //SM64			Set Mode Ansi
					cf_state=((a_byte=='\x69')||(a_byte=='\x71'))?5:6;
					break;
				case '\x31': //SM78			Set Mode 7800
				case '\x32': //SM77			Set Mode 7700
				case '\x33': //SM73			Set Mode 7300
					cf_state=(a_byte=='\x71')?5:6;
					break;
				default:
					cf_state=6;
					break;
			}
			cf_Data.push_back(a_byte);
			break;
		default:
			throw 1;
			break;
		}
		unsigned char _byte;
		switch (cf_state)
		{
		case 5:
			_byte=cf_Data.back(); // esc == 1b
			cf_Data.pop_back();
			_byte=cf_Data.back();
			cf_Data.pop_back();
			switch (_byte)
			{
			case '\x20':
				_byte=cf_Data.back();
				cf_Data.pop_back();
				switch (_byte)
				{
				case '\x46':
					//S7C			Select 7bit ctrls
					// i dunno what to do, lol
					break;
				case '\x47':
					//S8C			Select 8bit ctrls
					// i dunno what to do, lol
					break;
				}
				break;
			case '\x23':
				_byte=cf_Data.back();
				cf_Data.pop_back();
				switch (_byte)
				{
				case '\x33':
					//DHL			Double height, top
					// i dunno what to do, lol
					break;
				case '\x34':
					//DHL			Double height, bottom
					// i dunno what to do, lol
					break;
				case '\x35':
					//SWL			Single width/height
					// i dunno what to do, lol
					break;
				case '\x36':
					//DWL			Double width
					// i dunno what to do, lol
					break;
				case '\x38':
					//ALN			Screen alignment
					// i dunno what to do, lol
					break;
				}
				break;
			case '\x28':
				//SCS-0			Select G0 set (94)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x29':
				//SCS-1			Select G1 set (94)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x2a':
				//SCS-2			Select G2 set (94)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x2b':
				//SCS-3			Select G3 set (94)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x2d':
				//SCS-1A		Select G1 set (96)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x2e':
				//SCS-2A		Select G2 set (96)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x2f':
				//SCS-3A		Select G3 set (96)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x37':
				//SC			Save Cursor State
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x38':
				//RC			Restore Cursor
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x3d':
				//PAM			App. keypad mode
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x3e':
				//PNM			Num. keypad mode
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x44':
				//IND			Index
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x45':
				//NEL			Next line
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x48':
				//HTS			Horizontal tab set
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x4d':
				//RI			Reverse Index
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x4e':
				//SS2			Single shift 2
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x50':
				//DCS			Device control string
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x5a':
				//DA			Device attr request
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x5c':
				//ST			String terminator
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x60':
				//DMI			Disable manual input
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x62':
				//EMI			Enable manual input
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x63':
				//RIS			Terminal reset
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x6e':
				//LS2			Lock shift 2
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x6f':
				//LS3			Lock shift 3
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x7c':
				//LS3R			Lock shift 3 (R)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x7d':
				//LS2R			Lock shift 2 (R)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x7e':
				//LS1R			Lock shift 1 (R)
				_byte=cf_Data.back();
				cf_Data.pop_back();
				// i dunno what to do, lol
				break;
			case '\x4f':
				if (cf_Data.size()==0)
				{
					//SS3			Single shift 3
					// i dunno what to do, lol
				} else
				{
					_byte=cf_Data.back();
					cf_Data.pop_back();
					switch (_byte)
					{
					case '\x70':
						//AK-0			App keypad 0
						// i dunno what to do, lol
						break;
					case '\x71':
						//AK-1			App keypad 1
						// i dunno what to do, lol
						break;
					case '\x72':
						//AK-2			App keypad 2
						// i dunno what to do, lol
						break;
					case '\x73':
						//AK-3			App keypad 3
						// i dunno what to do, lol
						break;
					case '\x74':
						//AK-4			App keypad 4
						// i dunno what to do, lol
						break;
					case '\x75':
						//AK-5			App keypad 5
						// i dunno what to do, lol
						break;
					case '\x76':
						//AK-6			App keypad 6
						// i dunno what to do, lol
						break;
					case '\x77':
						//AK-7			App keypad 7
						// i dunno what to do, lol
						break;
					case '\x78':
						//AK-8			App keypad 8
						// i dunno what to do, lol
						break;
					case '\x79':
						//AK-9			App keypad 9
						// i dunno what to do, lol
						break;
					case '\x6d':
						//AK-(-)		App keypad minus
						// i dunno what to do, lol
						break;
					case '\x6c':
						//AK-(,)		App keypad comma
						// i dunno what to do, lol
						break;
					case '\x6e':
						//AK-(.)		App keypad period
						// i dunno what to do, lol
						break;
					case '\x4d':
						//AK-CR		App keypad Enter
						// i dunno what to do, lol
						break;
					case '\x50':
						//PF1			App keypad PF1
						// i dunno what to do, lol
						break;
					case '\x51':
						//PF2			App keypad PF2
						// i dunno what to do, lol
						break;
					case '\x52':
						//PF3			App keypad PF3
						// i dunno what to do, lol
						break;
					case '\x53':
						//PF4			App keypad PF4
						// i dunno what to do, lol
						break;
					}
				}
				break;
			case '\x5b':
				if (cf_Data.size()==0)
				{
					//CSI			Control Seq Intro
					// i dunno what to do, lol
				} else
				{
					_byte=cf_Data.back();
					cf_Data.pop_back();
					switch (a_byte)
					{
					case '\x40':
						//ICH			Insert character
						// i dunno what to do, lol
						break;
					case '\x41':
						//CUU			Cursor up
						// i dunno what to do, lol
						break;
					case '\x42':
						//CUD			Cursor down
						// i dunno what to do, lol
						break;
					case '\x43':
						//CUF			Cursor forward
						// i dunno what to do, lol
						break;
					case '\x44':
						//CUB			Cursor backward
						// i dunno what to do, lol
						break;
					case '\x45':
						//CNL			Cursor Next Line
						// i dunno what to do, lol
						break;
					case '\x46':
						//CPL			Cursor Previous Line
						// i dunno what to do, lol
						break;
					case '\x47':
						//HPA			Horizontal Pos Abs
						// i dunno what to do, lol
						break;
					case '\x48':
						//CUP			Cursor position
						SetPos(0,0);
						break;
					case '\x4a':
						//ED			Erase data
						// i dunno what to do, lol
						break;
					case '\x4b':
						//EL			Erase line
						// i dunno what to do, lol
						break;
					case '\x4c':
						//IL			Insert line
						// i dunno what to do, lol
						break;
					case '\x4d':
						//DL			Delete line
						// i dunno what to do, lol
						break;
					case '\x4f':
						//EA			Erase area
						// i dunno what to do, lol
						break;
					case '\x50':
						//DCH			Delete character
						// i dunno what to do, lol
						break;
					case '\x53':
						//SU			Scroll Up
						// i dunno what to do, lol
						break;
					case '\x54':
						//SD			Scroll Down
						// i dunno what to do, lol
						break;
					case '\x58':
						//ECH			Erase character
						// i dunno what to do, lol
						break;
					case '\x5a':
						//CBT			Cursor back tab
						// i dunno what to do, lol
						break;
					case '\x63':
						//DA			Device attributes
						// i dunno what to do, lol
						break;
					case '\x66':
						//HVP			Hor/vertical position
						// i dunno what to do, lol
						break;
					case '\x67':
						//TBC			Tab clear
						// i dunno what to do, lol
						break;
					case '\x68':
						//SM			Set mode
						// i dunno what to do, lol
						break;
					case '\x6c':
						//RM			Reset mode
						// i dunno what to do, lol
						break;
					case '\x6d':
						//SGR			Set graphic rendition
						// i dunno what to do, lol
						break;
					case '\x6e':
						//DSR			Device status
						// i dunno what to do, lol
						break;
					case '\x72':
						//SCR			Scrolling region
						// i dunno what to do, lol
						break;
					case '\x73':
						//SC			Save cursor
						// i dunno what to do, lol
						break;
					case '\x75':
						//RC			Restore cursor
						// i dunno what to do, lol
						break;
					case '\x30':
						//MC/0			Print screen
						// i dunno what to do, lol
						break;
					case '\x34':
						_byte=cf_Data.back();
						cf_Data.pop_back();
						switch (a_byte)
						{
						case '\x69':
							//MC/4			End host print
							// i dunno what to do, lol
							break;
						case '\x71':
							//SM83			Set Mode 8300
							// i dunno what to do, lol
							break;
						}
						break;
					case '\x35':
						_byte=cf_Data.back();
						cf_Data.pop_back();
						switch (a_byte)
						{
						case '\x69':
							//MC/5			Start host print
							// i dunno what to do, lol
							break;
						case '\x71':
							//SM64			Set Mode Ansi
							// i dunno what to do, lol
							break;
						}
						break;
					case '\x31':
						//SM78			Set Mode 7800
						// i dunno what to do, lol
						break;
					case '\x32':
						//SM77			Set Mode 7700
						// i dunno what to do, lol
						break;
					case '\x33':
						//SM73			Set Mode 7300
						// i dunno what to do, lol
						break;
					}
				}
			}
			cf_Data.clear();
			cf_state=0;
			break;
		case 6:
			for (vector<unsigned char>::iterator _i=cf_Data.begin(); _i!=cf_Data.end(); ++_i)
			{
				printf("%c", *_i);
			}
			cf_Data.clear();
			cf_state=0;
			break;
		default:
			break;
		}
	}
};

//ANSI/VT command reference (ASCII) // VT100
//
//Mnemonic		Command					ASCII		HEX
//ALT			Alternate shift			-			-
//BRK			Break					-			-
//CAPS LOCK		Caps lock				-			-
//CTL			Control					-			-
//LCL			Local					-			-
//NUML			Numeric lock			-			-
//SHIFT			Shift					-			-
//ENQ			Enquiry					enq			05
//BEL			Bell					bel			07
//BS			Back space				bs			08
//HT			Horizontal tab			ht			09
//LF			Line feed				lf			0A
//VT			Vertical tab			vt			0B
//FF			Form feed				ff			0C
//CR			Carriage return			cr			0D
//SO			Shift out				so			0E
//SI			Shift in				si			0F
//DLE			Data link escape		dle			10
//CAN			Cancel					can			18
//SS2			Single shift 2			ss2			19
//ESC			Escape					esc			1B
//S7C			Select 7bit ctrls		esc sp F	1B 20 46
//S8C			Select 8bit ctrls		esc sp G	1B 20 47
//DHL			Double height, top		esc # 3		1B 23 33
//DHL			Double height, bottom	esc # 4		1B 23 34
//SWL			Single width/height		esc # 5		1B 23 35
//DWL			Double width			esc # 6		1B 23 36
//ALN			Screen alignment		esc # 8		1B 23 38
//SCS-0			Select G0 set (94)		esc ( x		1B 28 xx
//SCS-1			Select G1 set (94)		esc ) x		1B 29 xx
//SCS-2			Select G2 set (94)		esc * x		1B 2A xx
//SCS-3			Select G3 set (94)		esc + x		1B 2B xx
//SCS-1A		Select G1 set (96)		esc - x		1B 2D xx
//SCS-2A		Select G2 set (96)		esc . x		1B 2E xx
//SCS-3A		Select G3 set (96)		esc / x		1B 2F xx
//SC			Save Cursor State		esc 7		1B 37
//RC			Restore Cursor			esc 8		1B 38
//PAM			App. keypad mode		esc =		1B 3D
//PNM			Num. keypad mode		esc >		1B 3E
//IND			Index					esc D		1B 44
//NEL			Next line				esc E		1B 45
//HTS			Horizontal tab set		esc H		1B 48
//RI			Reverse Index			esc M		1B 4D
//SS2			Single shift 2			esc N		1B 4E
//SS3			Single shift 3			esc O		1B 4F
//AK-0			App keypad 0			esc O p		1B 4F 70
//AK-1			App keypad 1			esc O q		1B 4F 71
//AK-2			App keypad 2			esc O r		1B 4F 72
//AK-3			App keypad 3			esc O s		1B 4F 73
//AK-4			App keypad 4			esc O t		1B 4F 74
//AK-5			App keypad 5			esc O u		1B 4F 75
//AK-6			App keypad 6			esc O v		1B 4F 76
//AK-7			App keypad 7			esc O w		1B 4F 77
//AK-8			App keypad 8			esc O x		1B 4F 78
//AK-9			App keypad 9			esc O y		1B 4F 79
//AK-(-)		App keypad minus		esc O m		1B 4F 6D
//AK-(,)		App keypad comma		esc O l		1B 4F 6C
//AK-(.)		App keypad period		esc O n		1B 4F 6E
//AK-CR			App keypad Enter		esc O M		1B 4F 4D
//PF1			App keypad PF1			esc O P		1B 4F 50
//PF2			App keypad PF2			esc O Q		1B 4F 51
//PF3			App keypad PF3			esc O R		1B 4F 52
//PF4			App keypad PF4			esc O S		1B 4F 53
//DCS			Device control string	esc P		1B 50
//DA			Device attr request		esc Z		1B 5A
//CSI			Control Seq Intro		esc [		1B 5B
//ICH			Insert character		esc [ @		1B 5B 40
//CUU			Cursor up				esc [ A		1B 5B 41
//CUD			Cursor down				esc [ B		1B 5B 42
//CUF			Cursor forward			esc [ C		1B 5B 43
//CUB			Cursor backward			esc [ D		1B 5B 44
//CNL			Cursor Next Line		esc [ E		1B 5B 45
//CPL			Cursor Previous Line	esc [ F		1B 5B 46
//HPA			Horizontal Pos Abs		esc [ G		1B 5B 47
//CUP			Cursor position			esc [ H		1B 5B 48
//ED			Erase data				esc [ J		1B 5B 4A
//EL			Erase line				esc [ K		1B 5B 4B
//IL			Insert line				esc [ L		1B 5B 4C
//DL			Delete line				esc [ M		1B 5B 4D
//EA			Erase area				esc [ O		1B 5B 4F
//DCH			Delete character		esc [ P		1B 5B 50
//SU			Scroll Up				esc [ S		1B 5B 53
//SD			Scroll Down				esc [ T		1B 5B 54
//ECH			Erase character			esc [ X		1B 5B 58
//CBT			Cursor back tab			esc [ Z		1B 5B 5A
//DA			Device attributes		esc [ c		1B 5B 63
//HVP			Hor/vertical position	esc [ f		1B 5B 66
//TBC			Tab clear				esc [ g		1B 5B 67
//SM			Set mode				esc [ h		1B 5B 68
//MC/0			Print screen			esc [ 0 i	1B 5B 30 69
//MC/4			End host print			esc [ 4 i	1B 5B 34 69
//MC/5			Start host print		esc [ 5 i	1B 5B 35 69
//RM			Reset mode				esc [ l		1B 5B 6C
//SGR			Set graphic rendition	esc [ m		1B 5B 6D
//DSR			Device status			esc [ n		1B 5B 6E
//SM78			Set Mode 7800			esc [ 1 q	1B 5B 31 71
//SM77			Set Mode 7700			esc [ 2 q	1B 5B 32 71
//SM73			Set Mode 7300			esc [ 3 q	1B 5B 33 71
//SM83			Set Mode 8300			esc [ 4 q	1B 5B 34 71
//SM64			Set Mode Ansi			esc [ 5 q	1B 5B 35 71
//SCR			Scrolling region		esc [ r		1B 5B 72
//SC			Save cursor				esc [ s		1B 5B 73
//RC			Restore cursor			esc [ u		1B 5B 75
//SGR			Set graphic rendition	esc [ m		1B 5B 6D
//ST			String terminator		esc \		1B 5C
//DMI			Disable manual input	esc `		1B 60
//EMI			Enable manual input		esc b		1B 62
//RIS			Terminal reset			esc c		1B 63
//LS2			Lock shift 2			esc n		1B 6E
//LS3			Lock shift 3			esc o		1B 6F
//LS3R			Lock shift 3 (R)		esc £		1B 7C
//LS2R			Lock shift 2 (R)		esc }		1B 7D
//LS1R			Lock shift 1 (R)		esc ~		1B 7E
//DEL			Delete					del			7F