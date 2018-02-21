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


Planet*	createNewPlanet();
LPTSTR	Slotname = TEXT("\\\\.\\mailslot\\superslot");


/// detta ar den tjanst server ger clienten. mata in data och ge tillbaka en unik pekare till skapad planet
/// returnerade addressen av denna skickas sedan i mailsloten till servern.
Planet*	createNewPlanet();
char*	clientMailslotName();


void main(void) {

	HANDLE	mailSlot;
	DWORD	bytesWritten;
	int		loops = 10;
	Planet* planet;
	char	ch;

	HANDLE	hMutex;

	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,            // request full access
		FALSE,                       // handle not inheritable
		TEXT("ServerMailslotMutex"));  // object name

	/// att implementera i loop:
	/// har maste man reallokera och langre ner free(planet),for att det ska skickas in en nya adress for varje ny planet,
	/// detta är kanske inte nödvändigt, för server allokerar på heapen för att ta emot data.

	while (1) {

		/// det forsta som sker nar en client startar ar att dess specifika mailslot skapas. 
		/// efteråt skickas det en automatiserad hälsning till sever.
		/// sedan börjar clienten ta in planet data

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


		//Clean input buffer
		while ((ch = getchar()) != '\n' && ch != EOF);


		planet = createNewPlanet();


		mailSlot = mailslotConnect(Slotname);
		if (mailSlot == INVALID_HANDLE_VALUE) {
			printf("Failed to get a handle to the mailslot!!\nHave you started the server?\n");
			return;
		}





		bytesWritten = mailslotWrite(mailSlot, planet, sizeof(Planet));
		if (bytesWritten != -1)
			printf("data sent to server, %d planet data.)\n", bytesWritten);
		else
			printf("failed sending data to server\n");
	}

		/* NOTE: replace code below for sending planet data to the server. */
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
		newPlanet->life		= 5000;
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
		printf("ERROR:COULD NOT ALLOCATE MEM."); /// denna kommer inte synas i fönstret. Se labbsepc för workaround.
												 /// om denna sedan lägges i client.c kommer det att synas.
		return newPlanet;
	}

	return newPlanet;
}

char* clientMailslotName()
{
	int processID = GetCurrentThreadId();
	char str[12];

	sprintf(str, "%d", processID);  // "gor om" int till string


}
