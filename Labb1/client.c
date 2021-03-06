/*********************************************
* client.c
*
* Desc: lab-skeleton for the client side of an
* client-server application
*
* Revised by Dag Nystrom & Jukka Maki-Turja
* NOTE: the server must be started BEFORE the
* client.
*********************************************/
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "wrapper.h"


LPTSTR	ServerSlotname = TEXT("\\\\.\\mailslot\\superslot");
HANDLE	hClientslot = NULL;

Planet*	createNewPlanet();
char*	clientMailslot();
void	readIncommingMsg();

int main(void) {

	HANDLE		hMailSlot = NULL;
	HANDLE		hMutex = NULL;

	DWORD		waitResult;
	DWORD		bytesWritten = 0;
	Planet*		planet;
	int			run = 1;
	char		ch;
	char*		clientSlotname;
	int			choice = 1;
	int			fun = 1;


	// skapar client mailslot 
	clientSlotname = clientMailslot();


	// tr�d f�r inkommande meddelanden
	threadCreate(readIncommingMsg, NULL);



	// �ppna access till interprocess mutex
	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,					// request full access
		FALSE,								// handle not inheritable
		TEXT("ServerMailslotMutex"));		// object name

	if (hMutex == NULL) {
		printf("OpenMutex error: %d\n", GetLastError());
	}
	else
		printf("Successfully opened the mutex.\n");

	while (run) {

		while (choice == 1) {

			planet = createNewPlanet();	

			printf("\nHello!\nPress Enter to create a new planet.\n");
			getchar();

			printf("\nWrite the name of the planet:\n");
			scanf("%s", planet->name);
			printf("X-axis position:\n");
			scanf("%lf", &planet->posx);
			printf("y-axis position:\n");
			scanf("%lf", &planet->posy);
			printf("X-axis velocity:\n");
			scanf("%lf", &planet->velx);
			printf("y-axis velocity:\n");
			scanf("%lf", &planet->vely);
			printf("Give your planet a mass:\n");
			scanf("%lf", &planet->mass);
			printf("Give your planet a life:\n");
			scanf("%d", &planet->life);

			printf("\n\nYou want to create %s with position (%lf, %lf) and velocity (%lf, %lf)."
				" It has a life of %d time units and a mass of size %lf",
				planet->name, planet->posx, planet->posy, planet->velx, planet->vely, planet->life, planet->mass);

			// planetens processID
			planet->pid = GetCurrentThreadId();

			printf("\n\nIf you want to send this information to the server, press 1. If not, press any other number. Then press enter.\n");
			scanf("%d", &choice);
	

			//Clean input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);

			switch (choice) {
			case 1:

				// v�ntar p� mutex vi �ppnat access till tidigare
				waitResult = WaitForSingleObject(hMutex, INFINITE);

				switch (waitResult) {
				case WAIT_OBJECT_0:
					__try {
						// connect'a till server mailslot
						hMailSlot = mailslotConnect(ServerSlotname);
						if (hMailSlot == INVALID_HANDLE_VALUE) {
							printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
							return;
						}
						else {
							mailslotWrite(hMailSlot, planet, sizeof(Planet));
						}
					}
					// denna g�rs alltid
					__finally {
						if (!ReleaseMutex(hMutex)) {

							printf("\nCould not release mutex: %d", GetLastError());
						}
					}
				case WAIT_ABANDONED:
				case WAIT_TIMEOUT:
				case WAIT_FAILED:
					continue;
				}
				// inget ska skrivas/skickas till server. Vi l�mnar inre loop, frig�r planet
			default:
				free(planet);
				break;

			}
		
		

			// sl�pp handtag efter skrivning
			ReleaseMutex(hMutex);
		}

		// i yttre loop. Ska vi skriva en ny planet eller inte
		printf("\nPress '0' to quit, or anykey to create a new planets.");
		scanf("%d", &run);
		if (!run) {
			printf("\nThank you for playing!");
			break;
		}
		else
			choice = 1;

	}

	// G�rs automatiskt n�r process avslutas. H�r g�rs det explicit
	CloseHandle(hMutex);

	system("pause");
	return 0;
}

/****************************/
/*	Ett par st�dfunktioner  */
/****************************/
Planet* createNewPlanet()
{

	Planet* newPlanet = NULL;
	newPlanet = (Planet*)malloc(sizeof(Planet));
	if (newPlanet) {
		newPlanet->life		= 0;
		newPlanet->mass		= 0.0;
		newPlanet->posx		= 0.0;
		newPlanet->posy		= 0.0;
		newPlanet->velx		= 0.0;
		newPlanet->vely		= 0.0;
		newPlanet->prev		= NULL;
		newPlanet->next		= NULL;
		newPlanet->isAlive	= TRUE;
	}

	else {
		printf("ERROR:COULD NOT ALLOCATE MEM TO NEW PLANET."); /// denna kommer inte synas i f�nstret. Se labbsepc f�r workaround.
												 /// om denna sedan l�gges i client.c kommer det att synas.
	}

	return newPlanet;
}

char* clientMailslot()
{
	int			processID = GetCurrentThreadId();
	char*		str = (char*)malloc(sizeof(char) * 10);
	char*		partslotname = "\\\\.\\mailslot\\";
	char*		slotname = (char*)malloc(sizeof(char) * 13);


	strcpy(slotname, partslotname);
	//printf("\n%s", slotname);

	sprintf(str, "%d", processID);  // "gor om" int till string
	//printf("\n%s", str);

	str = (char*)realloc(str, sizeof(strlen(str)));
	//printf("\n%s", str);

	strcat(slotname, str);
	//printf("\n%s\t%d", slotname, (int) strlen(slotname));

	hClientslot = mailslotCreate(slotname);

	return slotname;
}

void readIncommingMsg()
{
	char	readBuffer[35];
	int		bytesRead = 0;

	while (1) {
		bytesRead = mailslotRead(hClientslot, readBuffer, sizeof(readBuffer));
		/*if (!bytesRead) {
			Sleep(300);
		}*/
	}
}