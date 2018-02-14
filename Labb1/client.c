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
#include <string.h>``
#include "wrapper.h"


Planet* createNewPlanet();
LPTSTR Slotname = TEXT("\\\\.\\mailslot\\superslot");


void main(void) {

	HANDLE mailSlot;
	DWORD bytesWritten;
	int loops = 2000;
	char choice = NULL;
	int menu = 1;
	Planet* planet;
	char ch = NULL;

	planet = createNewPlanet();

	mailSlot = mailslotConnect(Slotname);

	if (mailSlot == INVALID_HANDLE_VALUE) {
		printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
		return;
	}

	/*Loop not working yet, only works first round*/

	/* NOTE: replace code below for sending planet data to the server. */
	while (loops-- > 0) {
		/* send a friendly greeting to the server */
		/* NOTE: The messages sent to the server need not to be of equal size.       */
		/* Messages can be of different sizes as long as they don't exceed the       */
		/* maximum message size that the mailslot can handle (defined upon creation).*/;

		while (menu == 1)
		{
			//Resets choice-variable
			choice = 0;

			printf("Hello!\nPress Enter to create a new planet.\n");
			getchar();

			printf("Write the name of the planet:\n");
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

			strcpy(planet->pid, "p0\0");

			printf("\n\nIf you want to send this information to the server, press 1. If not, press any button. Then press enter.\n\n");
			scanf_s("%d", &choice);

			if (choice == 1)
				menu = 0;

			//Clean input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);
		}

		printf("Menu has exit.");

		bytesWritten = mailslotWrite(mailSlot, planet, sizeof(Planet));
		if (bytesWritten != -1)
			printf("data sent to server (bytes = %d)\n", bytesWritten);
		else
			printf("failed sending data to server\n");

		menu = 1;
	}

	mailslotClose(mailSlot);

	/* (sleep for a while, enables you to catch a glimpse of what the */
	/*  client prints on the console)                                 */
	Sleep(2000);
	return;
}




Planet* createNewPlanet()
{

	Planet* newPlanet = NULL;
	newPlanet = (Planet*)malloc(sizeof(Planet));
	if (newPlanet) {
		newPlanet->life = 5000;
		newPlanet->mass = 0.0;
		newPlanet->posx = 0.0;
		newPlanet->posy = 0.0;
		newPlanet->velx = 0.0;
		newPlanet->vely = 0.0;
		newPlanet->prev = NULL;
		newPlanet->next = NULL;
	}

	else {
		printf("ERROR:COULD NOT ALLOCATE MEM."); /// denna kommer inte synas i fönstret. Se labbsepc för workaround.
												 /// om denna sedan lägges i client.c kommer det att synas.
		return newPlanet;
	}

	return newPlanet;
}
