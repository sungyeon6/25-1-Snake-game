#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
using std::fstream;
using std::ifstream;
using std::ofstream;

int countA;
int countP;
int countG;
#define MAXlen 10
class Vector
{
public:
	Vector() : x(0), y(0) {}
	Vector(int x, int y)
	{
		setCol(x);
		setRow(y);
	}

	void setCol(int x) { this->x = x; }
	int getCol() const { return this->x; }
	void setRow(int y) { this->y = y; }
	int getRow() const { return this->y; }

	Vector &operator=(const Vector &v)
	{
		setCol(v.getCol());
		setRow(v.getRow());
		return *this;
	}
	Vector &operator+=(const Vector &v)
	{
		setCol(getCol() + v.getCol());
		setRow(getRow() + v.getRow());
		return *this;
	}
	Vector operator+(const Vector &v)
	{
		Vector vector(getCol(), getRow());
		return vector += v;
	}
	Vector &operator-=(const Vector &v)
	{
		setCol(getCol() - v.getCol());
		setRow(getRow() - v.getRow());
		return *this;
	}
	Vector operator-(const Vector &v)
	{
		Vector vector(getCol(), getRow());
		return vector -= v;
	}
	bool operator==(const Vector &v)
	{
		return getCol() == v.getCol() && getRow() == v.getRow();
	}
	bool operator!=(const Vector &v)
	{
		return !operator==(v);
	}

private:
	int x, y;
};

class Snake
{
private:
	std::vector<Vector> body;
	Vector growth;
	Vector poison;
	Vector direction;
	int points;
	int level;
	int best;
	const int height;
	const int width;
	bool exit;
	int speed;
	char *table;

public:
	Snake(int _height, int _width, int _best)
		: best(_best), height(_height), width(_width)
	{
		getGrowth();
		getPoison();
		int startX = width / 2;
		int startY = height / 2;
		body.push_back(Vector(startX, startY));
		body.push_back(Vector(startX - 1, startY));
		body.push_back(Vector(startX - 2, startY));
		setDirection(0);
		exit = false;
		speed = 80;
		level = 1;
		points = 0;
		srand(time(NULL));
		table = new char[height * width];
	}
	~Snake()
	{
		delete[] table;
	}

	void getGrowth()
	{
		countA++;
		while (true)
		{
			int x = rand() % width;
			int y = rand() % height;
			Vector candidate(x, y);

			bool overlap = false;
			for (const auto &segment : body)
			{
				if (candidate == segment)
				{
					overlap = true;
					break;
				}
			}
			if (!overlap)
			{
				growth = candidate;
				break;
			}
		}
	}

	void getPoison()
	{
		countP++;
		while (true)
		{
			int x = rand() % width;
			int y = rand() % height;
			Vector candidate(x, y);

			bool overlap = false;
			for (const auto &segment : body)
			{
				if (candidate == segment || candidate == growth)
				{
					overlap = true;
					break;
				}
			}
			if (!overlap)
			{
				poison = candidate;
				break;
			}
		}
	}

	void setDirection(int d)
	{
		switch (d)
		{
		case 0:
			direction = Vector(0, -1);
			break;
		case 1:
			direction = Vector(1, 0);
			break;
		case 2:
			direction = Vector(0, 1);
			break;
		case 3:
			direction = Vector(-1, 0);
			break;
		}
	}

	void makeMove()
	{
		exit = false;
		if (body[0].getCol() < 0 || body[0].getCol() >= width)
			exit = true;
		else if (body[0].getCol() == 0)
		{
			if (direction == Vector(-1, 0))
				exit = true;
		}
		else if (body[0].getCol() == (width - 1))
		{
			if (direction == Vector(1, 0))
				exit = true;
		}

		if (body[0].getRow() < 0 || body[0].getRow() >= height)
			exit = true;
		else if (body[0].getRow() == 0)
		{
			if (direction == Vector(0, -1))
				exit = true;
		}
		else if (body[0].getRow() == (height - 1))
		{
			if (direction == Vector(0, 1))
				exit = true;
		}

		if (!exit)
			body[0] += direction;

		for (unsigned int i = 1; i < body.size(); ++i)
			if (body[i] == body[0])
			{
				exit = true;
				break;
			}
	}
	void bodyMove()
	{
		for (unsigned int i = body.size() - 1; i > 0; --i)
			snakeSwap(i);
	}
	void checkForGrowth()
	{
		if (body[0] == growth)
		{
			points++;
			if (points > best)
				best = points;
			getGrowth();
			snakeSwap(body.size());
		}
	}
	void checkForPoison()
	{
		if (body[0] == poison)
		{
			points--;
			getPoison();
			snakeSwap2(body.size());

			if (body.size() < 3)
				exit = true;
		}
	}
	void checkPoints()
	{
		switch (points)
		{
		case 3:
			level = 2;
			speed = 98;
			break;
		case 5:
			level = 3;
			speed = 85;
			break;
		case 7:
			level = 4;
			speed = 80;
			break;
		case 9:
			level = 5;
			speed = 60;
			break;
		case 50:
			level = 6;
			speed = 80;
			break;
		case 60:
			level = 7;
			speed = 65;
			break;
		case 70:
			level = 8;
			speed = 50;
			break;
		case 80:
			level = 9;
			speed = 40;
			break;
		case 90:
			level = 10;
			speed = 30;
			break;
		case 100:
			level = 999;
			speed = 15;
			break;
		}
	}
	void snakeSwap(uint i)
	{
		if (i >= body.size())
			body.push_back(body.back());
		else
			body[i] = body[i - 1];
	}

	void snakeSwap2(uint i)
	{
		if (i >= body.size())
			body.pop_back();
		else
			body[i] = body[i - 1];
	}
	int getPoints() { return points; }
	int getLevel() { return level; }
	int getBest() { return best; }
	bool getExit() { return exit; }
	void setExit(bool e) { exit = e; }
	int getSpeed() { return speed; }
	int getHeight() { return height; }
	int getWidth() { return width; }
	char *getTable()
	{
		std::memset(table, ' ', height * width);

		table[body[0].getRow() * width + body[0].getCol()] = 'h';
		for (unsigned int i = 1; i < body.size(); ++i)
			table[body[i].getRow() * width + body[i].getCol()] = 'b';
		table[growth.getRow() * width + growth.getCol()] = 'a';
		table[poison.getRow() * width + poison.getCol()] = 'x';

		return table;
	}
	int getDirection()
	{
		if (direction.getCol() == 1)
			return 1;
		else if (direction.getCol() == -1)
			return 3;
		else if (direction.getRow() == -1)
			return 0;
		else
			return 2;
	}
};

int getBest();
void writeBest(int best);
bool writeEndAndGetInput();
void printScore(WINDOW *, int, int, int);
void printMission(WINDOW *, int, int, int);
void draw(WINDOW *, Snake &, char *, int, int);
void proccesInput(WINDOW *, Snake &, int);
int main()
{
	countA = 0;
	countP = 0;
	countG = 0;

	initscr();
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);

	int best = getBest();

	WINDOW *window1 = newwin(24, 60, 3, 3);
	WINDOW *window2 = newwin(10, 35, 3, 65);
	WINDOW *window3 = newwin(10, 35, 14, 65);
	WINDOW *message = newwin(2, 60, 28, 3);

	wbkgd(window1, COLOR_PAIR(1));
	wbkgd(window2, COLOR_PAIR(1));
	wbkgd(window3, COLOR_PAIR(1));
	wbkgd(message, COLOR_PAIR(1));

	wborder(window1, '*', '*', '*', '*', '@', '@', '@', '@');
	wborder(window2, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
			ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wborder(window3, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
			ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

	mvwprintw(window2, 1, 2, "Score Board");
	mvwprintw(window2, 4, 2, "B: Current Length / Max Length");
	mvwprintw(window2, 5, 2, "+: Acquired Growth Items");
	mvwprintw(window2, 6, 2, "-: Acquired Poison Items");
	mvwprintw(window2, 7, 2, "G: Number of Gate uses");

	mvwprintw(window3, 1, 1, "Mission");
	mvwprintw(window3, 4, 1, "B: 10  ( )");
	mvwprintw(window3, 5, 1, "+: 5   ( )");
	mvwprintw(window3, 6, 1, "-: 2   ( )");
	mvwprintw(window3, 7, 1, "G: 1   (v)");

	wrefresh(window1);
	wrefresh(window2);
	wrefresh(window3);
	wrefresh(message);

	nodelay(window1, TRUE);
	keypad(window1, TRUE);

	do
	{
		Snake snake(24, 60, best);
		refresh();
		wrefresh(window1);
		printScore(window2, 0, 1, best);
		printMission(window3, 0, 1, best);
		countA = 0;
		countP = 0;
		countG = 0;

		while (!snake.getExit())
		{
			char *tbl = snake.getTable();
			draw(window1, snake, tbl, snake.getHeight(), snake.getWidth());

			printScore(window2, snake.getPoints(), snake.getLevel(), snake.getBest());
			printMission(window3, snake.getPoints(), snake.getLevel(), snake.getBest());

			int input = wgetch(window1);
			proccesInput(window1, snake, input);
			if (snake.getExit())
				break;

			snake.bodyMove();
			snake.makeMove();
			snake.checkForGrowth();
			snake.checkForPoison();
			snake.checkPoints();

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

	delwin(window2);
	delwin(window1);
	delwin(window3);
	endwin();
}

std::string getFile()
{
#ifdef WIN32
	std::string home = "%appdata%/.md.snake\0";
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
	WINDOW *endwin = newwin(4, COLS / 2, LINES / 2, 2);
	box(endwin, 0, 0);
	nodelay(endwin, TRUE);
	keypad(endwin, TRUE);
	mvwprintw(endwin, 1, 1, "Press Spacebar/Enter to play again");
	mvwprintw(endwin, 2, 1, "Press q to quit");
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
	for (int i = 0; i < (height * width); ++i)
	{
		if (table[i] != ' ')
		{
			int y = i / width;
			int x = i - (y * width);
			int ch;
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
