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
#include <conio.h>

#define MESSAGE "Hello!"

LPTSTR name = TEXT("\\\\.\\mailslot\\superslotname");

void main(void) {

	HANDLE mailSlot;
	DWORD bytesWritten;
	int loops = 2000;
	char choice = NULL; 
	int menu = 1; 
	planet_type planet; 
	planet_type* planetPtr = &planet; 
	char ch = NULL; 

	mailSlot = mailslotConnect(name); 

	if (mailSlot == INVALID_HANDLE_VALUE) {
		printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
		return;
	}

	/*Skapa mailslot som lyssnar efter om planeter dött?*/

					/* NOTE: replace code below for sending planet data to the server. */
	while(loops-- > 0) {
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
			scanf("%s", planetPtr->name);
			printf("X-axis position:\n");
			scanf("%lf", &planetPtr->sx);
			printf("y-axis position:\n");
			scanf("%lf", &planetPtr->sy);
			printf("X-axis velocity:\n");
			scanf("%lf", &planetPtr->vx);
			printf("y-axis velocity:\n");
			scanf("%lf", &planetPtr->vy);
			printf("Give your planet a mass:\n");
			scanf("%lf", &planetPtr->mass);
			printf("Give your planet a life:\n");
			scanf("%d", &planetPtr->life);

			printf("\n\nYou want to create %s with position (%lf, %lf) and velocity (%lf, %lf)."
				" It has a life of %d time units and a mass of size %lf",
				planetPtr->name, planetPtr->sx, planetPtr->sy, planetPtr->vx, planetPtr->vy, planetPtr->life, planetPtr->mass);

			printf("\n\nIf you want to send this information to the server, press 1. If not, press any button. Then press enter.\n\n");
			scanf_s("%d", &choice);

			if (choice == 1)
				menu = 0;

			//Clean input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);
		}

		printf("Menu has exit."); 
    
		bytesWritten = mailslotWrite (mailSlot, planetPtr, sizeof(planetPtr));
		if (bytesWritten!=-1)
			printf("data sent to server (bytes = %d)\n", bytesWritten);
		else
			printf("failed sending data to server\n");

		menu = 1; 
	}

	mailslotClose (mailSlot);

					/* (sleep for a while, enables you to catch a glimpse of what the */
					/*  client prints on the console)                                 */
	Sleep(2000);
	return;
}
