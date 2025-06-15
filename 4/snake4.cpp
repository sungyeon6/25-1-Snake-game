#include "class.h"
using std::fstream;
using std::ifstream;
using std::ofstream;

#define MAXlen 10
int getBest();
void writeBest(int best);
bool writeEndAndGetInput();
void printScore(WINDOW *, int, int, int);
void printMission(WINDOW *, int, int, int);
void draw(WINDOW *, Snake &, char *, int, int);
void proccesInput(WINDOW *, Snake &, int);
int MAP(int x, int y);
int main()
{
	countA = 0;
	countP = 0;
	countG = 0;
	initscr();
	noecho();
	cbreak();
	int x, y;
	x = 100;
	y = 30;
	int best = getBest();

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	mvprintw(1, 1, "Press Spacebar/Enter to start or q to quit");
	refresh();

	getch();
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
		Snake snake(y, x - 40, best);
		refresh();
		wrefresh(win);
		printScore(score, 0, 1, best);
		printMission(mission, 0, 1, best);
		countA = 0;
		countP = 0;
		countG = 0;

		while (!snake.getExit())
		{
			char *tbl = snake.checkMap();
			draw(win, snake, tbl, snake.getHeight(), snake.getWidth());

			printScore(score, snake.getPoints(), snake.getLevel(), snake.getBest());
			printMission(mission, snake.getPoints(), snake.getLevel(), snake.getBest());

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

			if (snake.getBody().size() < 3)
			{
				snake.setExit(true);
				break;
			}

#ifdef WIN32
			Sleep(snake.getSpeed());
#else
			usleep(snake.getSpeed() * 1000);
#endif
		}
		if (snake.getBest() > best)
		{
			writeBest(snake.getBest());
			best = snake.getBest();
		}
	} while (writeEndAndGetInput());

	delwin(score);
	delwin(win);
	delwin(mission);
	endwin();
}
std::string getFile()
{
#ifdef WIN32

#else
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	std::string home = std::string(homedir) + "/.md.snake\0";
#endif
	return home;
}
int getBest()
{
	ifstream fp(getFile().c_str());
	int best = 0;
	std::string content;
	if (fp.is_open() && fp.good())
	{
		getline(fp, content);
		best = strtol(content.c_str(), NULL, 10);
		fp.close();
	}
	return best;
}
void writeBest(int best)
{
	ofstream fp(getFile().c_str());
	if (fp.is_open())
	{
		fp << best;
		fp.close();
	}
}
bool writeEndAndGetInput()
{
	WINDOW *endwin = newwin(32, 110, 0, 0);
	box(endwin, 0, 0);
	nodelay(endwin, TRUE);
	keypad(endwin, TRUE);
	mvwprintw(endwin, 14, 40, "GAME OVER!");
	mvwprintw(endwin, 15, 40, "Press Spacebar/Enter to play again or q to quit.");
	int c;
	do
	{
		c = wgetch(endwin);
	} while (c != 10 && c != ' ' && c != 'q');
	werase(endwin);
	wrefresh(endwin);
	delwin(endwin);
	return (c == 'q' || c == 'Q') ? false : true;
}
void printScore(WINDOW *w, int score, int level, int best)
{
	werase(w);
	wbkgd(w, COLOR_PAIR(1));
	wattron(w, COLOR_PAIR(1));
	wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	mvwprintw(w, 1, 2, "Score Board");
	mvwprintw(w, 4, 2, "B: Current Length / Max Length %d/%d", score, MAXlen);
	mvwprintw(w, 6, 2, "+: Number of Growth Items %d", countA);
	mvwprintw(w, 8, 2, "-: Number of Poison Items %d", countP);
	mvwprintw(w, 10, 2, "G: Number of gate used %d", countG);
	wrefresh(w);
}

void printMission(WINDOW *w, int score, int level, int best)
{
	werase(w);
	wbkgd(w, COLOR_PAIR(1));
	wattron(w, COLOR_PAIR(1));
	wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	mvwprintw(w, 1, 2, "Mission");

	mvwprintw(w, 4, 2, "B: 10  (%d)", score / 5);
	mvwprintw(w, 5, 2, "+: 5   (%d)", countA);
	mvwprintw(w, 6, 2, "-: 2   (%d)", countP);
	mvwprintw(w, 7, 2, "G: 1   (%d)", countG);
	wrefresh(w);
}

void draw(WINDOW *win, Snake &snake, char *table, int height, int width)
{
	werase(win);
	wbkgd(win, COLOR_PAIR(1));
	wattron(win, COLOR_PAIR(1));
	wborder(win, '*', '*', '*', '*', '@', '@', '@', '@');
	int ch;

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
				ch = 'G';
				break;
			case 'x':
				ch = 'P';
				break;
			case 'h':
				ch = 'O';
				break;
			case 'b':
				ch = 'o';
				break;
			case '1':
				ch = '-';
				break;
			case '2':
				ch = '+';
				break;
			case '3':
				ch = ' ';
				break;
			case 'G':
				ch = ' ';
				break;
			}
			mvwaddch(win, 1 + y, 1 + x, ch);
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
		if (d != 0 && !(d == 2 && snake.getPoints() > 0))
			snake.setDirection(0);
		break;
	case KEY_DOWN:
		if (d == 0)
		{
			snake.setExit(true);
			break;
		}
		if (d != 2 && !(d == 0 && snake.getPoints() > 0))
			snake.setDirection(2);
		break;
	case KEY_LEFT:
		if (d == 1)
		{
			snake.setExit(true);
			break;
		}
		if (d != 3 && !(d == 1 && snake.getPoints() > 0))
			snake.setDirection(3);
		break;
	case KEY_RIGHT:
		if (d == 3)
		{
			snake.setExit(true);
			break;
		}
		if (d != 1 && !(d == 3 && snake.getPoints() > 0))
			snake.setDirection(1);
		break;
	case 'q':
		snake.setExit(true);
		break;
	}
}
