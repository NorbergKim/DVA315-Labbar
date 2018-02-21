/********************************************************************\
* server.c                                                           *
*                                                                    *
* Desc: example of the server-side of an application                 *
* Revised: Dag Nystrom & Jukka Maki-Turja							 *
*                                                                    *
* Based on generic.c from Microsoft.                                 *
*                                                                    *
*  Functions:                                                        *
*     WinMain      - Application entry point                         *
*     MainWndProc  - main window procedure                           *
*                                                                    *
* NOTE: this program uses some graphic primitives provided by Win32, *
* therefore there are probably a lot of things that are unfamiliar   *
* to you. There are comments in this file that indicates where it is *
* appropriate to place your code.                                    *
* *******************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"
#include <math.h>

/* the server uses a timer to periodically update the presentation window */
/* here is the timer id and timer period defined                          */

#define UPDATE_FREQ     10	/* update frequency (in ms) for the timer */

/* (the server uses a mailslot for incoming client requests) */

#define THREADCOUNT 100 /// hardcoded max threadcount



/*********************  Prototypes  ***************************/
/* NOTE: Windows has defined its own set of types. When the   */
/*       types are of importance to you we will write comments*/
/*       to indicate that. (Ignore them for now.)             */
/**************************************************************/

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI mailThread(LPVOID);



HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc	*/
				/* we need it to access the window for printing and drawing		*/



/*************************************/
/*
	Globala variabler som behövs
*/
DOUBLE		G = 6.67259e-11;				// graitavionskonstant
Planetlist* listofplanets;
RECT		rect;							// struct som innehåller koordinater till hörn
//LPTSTR		Slotname = TEXT("\\\\.\\mailslot\\superslot");
char*		Slotname = "\\\\.\\mailslot\\superslot";
HANDLE		hMutex;

/************************************/


/********************************************************************************/
/*
	Funktioner som:
	- beräknar nya positioner och hastigheter,
	- om en planet är vid liv
	- för att måla planeterna i fönstret
	- div. stödfunktioner till ovan.
*/

void	planetPosCalc(Planet* planet);
double	p2pRadius(Planet* focus, Planet* target);
double	p2pxacc(Planet* focus, Planet* target, double r);
double	p2pyacc(Planet* focus, Planet* target, double r);
void	newPlanetPos(Planet* planet, double atotx, double atoty);
void	paintPlanets();
void	checkIfDeadAndRemove(Planet* planet);

/*******************************************************************************/


/***************************************************************************************************/
/*
	Har under ligger definitioner for funktioner
	som hanterar den lankade listan av planeter.
*/

void		planetThread(Planet* planet);
Planetlist*	createPlanetlist();
Planet*		createNewPlanet();
void		addPlanet(Planet* data);	
void		removePlanet(char* IDtoRemove);

/***************************************************************************************************/


/**********************************************/
/*
	Några testfunkioner. Hårdkodade datatyper för testning.
	Dessa ryker sedan i live implementeringen.
*/

//void	calcAllPlanetPos();	/// denna måste göras om då varje planet tråd räknar endast ut ny pos/vel för sin egen planet
//void	initTestPlanetsAndFillplanetlist();
/*********************************************/
				

				/********************************************************************\
				*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
				*                                                                    *
				*   Purpose: Initializes Application                                 *
				*                                                                    *
				*  Comments: Register window class, create and display the main      *
				*            window, and enter message loop.                         *
				*                                                                    *
				*                                                                    *
				\********************************************************************/

				/* NOTE: This function is not too important to you, it only */
				/*       initializes a bunch of things.                     */
				/* NOTE: In windows WinMain is the start function, not main */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {

	HWND	hWnd;		// handle window
	DWORD	threadID;
	MSG		msg;

	hMutex = CreateMutex(
		NULL,								// default security descriptor
		FALSE,								// mutex not owned
		TEXT("ServerMailslotMutex"));		// object name, detta krävs för interprocess kommunikation. opernMutex används hos client
	GetWindowRect(hWnd, &rect);
	listofplanets = createPlanetlist();
	/// initTestPlanetsAndFillplanetlist(); /// endast för testning

	/* Create the window, 3 last parameters important */
	/* The tile of the window, the callback function */
	/* and the backgrond color */

	hWnd = windowCreate(hPrevInstance, hInstance, nCmdShow, "Himmel", MainWndProc, COLOR_WINDOW + 1);


	/* start the timer for the periodic update of the window    */
	/* (this is a one-shot timer, which means that it has to be */
	/* re-set after each time-out) */
	/* NOTE: When this timer expires a message will be sent to  */
	/*       our callback function (MainWndProc).               */

	windowRefreshTimer(hWnd, UPDATE_FREQ);


	/* create a thread that can handle incoming client requests */
	/* (the thread starts executing in the function mailThread) */
	/* NOTE: See online help for details, you need to know how  */
	/*       this function does and what its parameters mean.   */
	/* We have no parameters to pass, hence NULL.				*/


	threadID = threadCreate(mailThread, NULL);


	/* (the message processing loop that all windows applications must have) */
	/* NOTE: just leave it as it is. */
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


/********************************************************************\
* Function: mailThread                                               *
* Purpose: Handle incoming requests from clients                     *
* NOTE: This function is important to you.                           *
/********************************************************************/
DWORD WINAPI mailThread(LPVOID arg) {

	//Planet*		planet;
	void*		message;	/// testar att ta emot data sa har.
	DWORD		bytesRead = 0;
	static int	posY = 0;
	HANDLE		hMailbox;
	HANDLE		hThreads[THREADCOUNT];


	hMailbox = mailslotCreate(Slotname);

	/* create a mailslot that clients can use to pass requests through   */
	/* (the clients use the name below to get contact with the mailslot) */
	/* NOTE: The name of a mailslot must start with "\\\\.\\mailslot\\"  */


	while (1) {

		//planet = createNewPlanet;

		bytesRead = mailslotRead(hMailbox, message, sizeof(Planet));

		if (bytesRead == sizeof(Planet)) { // har läst planetdata
			TextOut(hDC, 20, 500, "Mailslot read success\0", sizeof(strlen("Mailslot read success\0")));
			threadCreate(planetThread, message);
		}
		else {
			/// om något annat lästs in med annan storlek än planet, kan vi anta att detta är hälsningen från client
			TextOut(hDC, 50, 1000, message, sizeof(message));
		}
	}






	//for (;;) {
	//	/* (ordinary file manipulating functions are used to read from mailslots) */
	//	/* in this example the server receives strings from the client side and   */
	//	/* displays them in the presentation window                               */
	//	/* NOTE: binary data can also be sent and received, e.g. planet structures*/


	//

	//	if (bytesRead != 0) {
	//		/* NOTE: It is appropriate to replace this code with something */
	//		/*       that match your needs here.                           */
	//		/* (hDC is used reference the previously created window) */

	//		//			TextOut(hDC, 10, 50+posY%200, planet->name, bytesRead);
	//	}
	//	else {
	//		/* failed reading from mailslot                              */
	//		/* (in this example we ignore this, and happily continue...) */
	//	}
	//}

	return 0;
}


/********************************************************************\
* Function: LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM) *
*                                                                    *
* Purpose: Processes Application Messages (received by the window)   *
* Comments: The following messages are processed                     *
*                                                                    *
*           WM_PAINT                                                 *
*           WM_COMMAND                                               *
*           WM_DESTROY                                               *
*           WM_TIMER                                                 *
*                                                                    *
\********************************************************************/
/* NOTE: This function is called by Windows when something happens to our window */

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	static int posX = 10;
	int posY;
	HANDLE context;
	static DWORD color = 0;



	switch (msg) {

			/**************************************************************/
			/*    WM_CREATE:        (received on window creation)
			/**************************************************************/

		case WM_CREATE:
			hDC = GetDC(hWnd);
			break;


			/**************************************************************/
			/*    WM_TIMER:         (received when our timer expires)
			/**************************************************************/

		case WM_TIMER:

			/* NOTE: replace code below for periodic update of the window */
			/*       e.g. draw a planet system)                           */
			/* NOTE: this is referred to as the 'graphics' thread in the lab spec. */

			/* here we draw a simple sinus curve in the window    */
			/* just to show how pixels are drawn                  */


			paintPlanets();

			windowRefreshTimer(hWnd, UPDATE_FREQ);
			break;



			/****************************************************************\
			*     WM_PAINT: (received when the window needs to be repainted, *
			*               e.g. when maximizing the window)                 *
			\****************************************************************/

		case WM_PAINT:

			/* NOTE: The code for this message can be removed. It's just */
			/*       for showing something in the window.                */
			context = BeginPaint(hWnd, &ps); /* (you can safely remove the following line of code) */
			//TextOut(context, 10, 10, "Hello, World!", 13); /* 13 is the string length */

			EndPaint(hWnd, &ps);
			break;


			/**************************************************************\
			*     WM_DESTROY: PostQuitMessage() is called                  *
			*     (received when the user presses the "quit" button in the *
			*      window)                                                 *
			\**************************************************************/

		case WM_DESTROY:

			PostQuitMessage(0);
			/* NOTE: Windows will automatically release most resources this */
			/*       process is using, e.g. memory and mailslots.           */
			/*       (So even though we don't free the memory which has been*/
			/*       allocated by us, there will not be memory leaks.)      */

			ReleaseDC(hWnd, hDC); /* Some housekeeping */
			break;


			/**************************************************************\
			*     Let the default window proc handle all other messages    *
			\**************************************************************/

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}

/****************************/
/* Beräknar alla planeters  */
/* nya positioner och       */
/* hastigheter			    */
/****************************/
void planetPosCalc(Planet* planet)
{
	Planet* targets;
	double	atotx = 0.0;
	double	atoty = 0.0;
	double	radius = 0.0;

	targets = listofplanets->head;

	while (targets != NULL) {
		if (strcmp(planet->name, targets->name)) {  // om ej samma berakna acceleration
			radius = p2pRadius(planet, targets);
			atotx += p2pxacc(planet, targets, radius);
			atoty += p2pyacc(planet, targets, radius);
		}

		targets = targets->next;
	}

	// när den samlade accelerationspåverkan mellan focus och resten (targets)
	// är uträknad så beräknas ny hastighet och position ut (i x- och y-led).
	newPlanetPos(planet, atotx, atoty);


	// kontrollera om livet kommer ner till noll eller om utanför fönster
	checkIfDeadAndRemove(planet);


	//reseta denna
	targets = listofplanets->head;
	
}

double p2pRadius(Planet* focus, Planet* target)
{
	return sqrt(pow((target->posx - focus->posx), 2) + pow((target->posy - focus->posy), 2));
}

double p2pxacc(Planet* focus, Planet* target, double r)
{
	static double dt = 10;
	double a = 0.0;
	double accx = 0.0;

	a = G * target->mass / pow(r, 2);				// acceleration mellan planeterna
	accx = a * (target->posx - focus->posx) / r;	// acceleration i x-led

	return accx;
}

double p2pyacc(Planet* focus, Planet* target, double r)
{
	static double dt = 10;
	double a = 0.0;
	double accy = 0.0;

	a = G * target->mass / pow(r, 2);				// acceleration mellan planeterna
	accy = a * (target->posy - focus->posy) / r;	// acceleration i y-led

	return accy;
}

void newPlanetPos(Planet* planet, double atotx, double atoty)
{
	static double dt = 10;

	planet->velx = planet->velx + atotx * dt;
	planet->vely = planet->vely + atoty * dt;

	planet->posx = planet->posx + planet->velx * dt;
	planet->posy = planet->posy + planet->vely * dt;
}

/***************************/
/* planet och listfunktion */
/***************************/


void planetThread(Planet* planet)
{
	addPlanet(planet);

	while (1)
	{
		planetPosCalc(planet);
		Sleep(200);
		if (!planet->isAlive) {
			free(planet);
			return;
		}
	}
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

void checkIfDeadAndRemove(Planet* planet)
{
	// kontrollera liv 
	planet->life--;
	if (planet->life == 0) {
		removePlanet(planet->name);
		planet->isAlive = FALSE;
		Beep(440, 100);
	}

	// kontrollera om utanför fönster
	else if
		(planet->posx >= rect.right || planet->posy <= rect.top || planet->posx <= rect.left || planet->posy >= rect.bottom) {
		removePlanet(planet->name);
		planet->isAlive = FALSE;
		Beep(880, 100);

	}

	/// antingen sköter removePlanet() meddelandet till client eller 
	/// så görs det här i en egen funkiton med pID som argument
}

void paintPlanets()
{
	static DWORD color = 0;
	Planet* planetToPaint = NULL;

	planetToPaint = listofplanets->head;

	while (planetToPaint != NULL) {
		SetPixel(hDC, planetToPaint->posx, planetToPaint->posy, (COLORREF)color);
		planetToPaint = planetToPaint->next;
	}

	free(planetToPaint);
}

Planetlist* createPlanetlist(void)
{
	Planetlist* listofplanets = NULL;
	listofplanets = (Planetlist*)malloc(sizeof(Planetlist));
	if (listofplanets) {
		listofplanets->head = NULL;
		listofplanets->planetcount = 0;
		printf("\nNew list created.\n");
	}
	else {
		printf("ERROR: COULD NOT ALLOCATE MEM.");
		return NULL;
	}

	return listofplanets;
}

void addPlanet(Planet * planet)
{
	if (listofplanets->planetcount == 0) // om tom planetlista lägg till först
		listofplanets->head = planet;

	else {
		listofplanets->head->prev = planet; // om fler planeter lägg till på följande sätt (läggs alltid till först)
		planet->next = listofplanets->head;
		listofplanets->head = planet;
	}

	listofplanets->planetcount++;
}

void removePlanet(char* planetname)
{
	Planet* planetToRemove = NULL;

	planetToRemove = listofplanets->head;

	// traversa lista och finn planet
	while (planetToRemove != NULL && strcmp(planetname, planetToRemove->name)) {
		planetToRemove = planetToRemove->next;
		if (planetToRemove == NULL) {
			printf("\nThere is no such planet.");
			return;
		}
	}

	// när funnen ta bort
	if (planetToRemove->prev == NULL) {	/// om första planeten
		if (listofplanets->planetcount == 1) { // om enda planet
			free(planetToRemove);
			listofplanets->head = NULL;
			listofplanets->planetcount--;
			return;
		}

		else {	// om flera planeter
			planetToRemove->next->prev = NULL;
			listofplanets->head = planetToRemove->next;
			free(planetToRemove);
			planetToRemove = NULL;
			listofplanets->planetcount--;
			return;
		}
	}

	else if (planetToRemove->next == NULL) { /// om sista planeten
		if (listofplanets->planetcount == 1) {	// om enda planet
			free(planetToRemove);
			listofplanets->head = NULL;
			listofplanets->planetcount--;
			return;
		}
		else {	// om flera planeter
			planetToRemove->prev->next = NULL;
			free(planetToRemove);
			planetToRemove = NULL;
			listofplanets->planetcount--;
			return;
		}
	}

	else {
		/// alla andra fall
		planetToRemove->prev->next = planetToRemove->next;
		planetToRemove->next->prev = planetToRemove->prev;
		free(planetToRemove);
		planetToRemove = NULL;
		listofplanets->planetcount--;
	}
}


/************************/
/* testfunktioner		*/
/************************/
//void initTestPlanetsAndFillplanetlist()
//{
//
//	// sätter alla pid till samma för att simulera att de kommer från sammam client.
//
//	Planet* p0 = createNewPlanet();
//	p0->life = 20000;
//	p0->mass = pow(10, 8);
//	p0->posx = 600;
//	p0->posy = 300;
//	p0->velx = 0.0;
//	p0->vely = 0.0;
//	strcpy(p0->name, "planet0");
//	strcpy(p0->pid, "p0");
//
//	Planet* p1 = createNewPlanet();
//	p1->life = 20000;
//	p1->mass = 1000;
//	p1->posx = 500;
//	p1->posy = 300;
//	p1->velx = 0.0;
//	p1->vely = 0.008;
//	strcpy(p1->name, "planet1");
//	strcpy(p1->pid, "p0");
//
//	Planet* p2 = createNewPlanet();
//	p2->life = 20000;
//	p2->mass = 10000;
//	p2->posx = 800;
//	p2->posy = 300;
//	p2->velx = -0.001;
//	p2->vely = -0.01;
//	strcpy(p2->name, "planet2");
//	strcpy(p2->pid, "p0");
//
//	addPlanet(p0);
//	addPlanet(p1);
//	addPlanet(p2);
//}

//void calcAllPlanetPos()
//{
//	Planet* focus;
//	Planet* targets;
//	Planet* shadow;
//	double atotx = 0.0;
//	double atoty = 0.0;
//	double radius = 0.0;
//
//	focus = listofplanets->head;
//	shadow = listofplanets->head; // skuggar pekare som i sin tur sätter en planet i focus för beräkningarna
//	targets = listofplanets->head;
//
//	while (focus != NULL) {
//		while (targets != NULL) {
//			if (strcmp(focus->name, targets->name)) {  // om ej samma exekvera if sats
//				radius = p2pRadius(focus, targets);
//				atotx += p2pxacc(focus, targets, radius);
//				atoty += p2pyacc(focus, targets, radius);
//			}
//
//			targets = targets->next;
//		}
//
//		// när den samlade accelerationspåverkan mellan focus och resten (targets)
//		// är uträknad så beräknas ny hastighet och position ut (i x- och y-led).
//		newPlanetPos(focus, atotx, atoty);
//
//		// reseta dessa innan varje iteration för nya hastigheter ska beräknas
//		atotx = 0.0;
//		atoty = 0.0;
//
//		// next planet to calc
//		focus = focus->next;
//
//		// kolla av om livet kommer ner till noll eller om utanför fönster
//		checkIfDeadAndRemove(shadow);
//
//
//		shadow = focus;
//
//		//reseta denna
//		targets = listofplanets->head;
//	}
//}