//---------------------------------------------------------------------------------------
//	File:		Life.c
//
//	Functions:	main()
//				usable()
//				chInStr()
//				isAlive()
//				setLive()
//				setDead()
//				birther()
//				getCount()
//				playLife()
//				showLife()
//				readData()
//---------------------------------------------------------------------------------------

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "life.h"

#pragma warning(disable: 4996) 

//-----------------------------------------------------------------------------
//	Function:		main()
//
//	Title:			The main function of "Game of Life"
//
//	Description:	main() sets up and initializes the needed data structures, 
//					opens the input file for reading, then calls one function
//					to read in the data, then another one to play the game of life. 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		Operating System
//
//	Calls:			readData()
//					playLife()
//
//	Parameters:		argc: int;	 number of commandline arguments passed to main()
//					argv[]: char*;  optional commandline arguments where:
//					argv[1] may contain the path of the input file 
//
//	Returns:		EXIT_SUCCESS (successful execution) or 
//					an applicable Windows "errno" in case of a file I/O error
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	char	world[GEN][ROW][COL];
	int		i;
	int		gen = 0;
	char	fname[FILENAME_MAX];
	FILE	*fp;
	Life	life;

	// sets and clears some "life" values 
	strcpy(life.name, "Game of Life");
	life.row = ROW_DEF;
	life.col = COL_DEF;
	life.dead = DEAD_DEFAULT;
	life.live = LIVE_DEFAULT;
	life.delay = DEFAULT_DELAY;
	life.gen = 0;
	for (i = 0; i < GEN; i++)
		life.cells[i] = 0;

	// sets code page for the box drawing characters 
	SetConsoleOutputCP(437);

	// get output window handle for cursor positioning 
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
		puts("\nError getting handle for hStdout!");

	// get input from either argv[], or by using a local Life.txt
	if (argc > 1)
		strncpy(fname, argv[1], FILENAME_MAX);
	else
		strncpy(fname, DEFAULT_FNAME, FILENAME_MAX);

	if ((fp = fopen(fname, "r")) == NULL)
	{
		printf(FILE_ERROR, fname, HIT_ENTER);
		getchar();
		exit(errno);	// most likely file_not_found 
	}
	else
	{
		readData(world, gen, &life, fp);
		fclose(fp);
		system("cls");
		playLife(world, gen, &life);
	}

	printf("\nThank you for playing life; %s \n", HIT_ENTER);
	getchar();
	return EXIT_SUCCESS;

}	// end main()


//-----------------------------------------------------------------------------
//	Function:		chInStr()
//
//	Title:			Utility function for user input checking 
//
//	Description:	Given a user input character, this function will return 
//					TRUE if the character is in the string of allowed chaacters,
//					FALSE otherwise.
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/16/2016
//	Version:		1.0
//
//	Called by:		play_life()
//					read_data()
//
//	Calls:			N/A
//
//	Parameters:		
//			ch:		char;	character to be checked against the possibilities
//			str:	char*	string of possible characters 
//
//	Returns:		TRUE if the character is in the string
//					FALSE otherwise 
//
//	History log:	11/16/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------
 
int chInStr(char ch, const char* str)
{
	while (*str)
		if (ch == *str++)
			return TRUE;
	return FALSE;
}


//-----------------------------------------------------------------------------
//	Function:		usable()
//
//	Title:			Utility function for display character validation 
//
//	Description:	There are a number of characters that would really mess up things
//					when printed  with the usual screen I/O functions, so they cannot 
//					be used to represent live or dead cells under *any* circumstance 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		play_life()
//
//	Calls:			N/A
//
//	Parameters:		
//			ch:		char;	character to be checked against the disallowed chars
//
//	Returns:		TRUE if the character can be used 
//					FALSE otherwise 
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

int usable(char ch)
{
	if ((ch == NUL) || (ch == BELL) || (ch == BS) || (ch == TAB) ||
		(ch == LF)  || (ch == CR)   || (ch == DEL))
		return FALSE;
	else
		return TRUE;
}


//-----------------------------------------------------------------------------
//	Function:		readData()
//
//	Title:			Function 
//
//	Description:	Given a file pointer to an open text file containing a header
//					and a textual image of a game of life pattern, this function  
//					will read in the 'life' image and the associated parameters. 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		main()
//
//	Calls:			setLive()
//					setDead()
//					chrInStr()
//
//	Parameters:		
//		arr:		pointer to 3D array contining the game of life world 
//		idx:		int; index into a 2D slice in the above 3D array
//		*life:		Life: a structure containing various "life" variables 
//		*fp:		FILE; pointer to an opened file containing a life pattern
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void readData(char arr[][ROW][COL], int idx, Life *life, FILE *fp)
{
	int		num_result;
	int		chr_result;
	char	*fg_result;
	char	line[FILENAME_MAX];
	char	string[FILENAME_MAX];
	char	*pdest;
	char	ch;
	int		number;
	int		found_end = FALSE;
	int		i;
	int		j;

	do {
		fg_result = fgets(line, FILENAME_MAX, fp);

		if ((fg_result != NULL) && (line[0] != ';'))
		{
			pdest = strchr(line, ':');
			pdest++;
			if (pdest != NULL)
			{
				num_result = sscanf(pdest, "%d", &number);
				chr_result = sscanf(pdest, "%c", &ch);
				strcpy(string, pdest);
			}
			else
			{
				number = 0;
				ch = NUL;
				string[0] = NUL;
			}

			// read the values for Row, Col, Name, live and dead chars 
			// from the header of a life text file 
			if (strncmp(strupr(line), "ROW", strlen("ROW")) == 0)
			{
				if (number > ROW_MAX)
				{
					printf(COUNT_MAX_STR, "Row", number, ROW_MAX);
					printf(SET_TO_STR, "Row", ROW_MAX);
					number = ROW_MAX;
				}
				if (number < ROW_MIN)
				{
					printf(COUNT_MIN_STR, "Row", number, ROW_MIN);
					printf(SET_TO_STR, "Row", ROW_MIN);
					number = ROW_MIN;
				}
				life->row = number;
			}

			if (strncmp(strupr(line), "COL", strlen("COL")) == 0)
			{
				if (number > COL_MAX)
				{
					printf(COUNT_MAX_STR, "Column", number, COL_MAX);
					printf(SET_TO_STR, "Column", COL_MAX);
					number = COL_MAX;
				}
				if (number < COL_MIN)
				{
					printf(COUNT_MIN_STR, "Column", number, COL_MIN);
					printf(SET_TO_STR, "Column", COL_MIN);
					number = COL_MIN;
				}
				life->col = number;
			}

			if (strncmp(strupr(line), "NAME", strlen("NAME")) == 0)
			{
				*strchr(string, (int)'\n') = NUL;
				strcpy(life->name, string);
			}

			if (strncmp(strupr(line), "VER", strlen("VER")) == 0)
				life->version = number;

			if (strncmp(strupr(line), "DEAD", strlen("DEAD")) == 0)
				if (usable(ch))
					life->dead = ch;
				else
				{
					printf(CHAR_NO_GOOD, ch, "dead");
					printf(CHAR_SUBSTED, DEAD_DEFAULT, "dead");
					life->dead = DEAD_DEFAULT;
				}

			if (strncmp(strupr(line), "LIVE", strlen("LIVE")) == 0)
				if (usable(ch))
					life->live = ch;
				else
				{
					printf(CHAR_NO_GOOD, ch, "live");
					printf(CHAR_SUBSTED, LIVE_DEFAULT, "live");
					life->live = LIVE_DEFAULT;
				}

			if (strncmp(strupr(line), "BGCOLOR", strlen("BGCOLOR")) == 0)
				life->bgcolor = number;

			if (strncmp(strupr(line), "FGCOLOR", strlen("FGCOLOR")) == 0)
				life->fgcolor = number;

			if (strncmp(strupr(line), "END", strlen("END")) == 0)
				found_end = TRUE;

		}	// end if 

	} while (!found_end);

	// ensure that live and dead chars are different 
	if (life->dead == life->live)
	{
		life->dead = DEAD_DEFAULT;
		life->live = LIVE_DEFAULT;
		printf("\nThe characters for live and dead cells were found to be equal!!");
		printf("They were changed to '%c' and '%c'.\n", LIVE_DEFAULT, DEAD_DEFAULT);
		printf("\nHit <Enter> to continue...");
		getchar();
	}

	// this loop populates the first 2D plane 
	// if fgets() can't read any more data (most likely because of EOF), or line has 
	// less data then expected, this loop will populate the remainder with dead cells
	for (i = 0; i < life->row; i++)
	{
		fg_result = fgets(line, FILENAME_MAX, fp);

		for (j = 0; j < life->col; j++)
			if ((fg_result != NULL) && (j <= (int)strlen(line)))
				if (chInStr(line[j], LIVE_SET))
					setLive(arr, idx, life, i, j);
				else
					setDead(arr, idx, life, i, j);
			else
				setDead(arr, idx, life, i, j);

		line[0] = NUL;
	}

}	// end readData()


//-----------------------------------------------------------------------------
//	Function:		showLife()
//
//	Title:			Function to display one generation from the "Game of Life"
//
//	Description:	Given the 3D world of life and an index selecting one 2D plane
//					from that 3D world, this function will print out the content 
//					of the selected plane surrended by a border. 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		playLife()
//
//	Calls:			N/A
//
//	Parameters:		
//		arr:		pointer to 3D array containing the world of game of life 
//		idx:		integer index into a 2D slice in the above 3D array
//		*life:		Life: a structure containing various "life" variables 
//
//	Returns:		N/A 
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void showLife(char arr[][ROW][COL], int idx, Life *life)
{
	int		i;
	int		j; 
	char	parr[ROW + 2][COL + 2];			// print array 
	COORD	coord = { 0, 1 };				// 2nd row, 1st character 

	// instead of system("cls")  which takes a very-very long time and 
	// may cause a bad flicker, just move the cursor into the beginning 
	// of the 2nd row with a Win32 API call, if possible 
	if (hStdout != NULL)
		SetConsoleCursorPosition(hStdout, coord);
	else
		system("cls"); 

	// display 2nd line of title 
	printf("Generation: %-10d Live cells: %-5d\n", life->gen, life->cells[idx]);

	// copy data into print array leaving a line around for border 
	for (i = 0; i < life->row; i++)
		for (j = 0; j < life->col; j++)
			parr[i + 1][j + 1] = arr[idx][i][j];

	// put in the horizomtal lines 
	for (i = 1; i <= life->col; i++)
	{
		parr[0][i] = HOR_LINE;						// ASCI 205
		parr[life->row + 1][i] = HOR_LINE;			// ASCI 205 
	}

	// put in the vertical lines, plus terminating '\0's at end
	for (i = 0; i <= life->row; i++)
	{
		parr[i][0] = VERT_LINE;
		parr[i][life->col + 1] = VERT_LINE;			// ASCII 186 
		parr[i][life->col + 2] = NUL;
	}

	//  put in the 4 corners plus 2 terminating '\0's
	parr[0][0] = TOP_LEFT;							// ASCII 201
	parr[0][life->col + 1] = TOP_RIGHT;				// ASCII 187 
	parr[0][life->col + 2] = NUL;
	parr[life->row + 1][0] = BOT_LEFT;				// ASCII 200
	parr[life->row + 1][life->col + 1] = BOT_RIGHT;	// ASCII 188
	parr[life->row + 1][life->col + 2] = NUL;

	// finally, display the field on the screen 
	for (i = 0; i < (life->row + 2); i++)
		puts((char *)parr[i]);

}	// end display_field()


//-----------------------------------------------------------------------------
//	Function:		playLife()
//
//	Title:			The function responsible for playing "The Game of Life" 
//
//	Description:	Given a 3D world of life where generation 0 is loaded with 
//					a starting pattern, this function will play the game of life 
//					by calling the appropriate subroutines and handling user 
//					input.
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		main()
//
//	Calls:			birther()
//					showLife()
//
//	Parameters:		
//		arr:		pointer to 3D array containing the game of life world 
//		idx:		integer index into a 2D slice in the above 3D array
//		*life:		Life: a structure containing various "life" variables 
//
//	Returns:		N/A 
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void playLife(char arr[][ROW][COL], int gen, Life *life)
{
	char	choice = 'U';		// = sentinel for "Unused"
	int		quit = FALSE;
	int		count = 0;			// counts to run in a loop w/o waiting for user input								
	COORD	coord = { 0, 0 };	// coordinate for top left corner 
	int		idx;

	SetConsoleCursorPosition(hStdout, coord);		// move cursor to top-left corner 
	printf("Name: %-16s Rows: %d Columns: %d\n", life->name, life->row, life->col); 

	// the main loop starts here 
	do { 
		_sleep(life->delay); 

		// for most purposes, use the index derived from gen [generation] 
		idx = gen % GEN; 
		showLife(arr, idx, life);
		count--; 

		if (count > 0)
		{
			do {
				// preset an Unused value to clear leftovers and to ensure exit
				choice = 'U';		

				printf("%c[S]top  [Q]uit  [Space]  [+]/[-] delay of %d ms : ",
										 CR, life->delay);
				if (kbhit())
					choice = getche();

				if ((choice == 's') || (choice == 'S'))
				{
					printf("             "); 
					count = 0;
				}

			} while (!chInStr(choice, "U sSqQ+-"));
		}


		if ((count <= 0) && ((choice != 'q') || (choice != 'Q')))
		{
			do {
				printf("%c[N]ext  [R]un 100 cycles  run [C]ontinuously  [Q]uit: ", CR);
				choice = getche();

				// translate <Enter> into single step to meet project specification 
				if (choice == ENTER)
					choice = 'n'; 

			} while (!chInStr(choice, "nNrRcCqQ"));
		}

		// this switch handles the unhandled inputs from both of the above
		switch (choice) 
		{
		case ' ':			// pause for <Space>
			// clear leftover text first 
			printf(SPACES); 
			printf("%c %s %s ", CR, SPACES, SPACES); 
			printf("%cHit <Space> to un-pause....", CR);
			do {
				choice = getch();
			} while (choice != ' ');
			break;

		case '+':			// increase delay to slow down game 
			if (life->delay > 100)
				life->delay += 10;
			else if (life->delay > 30)
				life->delay += 5;
			else if (life->delay > 10)
				life->delay += 2;
			else
				life->delay += 1;
			break;

		case '-':			// decrease delay to speed up game 
			if (life->delay > 100)
				life->delay -= 10;
			else if (life->delay > 30)
				life->delay -= 5;
			else if (life->delay > 10)
				life->delay -= 2;
			else
				life->delay -= 1;
			if (life->delay < 0)
				life->delay = 0;
			break;

		case 'n':				// [N]ext = single step 
		case 'N':				// <Enter> will also single step 
			count = 1;
			break;

		case 'r':				// [R]un for 100 cycles 
		case 'R':
			// clear leftover text first 
			printf(SPACES); 
			printf("%c %s %s ", CR, SPACES, SPACES);
			count = 100;
			break;

		case 'c':				// run Continuously 
		case 'C':
			// clear leftover text first 
			printf(SPACES);
			printf("%c %s %s ", CR, SPACES, SPACES);
			count = INT_MAX;
			break;

		case 'p':				// Prev
		case 'P':
			puts("not implemented yet");
			break;

		case 'q':				// Quit 
		case 'Q':
			quit = TRUE;
			break;

		case 's':				// Stop continuous mode 
		case 'S':
			printf("             ");
			count = 0;
			break;

		case 'U':				// Unused, sentinel, just eat it silently
			break; 

		default:			
			printf("Life must've been bugged with: '%c' \n", choice);
			getchar(); 
			break;
		}

		// if not quitting, give life to the next generation 
		if (!quit)
		{
			birther(arr, gen, life);
			gen++;
			life->gen = gen;
		}

	} while (!quit); 

	printf("\n\nFinished Life after %d generations with %d live cells \n",
			life->gen, life->cells[gen % GEN]);

}	// end playLife()


//-----------------------------------------------------------------------------
//	Function:		isAlive()
//
//	Title:			Function  to check if a given cell is alive or not
//
//	Description:	Given some valid coordinates, this function will check 
//					if the cell there is alive, or not 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		getCount()
//					
//	Calls:			N/A
//
//	Parameters:		
//			arr:	pointer to a 3D array (world)
//			idx:	integer to select a 2D slice of the above array
//		   life:	pointer to a Life structure containing most variables 
//            y:    integer to select row in the 2D plane 
//            x:    integer to select column in the 2D plane 
//
//	Returns:		TRUE if the cell is alive
//					FALSE otherwise 
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

int isAlive(char arr[][ROW][COL], int idx, Life *life, int y, int x)
{
	return (arr[idx][y][x] == life->live);
}


//-----------------------------------------------------------------------------
//	Function:		setLive()
//
//	Title:			Sets a selected cell live
//
//	Description:	Given some valid coordinates, this function will set the 
//					cell located at that coordinate to live. 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		getCount()
//					
//	Calls:			N/A
//
//	Parameters:		
//			arr:	pointer to a 3D array (world)
//			idx:	integer to select a 2D slice of the above array
//		   life:	pointer to a Life structure containing most variables 
//            y:    integer to select row in the 2D plane 
//            x:    integer to select column in the 2D plane 
//
//	Returns:		N/A
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void setLive(char arr[][ROW][COL], int idx, Life *life, int y, int x)
{
	arr[idx][y][x] = life->live;
	life->cells[idx]++;
}


//-----------------------------------------------------------------------------
//	Function:		setDead()
//
//	Title:			Sets a selected cell dead
//
//	Description:	Given some valid coordinates, this function will set the cell 
//					there dead. 
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		birther()
//					
//	Calls:			N/A
//
//	Parameters:		
//			arr:	pointer to a 3D array (world)
//			idx:	integer to select a 2D slice of the above array
//		   life:	pointer to a Life structure containing most variables 
//            y:    integer to select row in the 2D plane 
//            x:    integer to select column in the 2D plane 
//
//	Returns:		N/A
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void setDead(char arr[][ROW][COL], int idx, Life *life, int y, int x)
{
	arr[idx][y][x] = life->dead;
}


//-----------------------------------------------------------------------------
//	Function:		getCount()
//
//	Title:			Returns the count of live cells around a given cell
//
//	Description:	Given some valid coordinates, this function will count the 
//					number of live cells around a selected  cell. This function 
//					utilizes an 8 by 2 array containing the x and y differences 
//					compared to the selected cell (0, 0), as shown below:
//                    ( -1, -1)  ( 0, -1) ( +1, -1) 
//					  ( -1   0)  ( cell ) ( +1,  0)
//					  ( -1, +1)  ( 0, +1) ( +1, +1)
//
//	Programmer:		Janos Szamosfalvi
//
//	Date:			11/26/2016
//	Version:		1.0
//
//	Called by:		birther()
//					
//	Calls:			isAlive()
//
//	Parameters:		
//			arr:	pointer to a 3D array (world)
//			idx:	integer to select a 2D slice of the above array
//		   life:	pointer to a Life structure containing most variables 
//            y:    integer to select row in the 2D plane 
//            x:    integer to select column in the 2D plane 
//
//	Returns:		count of live cells 
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

int	getCount(char arr[][ROW][COL], int idx, Life *life, int y, int x)
{
	int carr[8][2] = {	{ -1, -1 },{ -1,  0 },{ -1, +1 },
						{  0, -1 },			  {  0, +1 },
						{ +1, -1 },{ +1,  0 },{ +1, +1 } };
	int count = 0;		// count of live neighbors 
	int i;
	int xc;				// X coord of Cell to be Checked 
	int	yc;				// Y coord of Cell to be Checked 

	for (i = 0; i < 8; i++)
	{
		xc = x + carr[i][1];
		yc = y + carr[i][0];
		if (!((xc < 0) || (xc >= life->col) || (yc < 0) || (yc >= life->row)))
			count += isAlive(arr, idx, life, yc, xc);
	}
	return count;

}	// end getCount


//-----------------------------------------------------------------------------
//	Function:		birther()
//
//	Title:			Give birts to the next generation of cells
//
//
//	Called by:		playLife()
//					
//	Calls:			getCount()
//					isAlive()
//					setLive()
//					setDead()
//
//	Parameters:		
//			arr:	pointer to a 3D array (world)
//			idx:	integer to select a 2D slice of the above array
//		   life:	pointer to a Life structure containing most variables 
//            y:    integer to select row in the 2D plane 
//            x:    integer to select column in the 2D plane 
//
//	Returns:		N/A
//
//	History log:	11/26/2016	completed v1.0 for CS131
//-----------------------------------------------------------------------------

void birther(char arr[][ROW][COL], int gen, Life *life)
{
	int i;							// row or Y
	int j;							// col or X
	int count[ROW][COL];			// an array to hold the neighboor count
	int curr = gen % GEN;			// index into current life
	int next = (gen + 1) % GEN;		// index into the future 

	// initialize count of live cells to zero (used for display), and next gen 
	life->gens[next] = gen + 1;
	life->cells[next] = 0;

	// fill up the count array 
	for (i = 0; i < life->row; i++)
		for (j = 0; j < life->col; j++)
			count[i][j] = getCount(arr, curr, life, i, j);

	// decide what to do based on neighbor count 
	for (i = 0; i < life->row; i++)
		for (j = 0; j < life->col; j++)
			switch (count[i][j]) {
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				setDead(arr, next, life, i, j);
				break;
			case 2:
				if (isAlive(arr, curr, life, i, j))
					setLive(arr, next, life, i, j);
				else
					setDead(arr, next, life, i, j);
				break;
			case 3:
				setLive(arr, next, life, i, j);
				break;
			default:
				printf("\ncount[%d][%d]: %d is out of range in birther!! \n",
							i, j, count[i][j]); 
				getchar(); 
				break;
			}

}	// end birther()
