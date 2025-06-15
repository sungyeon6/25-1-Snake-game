#include "class.h"
#include <locale.h>
#include <ctime>

time_t startGT;
time_t overGT;

using std::fstream;
using std::ifstream;
using std::ofstream;

bool writeEndAndGetInput();
void printScore(WINDOW *, int, int, int);
void printMission(WINDOW *, int, int);
bool printEndGame();
void draw(WINDOW *, Snake &, char *, int, int);
void proccesInput(WINDOW *, Snake &, int);
int MAP(int x, int y);
bool com;

int main()
{

	setlocale(LC_ALL, "");
	countA = 0;
	countP = 0;
	countG = 0;
	comA = ' ';
	comB = ' ';
	comG = ' ';
	comP = ' ';
	int sz;
	int x, y;
	x = 100;
	y = 30;
	int x1 = 100;
	int x2 = 200;

	initscr();
	refresh();
	noecho();
	cbreak();
	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);

	endwin();

	WINDOW *win = newwin(y + 2, x - 38, 0, 0);

	wbkgd(win, COLOR_PAIR(1));
	wattron(win, COLOR_PAIR(1));
	wborder(win, '*', '*', '*', '*', '@', '@', '@', '@');

	nodelay(win, TRUE);
	keypad(win, TRUE);

	WINDOW *score = newwin(y / 2, x - 60, 0, x - 37);
	wbkgd(score, COLOR_PAIR(1));
	wattron(score, COLOR_PAIR(1));
	wborder(score, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wrefresh(score);
	nodelay(score, TRUE);

	WINDOW *mission = newwin(y / 2, x - 60, y / 2, x - 37);
	wbkgd(mission, COLOR_PAIR(1));
	wattron(mission, COLOR_PAIR(1));
	wborder(mission, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wrefresh(mission);
	nodelay(mission, TRUE);

	do
	{
		startGT = time(NULL);
		Snake snake(y, x - 40);
		refresh();
		wrefresh(win);
		sz = snake.getBodySize();
		printScore(score, 0, 1, sz);
		printMission(mission, 0, 1);
		countA = 0;
		countP = 0;
		countG = 0;

		while (!snake.getExit() || !snake.getEnd())
		{
			char *tbl = snake.checkMap();
			draw(win, snake, tbl, snake.getHeight(), snake.getWidth());

			printScore(score, snake.getPoints(), snake.getLevel(), snake.getBodySize());
			printMission(mission, snake.getPoints(), snake.getLevel());

			int input = wgetch(win);
			proccesInput(win, snake, input);
			if (snake.getExit())
				break;
			snake.checkPoints();
			snake.bodyMove();
			snake.makeMove();
			snake.checkForGrowth();
			snake.checkForPoison();
			snake.checkForwall();
			snake.checkGate();
			snake.changeGate();
			snake.mission();
			snake.getSize();
			com = snake.allComplete();

#ifdef WIN32
#else
			usleep(snake.getSpeed());
#endif
		}
	} while (writeEndAndGetInput());

	delwin(score);
	delwin(win);
	delwin(mission);
	endwin();
}

bool writeEndAndGetInput()
{
	if (com)
	{
		overGT = time(NULL);
		int score = countA + countB - countP + countG * 2;
		WINDOW *endwin = newwin(32, 110, 0, 0);
		box(endwin, 0, 0);
		nodelay(endwin, TRUE);
		keypad(endwin, TRUE);
		mvwprintw(endwin, 14, 40, "Nice Job!");
		mvwprintw(endwin, 15, 40, "Game Score : %d", score);
		mvwprintw(endwin, 16, 40, "Play Time : %d second", overGT - startGT);
		mvwprintw(endwin, 17, 40, "Total Score : %d", score + 1000 - (overGT - startGT));
		mvwprintw(endwin, 18, 40, "Press q to quit.");
		int c;
		do
		{
			c = wgetch(endwin);
		} while (c != 10 && c != ' ' && c != 'q');
		werase(endwin);
		wrefresh(endwin);
		delwin(endwin);
		return (c == 'q') ? false : true;
	}
	else
		overGT = time(NULL);
	int score = countA + countB - countP + countG * 2;
	WINDOW *endwin = newwin(32, 110, 0, 0);
	box(endwin, 0, 0);
	nodelay(endwin, TRUE);
	keypad(endwin, TRUE);
	mvwprintw(endwin, 14, 40, "GAME OVER!");
	mvwprintw(endwin, 15, 40, "Game Score : %d", score);
	mvwprintw(endwin, 16, 40, "Play Time : %d second", overGT - startGT);
	mvwprintw(endwin, 17, 40, "Total Score : %d", score + 1000 - (overGT - startGT));
	mvwprintw(endwin, 18, 40, "Press q to quit");
	int c;
	do
	{
		c = wgetch(endwin);
	} while (c != 10 && c != ' ' && c != 'q' && c != 'Q');
	werase(endwin);
	wrefresh(endwin);
	delwin(endwin);
	return (c == 'q') ? false : true;
}

void printScore(WINDOW *w, int score, int level, int sz)
{
	werase(w);
	wbkgd(w, COLOR_PAIR(1));
	wattron(w, COLOR_PAIR(1));
	wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	mvwprintw(w, 1, 2, "Score Board");
	mvwprintw(w, 4, 2, "B: Current Length / Max Length %d/%d", sz, missionB);
	mvwprintw(w, 6, 2, "+: Number of Growth Items %d", countA);
	mvwprintw(w, 8, 2, "-: Number of Poison Items %d", countP);
	mvwprintw(w, 10, 2, "G: Number of gate used %d", countG);
	wrefresh(w);
}

void printMission(WINDOW *w, int score, int level)
{
	werase(w);
	wbkgd(w, COLOR_PAIR(1));
	wattron(w, COLOR_PAIR(1));
	wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	mvwprintw(w, 1, 2, "Mission Board");
	mvwprintw(w, 4, 2, "B: %d   (%c)", missionB, comB);
	mvwprintw(w, 5, 2, "+: %d   (%c)", missionA, comA);
	mvwprintw(w, 6, 2, "-: %d   (%c)", missionP, comP);
	mvwprintw(w, 7, 2, "G: %d   (%c)", missionG, comG);
	wrefresh(w);
}

void draw(WINDOW *win, Snake &snake, char *table, int height, int width)
{
	werase(win);
	wbkgd(win, COLOR_PAIR(1));
	wattron(win, COLOR_PAIR(1));
	wborder(win, '*', '*', '*', '*', '@', '@', '@', '@');

	for (int i = 0; i < (height * width); ++i)
	{
		if (table[i] != ' ')
		{

			int y = i / width;
			int x = i - (y * width);

			int d;
			switch (table[i])
			{
			case 'a':
				mvwprintw(win, 1 + y, 1 + x, "G");
				break;
			case 'x':
				mvwprintw(win, 1 + y, 1 + x, "P");
				break;
			case 'h':
				mvwprintw(win, 1 + y, 1 + x, "O");
				break;
			case 'b':
				mvwprintw(win, 1 + y, 1 + x, "o");
				break;
			case '1':
				mvwprintw(win, 1 + y, 1 + x, "-");
				break;
			case '2':
				mvwprintw(win, 1 + y, 1 + x, "+");
				break;
			case '3':
				mvwprintw(win, 1 + y, 1 + x, " ");
				break;
			case 'G':
				mvwprintw(win, 1 + y, 1 + x, " ");
				break;
			}
		}
	}
	wrefresh(win);
}

void proccesInput(WINDOW *win, Snake &snake, int input)
{
	int d = snake.getDirection();
	switch (input)
	{
	case KEY_UP:
		if (d == 2)
		{
			snake.setExit(true);
			break;
		}
		if (d != 0)
			snake.setDirection(0);
		break;
	case KEY_DOWN:
		if (d == 0)
		{
			snake.setExit(true);
			break;
		}
		if (d != 2)
			snake.setDirection(2);
		break;
	case KEY_LEFT:
		if (d == 1)
		{
			snake.setExit(true);
			break;
		}
		if (d != 3)
			snake.setDirection(3);
		break;
	case KEY_RIGHT:
		if (d == 3)
		{
			snake.setExit(true);
			break;
		}
		if (d != 1)
			snake.setDirection(1);
		break;
	case 'q':
		snake.setExit(true);
		break;
	}
}
