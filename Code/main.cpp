#include "Head.h"

using namespace std;

string Firmware_Version = "11.0.1";

#define Title "Run 1.0.2156"
#define App "Run"
#define X 960
#define Y 540

string Sign[60][40] = {};// 48 : 27
int Colour[60][40] = {}; //  x : y

string Last_Sign[60][40] = {};// 48 : 27
int Last_Colour[60][40] = {};// 48 : 27

string Name = {};
string List[1000] = {};
int Game_Cnt = 0;

string Save_FilePath;
string Version;

int Patch_Cnt;
struct Pat
{
	string command;
	string patch_name;
	int patch_step = -1;
}Patch[10000];

int Mode = 0; // 1 = 内置  2 = exe
string Save = "main.fbi";

string Path;
bool Loading = true;
const int NR = 10000000;

char ch;
bool Need_Key = false;

double decimal_total = 0, decimal_avl = 0;
#define  GBYTES  1073741824    
#define  MBYTES  1048576    
#define  KBYTES  1024    
#define  DKBYTES 1024.0   

struct Tex
{
	string text;
	DWORD time;
	int mode = 0;
	int x, y;
	int color = 0;
	string geti;
}Text[NR];

bool Run();

void Run_Command(string Save_File_Path);


/*                                        Functions                                                 */
void Refresh()
{
	for (int i = 1; i <= 48; i++)
	{
		for (int j = 1; j <= 27; j++)
		{
			if ((Sign[i][j] != Last_Sign[i][j]) || (Colour[i][j] != Last_Colour[i][j]))
			{
				color(Colour[i][j]);
				gotoxy(i, j - 1);
				cout << Sign[i][j];
			}
		}
	}
	memcpy(Last_Sign, Sign, sizeof(Last_Sign));
	memcpy(Last_Colour, Colour, sizeof(Last_Colour));
}

void Fill(int x1, int y1, int x2, int y2, int colour, string sign)
{
	for (int i = x1; i <= x2; i++)
	{
		for (int j = y1; j <= y2; j++)
		{
			Sign[i][j] = sign;
			Colour[i][j] = colour;
		}
	}
}

void Replace_Short(int x, int y, int colour, string sign)
{
	int j = x;
	for (int i = x; i <= x + sign.length(); i+=2)
	{
		Sign[j][y] = sign[i - x];
		if (i != x + sign.length()) Sign[j][y] += sign[i - x + 1];
		Colour[j][y] = colour;
		j++;
	}
}

void Replace(int x, int y, int colour, string sign)
{
	for (int i = x; i <= x + sign.length(); i++)
	{
		Sign[i][y] = sign[i - x];
		Colour[i][y] = colour;
	}
}

void Set(int x, int y, int colour, string sign)
{
	Sign[x][y] = sign;
	Colour[x][y] = colour;
}

int Random()
{
	return rand() * rand() + rand();
}

void Clear()
{
	Fill(1, 1, 48, 27, 7, "  ");
	Refresh();
}

string Text_Small(string text)
{
	for (int i = 0; i <= text.length() - 1; i++)
	{
		if (text[i] >= 'A' && text[i] <= 'Z') text[i] = text[i] - 'A' + 'a';
	}
	return text;
}

bool Is_Text(string s)
{
	if (s.length() < 1 || s.length() > 2) return false;
	for (int i = 0; i <= s.length() - 1; i++)
	{
		if (s[i] != ' ') return true;
	}
	return false;
}

void Error()
{
	MessageBox(0, "Error!", Title, MB_OK);
	exit(0);
}

void File_Not_found(string name)
{
	string box = "\"" + name + "\" Not Founded!";
	MessageBox(0, box.c_str(), Title, MB_OK);
	exit(0);
}

void Error_Old()
{
	MessageBox(0, "Your software can't run this project\nPlease update your software and try again", Title, MB_OK);
	exit(0);
}

void Play_Music(string nam)
{
	string temp = "play " + Path + "Music\\" + nam;
	mciSendString(temp.c_str(), NULL, 0, NULL);
}
void Stop_Music(string nam)
{
	string temp = "pause " + Path + "Music\\" + nam;
	mciSendString(temp.c_str(), NULL, 0, NULL);
}
void Close_Music(string nam)
{
	string temp = "close " + Path + "Music\\" + nam;
	mciSendString(temp.c_str(), NULL, 0, NULL);
}

void Continue_Music(string nam)
{
	string temp = "resume " + Path + "Music\\" + nam;
	mciSendString(temp.c_str(), NULL, 0, NULL);
}

void Background(string file_name)
{
	string temp = Path + "Back\\" + file_name;
	ifstream in1(temp.c_str(), ios::binary);
	temp = Path + "Back\\" + file_name + ".color";
	ifstream in2(temp.c_str(), ios::binary);

	string getl;
	int temp1;
	for (int j = 1; j <= 27; j++)
	{
		getline(in1, getl);
		temp = "";
		for (int i = 0; i < 96; i++)
		{
			temp += getl[i];
			in2 >> temp1;
			Replace_Short(1, j, temp1, temp);
		}
	}
}

void Paste(int x, int y, string file_name)
{
	string temp = Path + "Paste\\" + file_name;
	ifstream in1(temp.c_str(), ios::binary);
	temp = Path + "Paste\\" + file_name + ".color";
	ifstream in2(temp.c_str(), ios::binary);

	string getl;
	int temp1;
	int j = 0;
	while(!in1.eof())
	{
		getline(in1, getl);
		temp = "";
		for (int i = 0; i < getl.length(); i++)
		{
			temp += getl[i];
			in2 >> temp1;
			Replace_Short(x, y + j, temp1, temp);
		}
		j++;
	}
}

string Space(string s)
{
	string t = "";
	for (int i = 0; i <= s.length() - 1; i++)
	{
		if (s[i] == '\\' && s[i + 1] == '\\')
		{
			t += ' ';
			i++;
		}
		else
		{
			t += s[i];
		}
	}
	return t;
}

/*                                          Effects                                                 */
const int SPEED = 1;
const int WAIT = 30;

void Star()
{
	int c1 = {}, c2 = {}, c3 = {}, c4 = {}, c5 = {}, c6 = {}, c7 = {}, c8 = {}, c9 = {}, c10 = {};
	srand(time(NULL));
	const int x = Random() % 3 + 1;
	int y = Random() % 12 + 8;
	int s = Random() % 4;
	while (y == 14)
	{
		y = Random() % 12 + 8;
	}
	for (int i = x; i <= 55; i += SPEED)
	{
		s = Random() % 5;
		if((i - 9 + s ) > 0) Fill(i - 9 + s, y, i - 1, y, 7, "─");
		Set(i, y, 14, "★");

		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				c1 = Colour[i - 3][j];
				Colour[i - 3][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				c2 = Colour[i - 2][j];
				Colour[i - 2][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				c3 = Colour[i - 1][j];
				Colour[i - 1][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				c4 = Colour[i][j];
				Colour[i][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				c5 = Colour[i + 1][j];
				Colour[i + 1][j] = 14;
				break;
			}
		}
		//
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				c6 = Colour[i - 3][j];
				Colour[i - 3][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				c7 = Colour[i - 2][j];
				Colour[i - 2][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				c8 = Colour[i - 1][j];
				Colour[i - 1][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				c9 = Colour[i][j];
				Colour[i][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				c10 = Colour[i + 1][j];
				Colour[i + 1][j] = 14;
				break;
			}
		}
		
		Refresh();
		Sleep(WAIT);
		
		Fill(i - 9 + s, y, i, y, 7, "  ");
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				Colour[i - 3][j] = c1;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				Colour[i - 2][j] = c2;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				Colour[i - 1][j] = c3;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				Colour[i][j] = c4;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i + 1][j]))
			{
				Colour[i + 1][j] = c5;
				break;
			}
		}
		//
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				Colour[i - 3][j] = c6;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				Colour[i - 2][j] = c7;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				Colour[i - 1][j] = c8;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				Colour[i][j] = c9;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				Colour[i + 1][j] = c10;
				break;
			}
		}
	}
	Refresh();
}
void Star_Middle()
{
	int c1 = {}, c2 = {}, c3 = {}, c4 = {}, c5 = {}, c6 = {}, c7 = {}, c8 = {}, c9 = {}, c10 = {};
	srand(time(NULL));
	const int x = Random() % 3 + 1, y = 14;
	int s = Random() % 4;
	
	for (int i = x; i <= 55; i += SPEED)
	{
		s = Random() % 5;
		if ((i - 9 + s) > 0) Fill(i - 9 + s, y, i - 1, y, 7, "─");
		Set(i, y, 14, "★");

		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				c1 = Colour[i - 3][j];
				Colour[i - 3][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				c2 = Colour[i - 2][j];
				Colour[i - 2][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				c3 = Colour[i - 1][j];
				Colour[i - 1][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				c4 = Colour[i][j];
				Colour[i][j] = 14;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				c5 = Colour[i + 1][j];
				Colour[i + 1][j] = 14;
				break;
			}
		}
		//
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				c6 = Colour[i - 3][j];
				Colour[i - 3][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				c7 = Colour[i - 2][j];
				Colour[i - 2][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				c8 = Colour[i - 1][j];
				Colour[i - 1][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				c9 = Colour[i][j];
				Colour[i][j] = 14;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				c10 = Colour[i + 1][j];
				Colour[i + 1][j] = 14;
				break;
			}
		}

		Refresh();
		Sleep(WAIT);

		Fill(i - 9 + s, y, i, y, 7, "  ");
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				Colour[i - 3][j] = c1;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				Colour[i - 2][j] = c2;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				Colour[i - 1][j] = c3;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				Colour[i][j] = c4;
				break;
			}
		}
		for (int j = y + 1; j <= 27; j++)
		{
			if (Is_Text(Sign[i + 1][j]))
			{
				Colour[i + 1][j] = c5;
				break;
			}
		}
		//
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 3][j]) == true && abs(y - j) > 3)
			{
				Colour[i - 3][j] = c6;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 2][j]) == true && abs(y - j) > 2)
			{
				Colour[i - 2][j] = c7;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i - 1][j]) == true)
			{
				Colour[i - 1][j] = c8;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i][j]) == true)
			{
				Colour[i][j] = c9;
				break;
			}
		}
		for (int j = y - 1; j >= 1; j--)
		{
			if (Is_Text(Sign[i + 1][j]) == true)
			{
				Colour[i + 1][j] = c10;
				break;
			}
		}
	}
	Refresh();
}

void Load_Circle()
{
	int i = 10 + Random() % 3;
	Set(46, 26, 8, "│");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "╱");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "─");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "╲");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "│");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "╱");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "─");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╲");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "│");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╱");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "─");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╲");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "│");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╱");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "─");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╲");
	Refresh();
	Sleep(50);

	while (i > 0)
	{
		Set(46, 26, 15, "│");
		Refresh();
		Sleep(50);
		Set(46, 26, 15, "╱");
		Refresh();
		Sleep(50);
		Set(46, 26, 15, "─");
		Refresh();
		Sleep(50);
		Set(46, 26, 15, "╲");
		Refresh();
		Sleep(50);
		i--;
	}
	Set(46, 26, 7, "│");
	Refresh();
	Sleep(50);
	Set(46, 26, 7, "╱");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "─");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "╲");
	Refresh();
	Sleep(50);
	Set(46, 26, 8, "  ");
	Refresh();

	Sleep(1000);
}

/*                                          Thread                                                  */
void Load_List()
{
	// 列表
	ifstream in("user\\sysdata\\game.list", ios::binary);
	while (in)
	{
		Game_Cnt++;
		getline(in, List[Game_Cnt]);
	}
	Game_Cnt--;

	// 内存
	string memory_info;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
	if (GlobalMemoryStatusEx(&statusex))
	{

		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		if (remain_avl > 0)
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;

		decimal_total += (double)total;
		decimal_avl += (double)avl;

	}

	// 结束
	Loading = false;
}

void Load_Program()
{
	Path = "user\\nand\\" + Name + "\\";
	string temp;
	temp = Path + "Config";
	ifstream in1(temp.c_str(), ios::binary);
	in1 >> temp;
	if (temp > Firmware_Version) Error_Old();
	in1 >> temp;
	if (temp == "Command" || temp == "1") Mode = 1;
	else Mode = 2;
	in1 >> Version;


	if(!in1.eof()) in1 >> Patch_Cnt;

	Save = "main.fbi";
	temp = Path + "SaveData";
	ifstream in2(temp.c_str(), ios::binary);
	if (in2) in2 >> Save;

	temp = Title;
	temp = temp + " - " + Name + " v" + Version + " (32bit)";
	SetConsoleTitle(TEXT(temp.c_str()));
	
	for (int i = 1; i <= Patch_Cnt; i++)
	{
		stringstream ss;
		string temp1;
		ss << i;
		ss >> temp1;
		temp = Path + "Patch\\" + temp1;
		ifstream in3(temp.c_str(), ios::binary);
		while (!in3.eof())
		{
			in3 >> Patch[i].patch_name >> Patch[i].patch_step;
			getline(in3, Patch[i].command);
		}
	}

	Loading = false;
}

void get_key()
{
	ch = getch();
}

void Drop_Key()
{
	while (1)
	{
		if (Need_Key == false) getch();
		Sleep(10);
	}
}

/*                                          Program                                                 */
void initialization()
{
	const int x = GetSystemMetrics(SM_CXSCREEN);
	const int y = GetSystemMetrics(SM_CYSCREEN);

	SetConsoleTitle(TEXT(Title));
	system("mode con cols=101 lines=27");
	HWND hWnd = FindWindow(NULL, Title);
	MoveWindow(hWnd, (x * 3 - X * 2) / 8, (y - Y) / 2, X, Y, true);
	HindCursor();

	memset(Colour, 7, sizeof(Colour));
	
	Sleep(500);
	Load_Circle();

	Replace(21, 14, 8, "Loading");
	Refresh();
	Sleep(100);

	Replace(21, 14, 7, "Loading");
	Refresh();
	thread LOADLIST(Load_List);
	LOADLIST.join();
	Sleep(1000);

	while (Loading == true)
	{
		Star();
	}
	Star_Middle();
	
	Sleep(750);
	Replace(17, 14, 8, "Present By DKM");
	Refresh();
	Sleep(85);
	Replace(17, 14, 7, "Present By DKM");
	Refresh();
	Sleep(85);
	Replace(17, 14, 15, "Present By DKM");
	Refresh();
	Sleep(3000);
	Star_Middle();
	Sleep(1000);
}

void finish()
{
	Clear();
	//MessageBox(0, "程序结束", Title, MB_OK);
	color(7);
}

string Choose()
{
	Sleep(1500);
	SetConsoleTitle(TEXT(Title));
	Clear();
	Sleep(1000);
	int choose = {};
	int Point = 1;
	int a = 1, b = min(3, Game_Cnt), last = -1;

	while (1)
	{
		if (b - a == 0)
		{
			if (last > 0)
			{
				Fill(2, 12, 41, 16, 7, " ");
				if (last > 1)
				{
					Fill(2, 19, 41, 26, 7, " ");
				}
			}

			Fill(3, 5, 40, 5, 7, "─");
			Fill(3, 9, 40, 9, 7, "─");
			Fill(2, 6, 2, 9, 7, "│");
			Fill(41, 6, 41, 9, 7, "│");
			Set(2, 5, 7, "┌");
			Set(41, 5, 7, "┐");
			Set(2, 9, 7, "└");
			Set(41, 9, 7, "┘");
			Point = 1;
		}
		if (b - a == 1)
		{
			if (last > 1)
			{
				Fill(2, 19, 41, 26, 7, " ");
			}
			for (int i = 5; i <= 12; i += 7)
			{
				Fill(3, i, 40, i, 7, "─"); //i = 5
				Fill(3, i + 4, 40, i + 4, 7, "─");
				Fill(2, i + 1, 2, i + 3, 7, "│");
				Fill(41, i + 1, 41, i + 3, 7, "│");
				Set(2, i, 7, "┌");
				Set(41, i, 7, "┐");
				Set(2, i + 4, 7, "└");
				Set(41, i + 4, 7, "┘");
			}
			if (Point == 3) Point = 2;
			if (ch == 80) Point = 2;
			if (ch == 72) Point = 1;
		}
		if (b - a == 2)
		{
			for (int i = 5; i <= 19; i += 7)
			{
				Fill(3, i, 40, i, 7, "─"); //i = 5
				Fill(3, i + 4, 40, i + 4, 7, "─");
				Fill(2, i + 1, 2, i + 3, 7, "│");
				Fill(41, i + 1, 41, i + 3, 7, "│");
				Set(2, i, 7, "┌");
				Set(41, i, 7, "┐");
				Set(2, i + 4, 7, "└");
				Set(41, i + 4, 7, "┘");
			}
			
			if (ch == 80 && Point < 3) Point++;
			if (ch == 72 && Point > 1) Point--;
		}
		last = b - a;
		if (Point == 1)
		{
			Set(46, 7, 15, "<");
			Set(46, 14, 7, " ");
			Set(46, 21, 7, " ");
		}
		if (Point == 2)
		{
			Set(46, 7, 7, " ");
			Set(46, 14, 15, "<");
			Set(46, 21, 7, " ");
		}
		if (Point == 3)
		{
			Set(46, 7, 7, " ");
			Set(46, 14, 7, " ");
			Set(46, 21, 15, "<");
		}

		Fill(4, 7, 40, 7, 7, "  ");
		Fill(4, 14, 40, 14, 7, "  ");
		Fill(4, 21, 40, 21, 7, "  ");
		Refresh();

		if (b - a == 0)
		{
			Replace_Short(4, 7, 15, List[a]);
		}
		if (b - a == 1)
		{
			Replace_Short(4, 7, 15, List[a]);
			Replace_Short(4, 14, 15, List[a + 1]);
		}
		if (b - a == 2)
		{
			Replace_Short(4, 7, 15, List[a]);
			Replace_Short(4, 14, 15, List[a + 1]);
			Replace_Short(4, 21, 15, List[a + 2]);
		}

		Refresh();
		Need_Key = true;
		get_key();
		Need_Key = false;

		if (ch == 32 || ch == 13) break;
		if (Game_Cnt > 3)
		{
			if (ch == 77)// ->
			{
				if (a + 3 > Game_Cnt)
				{
					a = 1;
					b = 3;
				}
				else
				{
					a += 3;
					b = min(b + 3, Game_Cnt);
				}
			}
			if (ch == 75)// <-
			{
				if (a - 3 < 1)
				{
					a = Game_Cnt - Game_Cnt % 3 + 1;
					b = Game_Cnt;
				}
				else
				{
					a -= 3;
					b = a + 2;
				}
			}
			for (int i = 1; i <= 48; i++)
			{
				for (int j = 1; j <= 27; j++)
				{
					Sign[i][j] = "  ";
					Colour[i][j] = 7;
				}
			}
		}
	}

	Fill(1, 1, 48, 27, 7, " ");
	Refresh();
	choose = a + Point - 1;
	return List[choose];
}

void Command_run(string command, string Save_File_Path)
{
	string temp, getl = command;
	ifstream in("user\\sysdata\\buffer.fbi");
	string temp3;
	cin >> temp3;
	if (getl[0] == '/' && getl[1] == '/') return;
	if (!getl.empty())
	{
		if (getl == "playsound")
		{
			in >> temp;
			Play_Music(temp);
		}
		else if (getl == "pausesound")
		{
			in >> temp;
			Stop_Music(temp);
		}
		else if (getl == "closesound")
		{
			in >> temp;
			Close_Music(temp);
		}
		else if (getl == "continuesound")
		{
			in >> temp;
			Continue_Music(temp);
		}
		else if (getl == "clear")
		{
			Clear();
		}
		else if (getl == "sleep")
		{
			DWORD time;
			in >> time;
			Sleep(time);
		}
		else if (getl == "show")
		{
			Refresh();
		}
		else if (getl == "actionbar")
		{
			int x, y, colour;
			string text;
			in >> x >> y >> colour >> text;
			Replace(x, y, colour, Space(text));
		}
		else if (getl == "replace")
		{
			int x, y, colour;
			string text;
			in >> x >> y >> colour >> text;
			Replace_Short(x, y, colour, Space(text));
		}
		else if (getl == "text")
		{
			int x1, y1, x2, y2, colour;
			DWORD delay;
			string text, sound;
			in >> x1 >> y1 >> x2 >> y2 >> colour >> delay >> sound >> text;

			int k = 0;
			text = Space(text);
			bool flag_text = false;
			for (int j = y1; j <= y2; j++)
			{
				temp = "";
				for (int i = x1; i <= x2; i++)
				{
					if (k <= text.length())
					{
						temp += text[k];
					}
					else
					{
						flag_text = true;
						break;
					}
					Replace_Short(x1, j, colour, temp);
					Refresh();
					if (sound != "NULL") Play_Music(sound);
					Sleep(delay);
					k++;
				}
				if (flag_text == true) break;
			}
		}
		else if (getl == "jump")
		{
			string text;
			in >> text;
			Run_Command(text);
		}
		else if (getl == "wait")
		{
			int aci;
			in >> aci;
			int ch_num = 1e7;
			Need_Key = true;
			while (ch_num != aci)
			{;
				get_key();
				ch_num = ch;
			}
			Need_Key = false;
		}
		else if (getl == "save")
		{
			string temp = Path + "SaveData";
			ofstream out1(temp.c_str(), ios::binary);
			out1 << Save_File_Path;
		}
		else if (getl == "background")
		{
			in >> temp;
			Background(temp);
		}
		else if (getl == "paste")
		{
			int x, y;
			in >> x >> y >> temp;
			Paste(x, y, temp);
		}
		else if (getl == "fill")
		{
			int x1, y1, x2, y2, colour;
			string text;
			in >> x1 >> y1 >> x2 >> y2 >> colour >> text;
			text = Space(text);
			Fill(x1, y1, x2, y2, colour, text);
		}
		else if (getl == "choose")
		{
			int x;
			in >> x;

		}
		else if (getl == "quit")
		{
			while (1)
			{
				Name = Choose();
				if (Run() == false)
				{
					Error();
					exit(0);
				}
			}
		}
	}
}

void Run_Command(string Save_File_Path)
{
	Save_FilePath = Save_File_Path;
	string temp = Path + Save_File_Path;
	ifstream in(temp.c_str(), ios::binary);
	//if(!in) File_Not_found(temp);

	int step = 0;

	string getl;
	while (in >> getl)
	{
		getl = Text_Small(getl);
		step++;
		for (int i = 1; i <= Patch_Cnt; i++)
		{
			if(Patch[i].patch_name == Save_File_Path && step == Patch[i].patch_step)
			{
				ofstream out1("user\\sysdata\\buffer.fbi", ios::binary);
				out1 << Patch[i].command;
				
			}
		}
		//MessageBox(0, getl.c_str(), Title, MB_OK);
		if (getl[0] == '/' && getl[1] == '/') continue;
		if (!getl.empty())
		{
			if (getl == "playsound")
			{
				in >> temp;
				Play_Music(temp);
			}
			else if (getl == "pausesound")
			{
				in >> temp;
				Stop_Music(temp);
			}
			else if (getl == "closesound")
			{
				in >> temp;
				Close_Music(temp);
			}
			else if (getl == "continuesound")
			{
				in >> temp;
				Continue_Music(temp);
			}
			else if (getl == "clear")
			{
				Clear();
			}
			else if (getl == "sleep")
			{
				DWORD time;
				in >> time;
				Sleep(time);
			}
			else if (getl == "show")
			{
				Refresh();
			}
			else if (getl == "actionbar")
			{
				int x, y, colour;
				string text;
				in >> x >> y >> colour >> text;
				Replace(x, y, colour, Space(text));
			}
			else if (getl == "replace")
			{
				int x, y, colour;
				string text;
				in >> x >> y >> colour >> text;
				Replace_Short(x, y, colour, Space(text));
			}
			else if (getl == "text")
			{
				int x1, y1, x2, y2, colour;
				DWORD delay;
				string text, sound;
				in >> x1 >> y1 >> x2 >> y2 >> colour >> delay >> sound >> text;
				
				int k = 0;
				text = Space(text);
				bool flag_text = false;
				for (int j = y1; j <= y2; j++)
				{
					temp = "";
					for (int i = x1; i <= x2; i++)
					{
						if (k <= text.length())
						{
							temp += text[k];
						}
						else
						{
							flag_text = true;
							break;
						}
						Replace_Short(x1, j, colour, temp);
						Refresh();
						if (sound != "NULL") Play_Music(sound);
						Sleep(delay);
						k++;
					}
					if (flag_text == true) break;
				}
			}
			else if (getl == "jump")
			{
				string text;
				in >> text;
				Run_Command(text);
			}
			else if (getl == "wait")
			{
				int aci;
				in >> aci;
				int ch_num = 1e7;
				Need_Key = true;
				while(ch_num != aci)
				{
					get_key();
					ch_num = ch;
				}
				Need_Key = false;
			}
			else if (getl == "save")
			{
				string temp = Path + "SaveData";
				ofstream out1(temp.c_str(), ios::binary);
				out1 << Save_File_Path;
			}
			else if (getl == "background")
			{
				in >> temp;
				Background(temp);
			}
			else if (getl == "paste")
			{
				int x, y;
				in >> x >> y >> temp;
				Paste(x, y, temp);
			}
			else if (getl == "fill")
			{
				int x1, y1, x2, y2, colour;
				string text;
				in >> x1 >> y1 >> x2 >> y2 >> colour >> text;
				text = Space(text);
				Fill(x1, y1, x2, y2, colour, text);
			}
			else if (getl == "choose")
			{
				int x;
				in >> x;

			}
			else if (getl == "quit")
			{
				while (1)
				{
					Name = Choose();
					if (Run() == false)
					{
						Error();
						exit(0);
					}
				}
			}
		}
	}
}

bool Run()
{
	Clear();
	Sleep(3000);
	// 加载
	Replace(40, 23, 8, App);
	Replace_Short(3, 3, 8, "Doncama TM");
	Fill(2, 2, 9, 2, 8, "─");
	Fill(3, 4, 9, 4, 8, "─");
	Set(1, 3, 8, " │");
	Set(10, 3, 8, " │");
	Set(2, 2, 8, "╭");
	Set(10, 2, 8, "╮");
	Set(1, 4, 8, " ╰");
	Set(10, 4, 8, " ╯");
	Fill(9, 13, 39, 13, 8, "─");
	Fill(9, 16, 39, 16, 8, "─");
	Fill(8, 14, 8, 15, 8, "│");
	Fill(40, 14, 40, 15, 8, "│");
	Set(8, 13, 8, "┌");
	Set(40, 13, 8, "┐");
	Set(8, 16, 8, "└");
	Set(40, 16, 8, "┘");
	Replace(20, 11, 8, "Launching");
	Refresh();
	Sleep(85);
	Replace(40, 23, 7, App);
	Replace_Short(3, 3, 7, "Doncama TM");
	Fill(2, 2, 9, 2, 7, "─");
	Fill(3, 4, 9, 4, 7, "─");
	Set(1, 3, 7, " │");
	Set(10, 3, 7, " │");
	Set(2, 2, 7, "╭");
	Set(10, 2, 7, "╮");
	Set(1, 4, 7, " ╰");
	Set(10, 4, 7, " ╯");
	Fill(9, 13, 39, 13, 7, "─");
	Fill(9, 16, 39, 16, 7, "─");
	Fill(8, 14, 8, 15, 7, "│");
	Fill(40, 14, 40, 15, 7, "│");
	Set(8, 13, 7, "┌");
	Set(40, 13, 7, "┐");
	Set(8, 16, 7, "└");
	Set(40, 16, 7, "┘");
	Replace(20, 11, 7, "Launching");
	Refresh();
	Sleep(85);
	Replace(40, 23, 15, App);
	Replace_Short(3, 3, 15, "Doncama TM");
	Fill(2, 2, 9, 2, 15, "─");
	Fill(3, 4, 9, 4, 15, "─");
	Set(1, 3, 15, " │");
	Set(10, 3, 15, " │");
	Set(2, 2, 15, "╭");
	Set(10, 2, 15, "╮");
	Set(1, 4, 15, " ╰");
	Set(10, 4, 15, " ╯");
	Fill(9, 13, 39, 13, 15, "─");
	Fill(9, 16, 39, 16, 15, "─");
	Fill(8, 14, 8, 15, 15, "│");
	Fill(40, 14, 40, 15, 15, "│");
	Set(8, 13, 15, "┌");
	Set(40, 13, 15, "┐");
	Set(8, 16, 15, "└");
	Set(40, 16, 15, "┘");
	Fill(9, 14, 39, 15, 187, "  ");
	Replace(20, 11, 15, "Launching");
	Refresh();
	Sleep(500);
	Loading = true;
	thread LOADPRO(Load_Program);
	//LOADPRO.join();
	int j = Random() % 2 + 1;
	int ts = 0, sp = 3;
	while (Loading == true || j >= 0)
	{
		for (int i = -12; i <= 31; i++)
		{
			Fill(9, 14, 39, 15, 187, "  ");
			Fill((i + 9 <= 8) ? 9 : (i + 9), 14, min(39, i + 9 + 12), 15, 204, "  ");

			if (ts == 0)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			if (ts == sp)
			{
				Replace(40, 22, 15, "R  ");
				Replace(40, 23, 15, " un");
			}
			if (ts == sp * 2)
			{
				Replace(40, 22, 15, " u ");
				Replace(40, 23, 15, "R n");
			}
			if (ts == sp * 3)
			{
				Replace(40, 22, 15, "  n");
				Replace(40, 23, 15, "Ru ");
			}
			if (ts == sp * 4)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			Refresh();
			Sleep(40);
			ts = (ts + 1) % (sp * 5 + 1);
		}
		for (int i = 1; i <= 10; i++)
		{
			if (ts == 0)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			if (ts == sp)
			{
				Replace(40, 22, 15, "R  ");
				Replace(40, 23, 15, " un");
			}
			if (ts == sp * 2)
			{
				Replace(40, 22, 15, " u ");
				Replace(40, 23, 15, "R n");
			}
			if (ts == sp * 3)
			{
				Replace(40, 22, 15, "  n");
				Replace(40, 23, 15, "Ru ");
			}
			if (ts == sp * 4)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			Refresh();
			Sleep(40);
			ts = (ts + 1) % (sp * 5 + 1);
		}
		j--;
	}
	if (ts >= sp)
	{
		for (; ts <= sp * 5 - 1; )
		{
			if (ts == 0)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			if (ts == sp)
			{
				Replace(40, 22, 15, "R  ");
				Replace(40, 23, 15, " un");
			}
			if (ts == sp * 2)
			{
				Replace(40, 22, 15, " u ");
				Replace(40, 23, 15, "R n");
			}
			if (ts == sp * 3)
			{
				Replace(40, 22, 15, "  n");
				Replace(40, 23, 15, "Ru ");
			}
			if (ts == sp * 4)
			{
				Replace(40, 22, 15, "   ");
				Replace(40, 23, 15, "Run");
			}
			Refresh();
			Sleep(40);
			ts = (ts + 1) % (sp * 5 + 1);
		}
	}
	Replace(40, 22, 15, "   ");
	Replace(40, 23, 15, "Run");

	Replace(40, 23, 7, App);
	Replace_Short(3, 3, 7, "Doncama TM");
	Fill(2, 2, 9, 2, 7, "─");
	Fill(3, 4, 9, 4, 7, "─");
	Set(1, 3, 7, " │");
	Set(10, 3, 7, " │");
	Set(2, 2, 7, "╭");
	Set(10, 2, 7, "╮");
	Set(1, 4, 7, " ╰");
	Set(10, 4, 7, " ╯");
	Fill(9, 13, 39, 13, 7, "─");
	Fill(9, 16, 39, 16, 7, "─");
	Fill(8, 14, 8, 15, 7, "│");
	Fill(40, 14, 40, 15, 7, "│");
	Set(8, 13, 7, "┌");
	Set(40, 13, 7, "┐");
	Set(8, 16, 7, "└");
	Set(40, 16, 7, "┘");
	Replace(20, 11, 7, "Launching");
	Refresh();
	Sleep(85);
	Replace(40, 23, 8, App);
	Replace_Short(3, 3, 8, "Doncama TM");
	Fill(2, 2, 9, 2, 8, "─");
	Fill(3, 4, 9, 4, 8, "─");
	Set(1, 3, 8, " │");
	Set(10, 3, 8, " │");
	Set(2, 2, 8, "╭");
	Set(10, 2, 8, "╮");
	Set(1, 4, 8, " ╰");
	Set(10, 4, 8, " ╯");
	Fill(9, 13, 39, 13, 8, "─");
	Fill(9, 16, 39, 16, 8, "─");
	Fill(8, 14, 8, 15, 8, "│");
	Fill(40, 14, 40, 15, 8, "│");
	Set(8, 13, 8, "┌");
	Set(40, 13, 8, "┐");
	Set(8, 16, 8, "└");
	Set(40, 16, 8, "┘");
	Replace(20, 11, 8, "Launching");
	Refresh();
	Sleep(85);
	Clear();
	
	if (Mode == 1)
	{
		Run_Command(Save);
	}
	SetConsoleTitle(TEXT(Title));
	return true;
}


int main()
{
	//thread DROPKEY(Drop_Key);
	//DROPKEY.join();

	initialization();

	if (Game_Cnt <= 0)
	{
		MessageBox(0, "No File Founded!", Title, MB_OK);
		finish();
		return 0;
	}

	while (1)
	{
		Name = Choose();
		
		if (Run() == false)
		{
			Error();
			break;
		}
	}

	finish();

	return 0;
}