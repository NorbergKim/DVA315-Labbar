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

#define MESSAGE "Hello!"

void main(void) {

	HANDLE mailSlot;
	DWORD bytesWritten;
	int loops = 2000;
	int choice = 0; 
	planet_type planet; 
	planet_type* planetPtr = &planet; 

	mailSlot = mailslotConnect("mailbox"); 

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
					   /* maximum message size that the mailslot can handle (defined upon creation).*/

		printf("Hello!\nWhat do you want to do?\n\n"); 
		printf("Press 1 if you want to create a new planet.\n"); 

		scanf_s("%d", &choice);

		switch (choice)
		{
			case 1: 
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
				scanf("%lf", &planetPtr->life);

				printf("\n\nYou want to create %s with position (%lf, %lf) and velocity (%lf, %lf)." 
					"It has a life of %lf time units and a mass of size %lf", 
					planetPtr->name, planetPtr->sx, planetPtr->sy, planetPtr->vx, planetPtr->vy, planetPtr->life, planetPtr->mass); 

				printf("\n\nIf you want to send this information to the server, press 5.\n\n"); 
		}
    
		/*bytesWritten = mailslotWrite (mailSlot, planetPtr, sizeof(planetPtr));
		if (bytesWritten!=-1)
			printf("data sent to server (bytes = %d)\n", bytesWritten);
		else
			printf("failed sending data to server\n");*/
	}

	mailslotClose (mailSlot);

					/* (sleep for a while, enables you to catch a glimpse of what the */
					/*  client prints on the console)                                 */
	Sleep(2000);
	return;
}
