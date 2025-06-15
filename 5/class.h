#include "vector.h"
#include <ctime>

int countA;
int countP;
int countG;
int countB;
int countW;
char comA;
char comP;
char comG;
char comB;
int missionA;
int missionP;
int missionB;
int missionG;

time_t nowGameT;
time_t startAT;
time_t nowAT;
time_t startPT;
time_t nowPT;
int itemT = 6;

using namespace std;
int map[30][60];

class Snake
{
private:
	vector<Vector> body;
	Vector growth;
	Vector poison;
	Vector wall1[1000];
	Vector gate[2];
	Vector direction;
	int points;
	int level;
	int best;
	const int height;
	const int width;
	bool shouldExit;
	int speed;
	char *table;
	bool end;

public:
	Snake(int _height, int _width)
		: height(_height), width(_width)
	{
		init(map);
		dropGrowth(map);
		dropPoison(map);
		checkWall(map);
		getGate(wall1);
		body.push_back(Vector(width / 2, height / 2));
		body.push_back(Vector(width / 2 + 1, height / 2));
		body.push_back(Vector(width / 2 + 2, height / 2));
		setDirection(0);
		shouldExit = false;
		speed = 100000;
		level = 1;
		missionA = 1;
		missionB = 5;
		missionP = 1;
		missionG = 1;
		points = 0;
		srand(time(NULL));
		table = new char[height * width];
	}
	~Snake()
	{
		delete[] table;
	}

	void init(int map[30][60])
	{
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				map[i][j] = map1[i][j];
			}
		}
	}

	void mapChange(int next_map[30][60])
	{
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				map[i][j] = next_map[i][j];
			}
		}
		checkWall(next_map);
	}

	void getGate(Vector wall[])
	{
		int A = rand() % countW;
		int B = rand() % countW;
		if (B == A)
			getGate(wall1);
		gate[0] = wall1[A];
		gate[1] = wall1[B];
	}

	void dropGrowth(int map[30][60])
	{
		startAT = time(NULL);

		int x = rand() % width;
		int y = rand() % height;
		growth = Vector(x, y);
		for (unsigned int i = 0; i < body.size(); ++i)
			if (growth == body[i])
				dropGrowth(map);
		if (map[y][x] != 0)
			dropGrowth(map);
	}

	void getSize()
	{
		if (body.size() < 3)
		{
			shouldExit = true;
		}
	}

	void dropPoison(int map[30][60])
	{
		startPT = time(NULL);

		int x, y;
		x = rand() % width;
		y = rand() % height;
		poison = Vector(x, y);
		for (unsigned int i = 0; i < body.size(); ++i)
			if (poison == body[i])
				dropPoison(map);

		if (map[y][x] != 0)
			dropPoison(map);
	}

	void checkWall(int map[30][60])
	{

		countW = 0;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
				if (map[i][j] == 1)
				{
					wall1[countW++] = Vector(j, i);
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

	void changeBody()
	{

		for (int i = 0; i < body.size() - 3; i++)
		{
			reduceBodySize(body.size());
		}
	}

	void makeMove()
	{
		if (!shouldExit)
			body[0] += direction;

		for (unsigned int i = 1; i < body.size(); ++i)
			if (body[i] == body[0])
			{
				shouldExit = true;
				break;
			}
	}

	void bodyMove()
	{
		for (unsigned int i = body.size() - 1; i > 0; --i)
			increaseBodySize(i);
	}

	void checkForGrowth()
	{
		nowAT = time(NULL);

		if (nowAT - startAT > itemT)
		{
			dropGrowth(map);
		}
		if (body[0] == growth)
		{
			countA++;
			points++;
			if (points > best)
				best = points;
			dropGrowth(map);
			increaseBodySize(body.size());
		}
	}
	void checkForPoison()
	{

		nowPT = time(NULL);

		if (nowPT - startPT > itemT)
		{
			dropPoison(map);
		}

		if (body[0] == poison)
		{
			countP++;
			points--;
			dropPoison(map);
			reduceBodySize(body.size());
		}
	}

	void checkForwall()
	{
		for (int i = 0; i < countW; i++)
		{
			if (body[0] == wall1[i] && body[0] != gate[0] && body[0] != gate[1])
			{
				shouldExit = true;
			}
		}
	}

	void checkGate()
	{
		if (body[0] == gate[0])
		{
			body[0] = gate[1];
			countG++;
			setDirection(gateDirection(gate[1]));
		}

		else if (body[0] == gate[1])
		{
			countG++;
			body[0] = gate[0];
			setDirection(gateDirection(gate[0]));
		}
	}

	void changeGate()
	{
		Vector A = direction;
		for (int i = 0; i < body.size(); i++)
		{
			A = A + direction;
		}
		if (body[0] == gate[0] + A)
		{
			getGate(wall1);
		}

		else if (body[0] == gate[1] + A)
		{
			getGate(wall1);
		}
	}

	int gateDirection(Vector gate)
	{
		Vector A = gate + Vector(0, -1);
		if (map[A.getRow()][A.getCol()] == 0)
			return 0;
		Vector B = gate + Vector(1, 0);
		if (map[B.getRow()][B.getCol()] == 0)
			return 1;
		Vector C = gate + Vector(0, 1);
		if (map[C.getRow()][C.getCol()] == 0)
			return 2;
		Vector D = gate + Vector(-1, 0);
		if (map[D.getRow()][D.getCol()] == 0)
			return 3;
		return -1;
	};

	void mission()
	{
		if (missionA <= countA)
		{
			comA = 'v';
		}
		else
			comA = ' ';
		if (missionP <= countP)
		{

			comP = 'v';
		}
		else
			comP = ' ';
		if (missionG <= countG)
		{

			comG = 'v';
		}
		else
			comG = ' ';
		if (missionB <= body.size())
		{

			comB = 'v';
		}
		else
			comB = ' ';
	}

	int missionComplete()
	{
		if (comP == 'v' && comA == 'v' && comG == 'v' && comB == 'v')
		{
			level++;
			return level;
		}
		return level;
	}

	bool allComplete()
	{
		if (level > 5)
		{
			shouldExit = true;
			return true;
		}
		return false;
	}

	void checkPoints()
	{
		switch (missionComplete())
		{
		case 2:
		{
			missionA = 2;
			missionB = 4;
			missionP = 2;
			missionG = 2;
			mapChange(map2);
			break;
		}
		case 3:
		{
			missionA = 3;
			missionB = 5;
			missionP = 3;
			missionG = 3;
			mapChange(map3);
			break;
		}
		case 4:
		{
			missionA = 4;
			missionB = 6;
			missionP = 4;
			missionG = 4;
			mapChange(map4);
			break;
		}
		case 5:
		{
			missionA = 5;
			missionB = 7;
			missionP = 5;
			missionG = 5;
			mapChange(map5);
			break;
		}
		}
	}

	char *checkMap()
	{
		if (level == 1)
		{
			return getTable(map1);
		}
		if (level == 2)
		{
			return getTable(map2);
		}
		if (level == 3)
		{
			return getTable(map3);
		}
		if (level == 4)
		{
			return getTable(map4);
		}
		if (level == 5)
		{
			return getTable(map5);
		}
		return getTable(map5);
	}

	void increaseBodySize(uint i)
	{
		if (i >= body.size())
			body.push_back(body.back());
		else
			body[i] = body[i - 1];
	}

	void reduceBodySize(uint i)
	{
		if (i >= body.size())
			body.pop_back();
		else
			body[i] = body[i - 1];
	}

	int getBodySize() { return body.size(); }
	int getPoints() { return points; }
	int getLevel() { return level; }
	bool getExit() { return shouldExit; }
	bool getEnd() { return end; }
	void setExit(bool e) { shouldExit = e; }
	int getSpeed() { return speed; }
	int getHeight() { return height; }
	int getWidth() { return width; }

	char *getTable(int map[30][60])
	{
		std::memset(table, ' ', height * width);

		table[body[0].getRow() * width + body[0].getCol()] = 'h';
		for (unsigned int i = 1; i < body.size(); ++i)
			table[body[i].getRow() * width + body[i].getCol()] = 'b';

		table[growth.getRow() * width + growth.getCol()] = 'a';
		table[poison.getRow() * width + poison.getCol()] = 'x';

		for (unsigned int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				if (map[i][j] == 1)
					table[i * width + j] = '1';
				else if (map[i][j] == 2)
					table[i * width + j] = '2';
				else if (map[i][j] == 3)
					table[i * width + j] = '3';
			}
		}
		table[gate[0].getRow() * width + gate[0].getCol()] = 'G';
		table[gate[1].getRow() * width + gate[1].getCol()] = 'G';

		return table;
	}

	int getDirection()
	{
		if (direction.getRow() == -1)
			return 0;
		else if (direction.getCol() == 1)
			return 1;
		else if (direction.getRow() == 1)
			return 2;
		else
			return 3;
	};
};