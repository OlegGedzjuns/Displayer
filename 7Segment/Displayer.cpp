#include "Displayer.h"
#include "DisplayEncoding.h"

void DisplayerClass::Init(const short segPin_[8], const short dispCnt_, const short dispPin_[])
{
	// Creating display containers, saving segment and display pins 
	dispCnt = dispCnt_;
	display = new Display[dispCnt];
	memcpy(segPin, segPin_, sizeof(segPin_) * sizeof(short));

	for (int seg = 0; seg < 8; ++seg)
	{
		pinMode(segPin[seg], OUTPUT);
		digitalWrite(segPin[seg], LOW);
	}
	for (int disp = 0; disp < dispCnt; ++disp)
	{
		display[disp].pin = dispPin_[disp];
		pinMode(display[disp].pin, OUTPUT);
		digitalWrite(display[disp].pin, HIGH);
	}
	Show("xxx");
}

void DisplayerClass::Show(const char c[] = "")
{
	for (int dispPos = 0, strPos = 0; c[strPos] != '\0' && dispPos < 3; ++strPos)
	{
		if (c[strPos] == '.')
			continue;
		GetSymbol(c[strPos], display[dispPos].segment, c[strPos + 1] == '.');
		++dispPos;
	}
}

void DisplayerClass::Show(int num)
{
	if (num <= 0)
	{
		Show("000");
		return;
	}

	char cnum[] = { 1, 1, 1, 0 };
	for (int i = 2; i >= 0; --i)
	{
		if (num > 0)
		{
			cnum[i] = num % 10 + '0';
			num /= 10;
		}
	}

	Show(cnum);
}

void DisplayerClass::Show(float num)
{
	if (num <= 0)
	{
		Show("000.");
		return;
	}

	char cnum[] = { 1, 1, 1, '.', 0 };
	int len = 0;
	for (int i = num; i > 0 && len < 3; i /= 10, ++len);
	if (len < 3)
	{
		cnum[4] = (int)(num * 10) % 10 + '0';
		cnum[3] = '.';
	}
	else
		Show((int)num);
	for (int i = 1; i >= 0; --i)
	{
		if (num > 0)
		{
			cnum[i] = (int)num % 10 + '0';
			num /= 10;
		}
	}
}

void DisplayerClass::RefreshStart()
{
	// light up display 
	if(refresh)
	{
		for (int seg = 0; seg < 8; ++seg)
		{
			digitalWrite(segPin[seg], display[refreshableDisp].segment[seg]);
		}
		digitalWrite(display[refreshableDisp].pin, LOW);
		refresh = false;
		startTime = millis();
	}
}

void DisplayerClass::RefreshEnd()
{
	// Change display 
	if (!refresh && millis() - startTime >= 5)
	{
		digitalWrite(display[refreshableDisp].pin, HIGH);
		refreshableDisp = (refreshableDisp + 1) % dispCnt;
		refresh = true;
	}
}

DisplayerClass Displayer;