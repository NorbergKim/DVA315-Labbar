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


Planet*	createNewPlanet();
LPTSTR		ServerSlotname = TEXT("\\\\.\\mailslot\\superslot");
HANDLE		hClientMailslot = NULL;


/// detta ar den tjanst server ger clienten. mata in data och ge tillbaka en unik pekare till skapad planet
/// returnerade addressen av denna skickas sedan i mailsloten till servern.
Planet*	createNewPlanet();
char*		clientMailslot();

typedef struct LivingPlanets {
	char	listOfPlanets[100][20];		// hårdkodar in att listan hanterar 100 stycken planet(namn)
}LivingPlanets;


void main(void) {

	HANDLE		mailSlot;
	DWORD		bytesWritten;
	Planet*	planet;
	char		ch;
	char*		clientSlotname;
	int			choice;
	HANDLE		hMutex = NULL;
	int			run = 1;
	int			index = 0;
	
	clientSlotname = clientMailslot();
	LivingPlanets planetList;

	/*
		Kvar att implementera: ta emot meddelande om planetdöd. Förslagsvis kan detta ske i början av loopen. 
		Denna kanske ska vara i en egen tråd.
	*/

	while (run) {


		planet = createNewPlanet();
		choice = 1;
		while (choice == 1) {

			planet->slotname = clientMailslot();

			printf("\nHello!\nPress Enter to create a new planet.\n");
			getchar();

			// har lägger man att mutex ska huggas. D.v.s. client tar endast mutex om det ska skrivas in data.
			hMutex = OpenMutex(
				MUTEX_ALL_ACCESS,					// request full access
				FALSE,									// handle not inheritable
				TEXT("ServerMailslotMutex"));		// object name

			if (hMutex == NULL) {
				printf("OpenMutex error: %d\n", GetLastError());
			}
			else
				printf("Successfully opened the mutex.\n");

			mailSlot = mailslotConnect(ServerSlotname);
			if (mailSlot == INVALID_HANDLE_VALUE) {
				printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
				CloseHandle(hMutex); // släpp mutex
			}

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

			planet->pid = GetCurrentThreadId();

			planet->slotname = (char*)malloc(sizeof(strlen(clientSlotname)));
			strcpy(planet->slotname, clientSlotname);

			printf("\n\nIf you want to send this information to the server, press 1. If not, press any button. Then press enter.\n\n");
			scanf_s("%d", &choice);

			//Clean input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);

			switch (choice)
			{
			case 1:
				planetList.listOfPlanets[index][0] = planet->name;
				index++;
				mailSlot = mailslotConnect(ServerSlotname);
				if (mailSlot == INVALID_HANDLE_VALUE) {
					printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
					CloseHandle(hMutex);/// släpp mutex
					return;
				}
				else {
					bytesWritten = mailslotWrite(mailSlot, planet, sizeof(Planet));
					if (bytesWritten) // non zero, skett skrivning till mailslot
						printf("data sent to server, %d planet data.)\n", bytesWritten);
					else
						printf("failed sending data to server\n");
				}

				// free(planet); break;

			default: break;
			}

			CloseHandle(hMutex);
		
			for (index; index < 0; index--) {
				printf("\nPlanets living:\n%s", planetList.listOfPlanets[index][0]);
			}

			printf("\nPress '0' to quit, or enter to continue creating planets.");
			scanf("%d", run);
			if (!run) {
				printf("\nThank you for playing!");
			}
		}
	}

	system("pause");
	mailslotClose(mailSlot);
	system("pause");
	return;
}

/***********************	*/
/*		Ett par stödfunktioner    */
/************************/
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
		printf("ERROR:COULD NOT ALLOCATE MEM TO NEW PLANET."); /// denna kommer inte synas i fönstret. Se labbsepc för workaround.
												 /// om denna sedan lägges i client.c kommer det att synas.
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

	hClientMailslot = mailslotCreate(slotname);

	return slotname;
}