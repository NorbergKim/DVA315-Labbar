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
	int loops = 10;
	Planet* planet;


	/// hardcoded data to be removed
	planet = createNewPlanet();
	planet->life = 20000;
	planet->mass = pow(10, 8);
	planet->posx = 600;
	planet->posy = 300;
	planet->velx = 0.08;
	planet->vely = 0.02;
	strcpy(planet->name, "planet0\0");
	strcpy(planet->pid, "p0\0");


	
	mailSlot = mailslotConnect(Slotname);
	if (mailSlot == INVALID_HANDLE_VALUE) {
		printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
		return;
	}



	/* NOTE: replace code below for sending planet data to the server. */
		
	
	bytesWritten = mailslotWrite(mailSlot, planet, sizeof(Planet));
	if (bytesWritten != -1)
		printf("data sent to server (bytes = %d)\n", bytesWritten);
	else
		printf("failed sending data to server\n");

	//while (loops-- > 0) {
	//	/* send a friendly greeting to the server */
	//	/* NOTE: The messages sent to the server need not to be of equal size.       */
	//	/* Messages can be of different sizes as long as they don't exceed the       */
	//	/* maximum message size that the mailslot can handle (defined upon creation).*/




	//}

	system("pause");

	mailslotClose(mailSlot);

	/* (sleep for a while, enables you to catch a glimpse of what the */
	/*  client prints on the console)                                 */
	system("pause");
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
