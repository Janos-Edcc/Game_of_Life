#ifndef LIFE_H
#define LIFE_H

// boolean defines
#define TRUE	1
#define FALSE	0

// life related defines 
#define GEN_MIN	2
#define GEN		10
#define GEN_MAX 100
#define	ROW_MIN	2
#define ROW_DEF 20
#define	ROW		140
#define ROW_MAX	400
#define	COL_MIN	3
#define COL		200
#define COL_DEF 40
#define COL_MAX	600

#define DEAD_DEFAULT	'.'
#define LIVE_DEFAULT	'#'
#define DEFAULT_DELAY	30
#define DEFAULT_FNAME	"Life.txt"
#define LIVE_SET		"@$%&*#O0XLl"

#define ENTER	13

// Some haracter definitions 
#define NUL		'\000'
#define BELL	'\007'
#define BS		'\010'
#define TAB		'\011'
#define LF		'\012'
#define CR		'\015'
#define DEL		'\177'
// plus the box drawing characters
#define TOP_LEFT	'\311'
#define TOP_RIGHT	'\273'
#define BOT_LEFT	'\310'
#define BOT_RIGHT	'\274'
#define HOR_LINE	'\315'
#define VERT_LINE	'\272'

// message defines 
#define SPACES			"                                 "
#define COUNT_MIN_STR	"%s count of %d is less than minimum of %d!\n"
#define COUNT_MAX_STR	"%s count of %d is more than maximum of %d!\n"
#define SET_TO_STR		"%s count is set to %d!\n\n"	
#define FILE_ERROR		"\nCouldn't open file '%s' for reading! \n\n%s\n"
#define HIT_ENTER		"Hit <Enter> to continue..."
#define CHAR_NO_GOOD	"\nAn ASCII %d cannot be used to represent %s cells.\n"
#define CHAR_SUBSTED	"Using '%c' to represent %s cells instead.\n"

struct LifeStruct {
	char name[64];		// name of life pattern
	int	 gen;			// generation or Z 
	int	 row;			// Y coord
	int	 col;			// X coord
	char dead;			// character to show dead cell
	char live;			// character to use for a live cell
	int  version;		//  save file version 
	int  delay;			// for game speed - 0 is the fastest
	int	 bgcolor;		// background color
	int  fgcolor; 		// foreground color 
	int	 gens[GEN];		// corresponding heneration 
	int	 cells[GEN];	// count of live cells 
}; 

typedef struct LifeStruct Life; 

// function prototypes 
int  usable  (char ch); 
int  chInStr (char ch, const char* str); 
int  isAlive (char arr[][ROW][COL], int idx, Life *life, int y, int x); 
void setLive (char arr[][ROW][COL], int idx, Life *life, int y, int x);
void setDead (char arr[][ROW][COL], int idx, Life *life, int y, int x); 
int	 getCount(char arr[][ROW][COL], int idx, Life *life, int y, int x);
void birther (char arr[][ROW][COL], int gen, Life *life);
void playLife(char arr[][ROW][COL], int gen, Life *life); 
void showLife(char arr[][ROW][COL], int idx, Life *life); 
void readData(char arr[][ROW][COL], int idx, Life *life, FILE *fp);

// variables for the Win32 API cursor positioning and colors
HANDLE hStdout;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

#endif LIFE_H
