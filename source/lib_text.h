/*****************************\
* 	text.h
*	By staringmonkey
*	Last modified on 12/31/01
\*****************************/

#ifndef TEXT_H
#define TEXT_H

////////////////////////////////////Necessary Includes///////////////////////////
#include <string.h>
#include <stdio.h>
#include "lib_background.h"
#include "letters256.h"

/////////////////////////////////////Memory Divisions/////////////////////////////
u16 *BG0 = (u16 *)0x6004000; //Memory for BG0's map
u16 *BG1 = (u16 *)0x6000000; //Memory for BG1's map
u16 *BG2 = (u16 *)0x6000000; //Memory for BG2's map
u16 *BG3 = (u16 *)0x6000000; //Memory for BG3's map

/////////////////////////////////////////Defines/////////////////////////////////
#define SPACE 0x0

//////////////////////////////////////Function Prototypes////////////////////////
void WriteText(u16 textXLeft, u16 textXRight, u16 textYTop, u16 textYBottom, char *message, u8 screenMem, u8 xFlip, u8 yFlip);
void WriteNum(u16 textXLeft, u16 textXRight, u16 textYTop, u16 textYBottom, s16 number, u8 layer, u8 xFlip, u8 yFlip);

///////////////////WriteText/////////////////
void WriteText(u16 textXLeft, u16 textXRight, u16 textYTop, u16 textYBottom, char *message, u8 screenMem, u8 xFlip, u8 yFlip)
{
	u16 currentLetter;  //Current letter
	u16 i, j = 0;		//Looping variables
	u16 xOff = 0;		//X Column
	u16 yOff = 0;		//Y Row
	u16 length = 0;		//Array length
	u16 findSpace;		//Used to find last space before line-end
	u16 newString[600]; //Hex version of the string
	u16 test;
	u16 *destination; //What layer to draw to

	//Switch to correct ScreenMem block
	switch (screenMem)
	{
	case 0:
		destination = ScreenMem0;
		break;
	case 1:
		destination = ScreenMem1;
		break;
	case 2:
		destination = ScreenMem2;
		break;
	case 3:
		destination = ScreenMem3;
		break;
	case 4:
		destination = ScreenMem4;
		break;
	case 5:
		destination = ScreenMem5;
		break;
	case 6:
		destination = ScreenMem6;
		break;
	case 7:
		destination = ScreenMem7;
		break;
	case 8:
		destination = ScreenMem8;
		break;
	case 9:
		destination = ScreenMem9;
		break;
	case 10:
		destination = ScreenMem10;
		break;
	case 11:
		destination = ScreenMem11;
		break;
	case 12:
		destination = ScreenMem12;
		break;
	case 13:
		destination = ScreenMem13;
		break;
	case 14:
		destination = ScreenMem14;
		break;
	case 15:
		destination = ScreenMem15;
		break;
	case 16:
		destination = ScreenMem16;
		break;
	case 17:
		destination = ScreenMem17;
		break;
	case 18:
		destination = ScreenMem18;
		break;
	case 19:
		destination = ScreenMem19;
		break;
	case 20:
		destination = ScreenMem20;
		break;
	case 21:
		destination = ScreenMem21;
		break;
	case 22:
		destination = ScreenMem22;
		break;
	case 23:
		destination = ScreenMem23;
		break;
	case 24:
		destination = ScreenMem24;
		break;
	case 25:
		destination = ScreenMem25;
		break;
	case 26:
		destination = ScreenMem26;
		break;
	case 27:
		destination = ScreenMem27;
		break;
	case 28:
		destination = ScreenMem28;
		break;
	case 29:
		destination = ScreenMem29;
		break;
	case 30:
		destination = ScreenMem30;
		break;
	case 31:
		destination = ScreenMem31;
		break;
	}

	while (message[j] != NULL)
	{
		switch (message[j])
		{
		case 'A':
			newString[j] = 0x01;
			break;
		case 'B':
			newString[j] = 0x02;
			break;
		case 'C':
			newString[j] = 0x03;
			break;
		case 'D':
			newString[j] = 0x04;
			break;
		case 'E':
			newString[j] = 0x05;
			break;
		case 'F':
			newString[j] = 0x06;
			break;
		case 'G':
			newString[j] = 0x07;
			break;
		case 'H':
			newString[j] = 0x08;
			break;
		case 'I':
			newString[j] = 0x09;
			break;
		case 'J':
			newString[j] = 0x0A;
			break;
		case 'K':
			newString[j] = 0x0B;
			break;
		case 'L':
			newString[j] = 0x0C;
			break;
		case 'M':
			newString[j] = 0x0D;
			break;
		case 'N':
			newString[j] = 0x0E;
			break;
		case 'O':
			newString[j] = 0x0F;
			break;
		case 'P':
			newString[j] = 0x10;
			break;
		case 'Q':
			newString[j] = 0x11;
			break;
		case 'R':
			newString[j] = 0x12;
			break;
		case 'S':
			newString[j] = 0x13;
			break;
		case 'T':
			newString[j] = 0x14;
			break;
		case 'U':
			newString[j] = 0x15;
			break;
		case 'V':
			newString[j] = 0x16;
			break;
		case 'W':
			newString[j] = 0x17;
			break;
		case 'X':
			newString[j] = 0x18;
			break;
		case 'Y':
			newString[j] = 0x19;
			break;
		case 'Z':
			newString[j] = 0x1A;
			break;
		case 'a':
			newString[j] = 0x1B;
			break;
		case 'b':
			newString[j] = 0x1C;
			break;
		case 'c':
			newString[j] = 0x1D;
			break;
		case 'd':
			newString[j] = 0x1E;
			break;
		case 'e':
			newString[j] = 0x1F;
			break;
		case 'f':
			newString[j] = 0x20;
			break;
		case 'g':
			newString[j] = 0x21;
			break;
		case 'h':
			newString[j] = 0x22;
			break;
		case 'i':
			newString[j] = 0x23;
			break;
		case 'j':
			newString[j] = 0x24;
			break;
		case 'k':
			newString[j] = 0x25;
			break;
		case 'l':
			newString[j] = 0x26;
			break;
		case 'm':
			newString[j] = 0x27;
			break;
		case 'n':
			newString[j] = 0x28;
			break;
		case 'o':
			newString[j] = 0x29;
			break;
		case 'p':
			newString[j] = 0x2A;
			break;
		case 'q':
			newString[j] = 0x2B;
			break;
		case 'r':
			newString[j] = 0x2C;
			break;
		case 's':
			newString[j] = 0x2D;
			break;
		case 't':
			newString[j] = 0x2E;
			break;
		case 'u':
			newString[j] = 0x2F;
			break;
		case 'v':
			newString[j] = 0x30;
			break;
		case 'w':
			newString[j] = 0x31;
			break;
		case 'x':
			newString[j] = 0x32;
			break;
		case 'y':
			newString[j] = 0x33;
			break;
		case 'z':
			newString[j] = 0x34;
			break;
		case '0':
			newString[j] = 0x35;
			break;
		case '1':
			newString[j] = 0x36;
			break;
		case '2':
			newString[j] = 0x37;
			break;
		case '3':
			newString[j] = 0x38;
			break;
		case '4':
			newString[j] = 0x39;
			break;
		case '5':
			newString[j] = 0x3A;
			break;
		case '6':
			newString[j] = 0x3B;
			break;
		case '7':
			newString[j] = 0x3C;
			break;
		case '8':
			newString[j] = 0x3D;
			break;
		case '9':
			newString[j] = 0x3E;
			break;
		case '!':
			newString[j] = 0x3F;
			break;
		//case '"': newString[j] = 0x40; break;
		case '#':
			newString[j] = 0x41;
			break;
		case '$':
			newString[j] = 0x42;
			break;
		case '%':
			newString[j] = 0x43;
			break;
		case '&':
			newString[j] = 0x44;
			break;
		//case ''': newString[j] = 0x45; break;
		case '(':
			newString[j] = 0x46;
			break;
		case ')':
			newString[j] = 0x47;
			break;
		case '*':
			newString[j] = 0x48;
			break;
		case '+':
			newString[j] = 0x49;
			break;
		case ',':
			newString[j] = 0x4A;
			break;
		case '-':
			newString[j] = 0x4B;
			break;
		case '.':
			newString[j] = 0x4C;
			break;
		case '/':
			newString[j] = 0x4D;
			break;
		case ':':
			newString[j] = 0x4E;
			break;
		case ';':
			newString[j] = 0x4F;
			break;
		case '<':
			newString[j] = 0x50;
			break;
		case '=':
			newString[j] = 0x51;
			break;
		case '>':
			newString[j] = 0x52;
			break;
		case '?':
			newString[j] = 0x53;
			break;

		default:
			newString[j] = SPACE;
			break;
		}

		j++;
	}

	//Set the newly-found length
	length = j;

	//Output the text
	for (currentLetter = 0; currentLetter < length; currentLetter++)
	{
		if (xOff >= textXRight - textXLeft)
		{
			if (newString[currentLetter - 1] == SPACE)
			{
				//Increase Y Line
				yOff++;

				//Set x back to zero
				xOff = 0;
			}
			else if (newString[currentLetter] == SPACE)
			{
				//Increase Y Line
				yOff++;

				//Set x to -1
				xOff = -1;
			}
			else
			{
				//Start at currentLetter
				findSpace = currentLetter;

				while (newString[findSpace] != SPACE)
				{
					//Search backwords
					findSpace--;
				}
				for (i = findSpace + 1; i < currentLetter; i++)
				{
					//Get current x location
					test = xOff - (currentLetter - i);
					destination[(textYTop + yOff) * 32 + textXLeft + test] = SPACE;
				}

				yOff++;
				xOff = 0;
				currentLetter = findSpace + 1;
			}
		}

		//X and Y Flip
		if (xFlip && yFlip)
		{
			destination[(textYTop + yOff) * 32 + textXLeft + xOff] = newString[currentLetter] | BIT10 | BIT11;
		}
		//X Flip
		else if (xFlip)
		{
			destination[(textYTop + yOff) * 32 + textXLeft + xOff] = newString[currentLetter] | BIT10;
		}
		//Y Flip
		else if (yFlip)
		{
			destination[(textYTop + yOff) * 32 + textXLeft + xOff] = newString[currentLetter] | BIT11;
		}
		//No Flip
		else
		{
			destination[(textYTop + yOff) * 32 + textXLeft + xOff] = newString[currentLetter];
		}

		//Next space
		xOff++;
	}
}

/////////////////////////////////////WriteNum///////////////////////////////
void WriteNum(u16 textXLeft, u16 textXRight, u16 textYTop, u16 textYBottom, s16 number, u8 screenMem, u8 xFlip, u8 yFlip)
{
	//A string to hold the converted number
	char numString[17] = "";

	//Convert
	sprintf(numString, "%d", number);

	//Write the number as text
	WriteText(textXLeft, textXRight, textYTop, textYBottom, numString, screenMem, xFlip, yFlip);
}

#endif
