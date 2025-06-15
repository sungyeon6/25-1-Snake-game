#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>

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
	bool operator==(const Vector &v)
	{
		return getCol() == v.getCol() && getRow() == v.getRow();
	}

private:
	int x, y;
};

class Snake
{
private:
	std::vector<Vector> body;
	Vector direction;
	const int height;
	const int width;
	bool exit;

public:
	Snake(int _height, int _width)
		: height(_height), width(_width), exit(false)
	{
		int startX = width / 2;
		int startY = height / 2;
		body.push_back(Vector(startX, startY));
		body.push_back(Vector(startX - 1, startY));
		body.push_back(Vector(startX - 2, startY));
		direction = Vector(1, 0);
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
	void move()
	{
		Vector next = body[0] + direction;
		if (next.getCol() <= 0 || next.getCol() >= width - 1 || next.getRow() <= 0 || next.getRow() >= height - 1)
		{
			exit = true;
			return;
		}
		for (size_t i = body.size() - 1; i > 0; --i)
		{
			body[i] = body[i - 1];
		}
		body[0] = next;
	}
	const std::vector<Vector> &getBody() const { return body; }
	bool getQuit() { return exit; }
	void setQuit(bool value) { exit = value; }
	int getDirection()
	{
		if (direction.getCol() == 1)
			return 1;
		if (direction.getCol() == -1)
			return 3;
		if (direction.getRow() == -1)
			return 0;
		return 2;
	}
};

int main()
{
	initscr();
	noecho();
	cbreak();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);

	while (true)
	{
		clear();
		mvprintw(1, 1, "Press Spacebar/Enter to start or q to quit");
		refresh();
		int ch = getch();
		if (ch == 'q')
			break;
		if (!(ch == ' ' || ch == 10))
			continue;

		WINDOW *window1 = newwin(24, 60, 3, 3);
		WINDOW *window2 = newwin(10, 35, 3, 65);
		WINDOW *window3 = newwin(10, 35, 14, 65);
		WINDOW *message = newwin(2, 60, 28, 3);

		wbkgd(window1, COLOR_PAIR(1));
		wbkgd(window2, COLOR_PAIR(1));
		wbkgd(window3, COLOR_PAIR(1));
		wbkgd(message, COLOR_PAIR(1));

		wborder(window1, '*', '*', '*', '*', '@', '@', '@', '@');
		wborder(window2, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		wborder(window3, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

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

		Snake snake(24, 60);

		while (!snake.getQuit())
		{
			int input = wgetch(window1);
			int d = snake.getDirection();
			switch (input)
			{
			case KEY_UP:
				if (d != 2)
					snake.setDirection(0);
				else
					snake.setQuit(true);
				break;
			case KEY_RIGHT:
				if (d != 3)
					snake.setDirection(1);
				else
					snake.setQuit(true);
				break;
			case KEY_DOWN:
				if (d != 0)
					snake.setDirection(2);
				else
					snake.setQuit(true);
				break;
			case KEY_LEFT:
				if (d != 1)
					snake.setDirection(3);
				else
					snake.setQuit(true);
				break;
			case 'q':
				snake.setQuit(true);
				break;
			}

			snake.move();
			werase(window1);
			wborder(window1, '*', '*', '*', '*', '@', '@', '@', '@');
			const auto &body = snake.getBody();
			for (size_t i = 0; i < body.size(); ++i)
			{
				char ch = (i == 0) ? 'O' : 'o';
				mvwaddch(window1, body[i].getRow(), body[i].getCol(), ch);
			}
			wrefresh(window1);
			wrefresh(window2);
			wrefresh(window3);
			usleep(100000);
		}

		mvwprintw(message, 0, 2, "Game Over! Press Spacebar/Enter to continue or q to quit.");
		wrefresh(message);
		nodelay(window1, FALSE);
		wgetch(window1);

		delwin(window1);
		delwin(window2);
		delwin(window3);
		delwin(message);
	}
	endwin();
	return 0;
}
