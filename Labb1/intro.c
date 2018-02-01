
#include <stdio.h>
#include "wrapper.h"

#define MSGSIZE 100

HANDLE hMailslot;
LPTSTR SlotName = TEXT("\\\\.\\mailslot\\superslotname");
int test;    
// Global variable
CRITICAL_SECTION CriticalSection;

typedef struct Textmsg {
	char text[MSGSIZE];
	BOOL notQuit;
	BOOL loop;
} Textmsg;



DWORD WINAPI Write(Textmsg* text)
{

	do {
		if (test == 1)
		{
			Sleep(100);
			printf("\n\nWriteStuff: ");

			fgets(text->text, MSGSIZE, stdin);  // (message, Storlek, input ifrån keyboard);

			if (strcmp(text->text, "END\n\0") == 0) {
				text->notQuit = FALSE;
			}

			EnterCriticalSection(&CriticalSection);
			mailslotWrite(hMailslot, text->text, MSGSIZE);
			LeaveCriticalSection(&CriticalSection);
			test = 0;
		}

	} while (text->notQuit == TRUE);

	return 0;
}



DWORD WINAPI Read(Textmsg* text)
{

	do {
		if (test == 0)
		{
			mailslotRead(hMailslot, text->text, MSGSIZE);
			test = 1;
		}
	} while (text->notQuit == TRUE);

	return 0;
}


/*
int main(void)
{
	test = 1;

	// Initialize the critical section one time only.
	if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
		0x00000400))
		return;

	DWORD tID, tID2;
	Textmsg _text;
	Textmsg* text = &_text;

	text->notQuit = TRUE;

	HANDLE hFile;

	hFile = mailslotCreate(SlotName);
	hMailslot = mailslotConnect(SlotName);


	tID = threadCreate(Write, text);
	printf("\nThread 1 created [%d]", tID);

	tID2 = threadCreate(Read, text);
	printf("\nThread 2 created [%d]", tID2);


	while (text->notQuit == TRUE) {


	}

	mailslotClose(hFile);
	printf("\n~*:[Avslutar main]:*~\n");
	system("Pause>nul");
	return 0;
}

*/
