#include "vector.h"
#include <ctime>
#include <vector>
#include <cstring>

int growthCount;
int poisonCount;
int gateCount;
int slowCount;
int maxSizeCount;
int wallCount;
char growthMissionComplete;
char poisonMissionComplete;
char gateMissionComplete;
char slowMissionComplete;
char maxSizeMissionComplete;
int growthMission;
int poisonMission;
int maxSizeMission;
int gateMission;
int slowMission;

time_t currentTime;
time_t startGrowthTime;
time_t currentGrowthTime;
time_t startPoisonTime;
time_t currentPoisonTime;
time_t startSlowTime;
time_t currentSlowTime;
time_t slowEndTime;
bool slowActive;
int itemTimeLimit = 6;

using namespace std;
int map[30][60];

class SnakeGame
{
private:
	vector<Vector> body;
	Vector growth;
	Vector poison;
	Vector slow;
	Vector wall[1000];
	Vector gate[2];
	Vector direction;
	int points;
	int level;
	int best;
	const int height;
	const int width;
	bool exitGame;
	int speed;
	int initialSpeed;
	char *table;

public:
	SnakeGame(int _height, int _width)
		: height(_height), width(_width)
	{
		init(map);
		placeGrowth(map);
		placePoison(map);
		placeSlow(map);
		checkWalls(map);
		placeGate(wall);
		body.push_back(Vector(width / 2, height / 2));
		body.push_back(Vector(width / 2 + 1, height / 2));
		body.push_back(Vector(width / 2 + 2, height / 2));
		setDirection(0);
		exitGame = false;
		initialSpeed = 50000;
		speed = initialSpeed;
		level = 1;
		growthMission = 1;
		maxSizeMission = 5;
		poisonMission = 1;
		gateMission = 1;
		slowMission = 1;
		points = 0;
		slowActive = false;
		srand(time(NULL));
		table = new char[height * width];
	}
	~SnakeGame()
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

	void changeMap(int next_map[30][60])
	{
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				map[i][j] = next_map[i][j];
			}
		}
		checkWalls(next_map);
		resetSpeed();
	}

	void placeGate(Vector wall[])
	{
		int A = rand() % wallCount;
		int B = rand() % wallCount;
		if (B == A)
			placeGate(wall);
		gate[0] = wall[A];
		gate[1] = wall[B];
	}

	void placeGrowth(int map[30][60])
	{
		startGrowthTime = time(NULL);

		int x = rand() % width;
		int y = rand() % height;
		growth = Vector(x, y);
		for (unsigned int i = 0; i < body.size(); ++i)
			if (growth == body[i])
				placeGrowth(map);
		if (map[y][x] != 0)
			placeGrowth(map);
	}

	void placeSlow(int map[30][60])
	{
		int x = rand() % width;
		int y = rand() % height;
		slow = Vector(x, y);
		for (unsigned int i = 0; i < body.size(); ++i)
			if (slow == body[i])
				placeSlow(map);
		if (map[y][x] != 0)
			placeSlow(map);
	}

	void getSize()
	{
		if (body.size() < 3)
		{
			exitGame = true;
		}
	}

	void placePoison(int map[30][60])
	{
		startPoisonTime = time(NULL);

		int x, y;
		x = rand() % width;
		y = rand() % height;
		poison = Vector(x, y);
		for (unsigned int i = 0; i < body.size(); ++i)
			if (poison == body[i])
				placePoison(map);

		if (map[y][x] != 0)
			placePoison(map);
	}

	void checkWalls(int map[30][60])
	{
		wallCount = 0;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
				if (map[i][j] == 1)
				{
					wall[wallCount++] = Vector(j, i);
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

	void move()
	{
		if (!exitGame)
			body[0] += direction;

		for (unsigned int i = 1; i < body.size(); ++i)
			if (body[i] == body[0])
			{
				exitGame = true;
				break;
			}
	}

	void moveBody()
	{
		for (unsigned int i = body.size() - 1; i > 0; --i)
			increaseBodySize(i);
	}

	void checkForGrowth()
	{
		currentGrowthTime = time(NULL);

		if (currentGrowthTime - startGrowthTime > itemTimeLimit)
		{
			placeGrowth(map);
		}
		if (body[0] == growth)
		{
			growthCount++;
			points++;
			if (points > best)
				best = points;
			placeGrowth(map);
			increaseBodySize(body.size());
		}
	}

	void checkForPoison()
	{
		currentPoisonTime = time(NULL);

		if (currentPoisonTime - startPoisonTime > itemTimeLimit)
		{
			placePoison(map);
		}

		if (body[0] == poison)
		{
			poisonCount++;
			points--;
			placePoison(map);
			reduceBodySize(body.size());
		}
	}

	void checkForSlow()
	{
		if (body[0] == slow)
		{
			slowCount++;
			slowActive = true;
			startSlowTime = time(NULL);
			slowEndTime = startSlowTime + 5;
			speed = static_cast<int>(initialSpeed * 1.5);
			placeSlow(map);
		}

		if (slowActive && time(NULL) >= slowEndTime)
		{
			slowActive = false;
			resetSpeed();
		}
	}

	void checkWalls()
	{
		for (int i = 0; i < wallCount; i++)
		{
			if (body[0] == wall[i] && body[0] != gate[0] && body[0] != gate[1])
			{
				exitGame = true;
			}
		}
	}

	void checkGate()
	{
		if (body[0] == gate[0])
		{
			body[0] = gate[1];
			gateCount++;
			setDirection(gateDirection(gate[1]));
		}
		else if (body[0] == gate[1])
		{
			gateCount++;
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
			placeGate(wall);
		}
		else if (body[0] == gate[1] + A)
		{
			placeGate(wall);
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
	}

	void updateMissionStatus()
	{
		if (growthMission <= growthCount)
		{
			growthMissionComplete = 'v';
		}
		else
			growthMissionComplete = ' ';
		if (poisonMission <= poisonCount)
		{
			poisonMissionComplete = 'v';
		}
		else
			poisonMissionComplete = ' ';
		if (gateMission <= gateCount)
		{
			gateMissionComplete = 'v';
		}
		else
			gateMissionComplete = ' ';
		if (maxSizeMission <= body.size())
		{
			maxSizeMissionComplete = 'v';
		}
		else
			maxSizeMissionComplete = ' ';
		if (slowMission <= slowCount)
		{
			slowMissionComplete = 'v';
		}
		else
			slowMissionComplete = ' ';
	}

	int checkMissionCompletion()
	{
		if (poisonMissionComplete == 'v' && growthMissionComplete == 'v' && gateMissionComplete == 'v' && maxSizeMissionComplete == 'v' && slowMissionComplete == 'v')
		{
			level++;
			return level;
		}
		return level;
	}

	void checkPoints()
	{
		switch (checkMissionCompletion())
		{
		case 2:
		{
			growthMission = 2;
			maxSizeMission = 4;
			poisonMission = 2;
			gateMission = 2;
			slowMission = 2;
			changeMap(map2);
			break;
		}
		case 3:
		{
			growthMission = 3;
			maxSizeMission = 5;
			poisonMission = 3;
			gateMission = 3;
			slowMission = 3;
			changeMap(map3);
			break;
		}
		case 4:
		{
			growthMission = 4;
			maxSizeMission = 6;
			poisonMission = 4;
			gateMission = 4;
			slowMission = 4;
			changeMap(map4);
			break;
		}
		case 5:
		{
			growthMission = 5;
			maxSizeMission = 7;
			poisonMission = 5;
			gateMission = 5;
			slowMission = 5;
			changeMap(map5);
			break;
		}
		}
	}

	char *getMap()
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
	bool getExit() { return exitGame; }
	void setExit(bool e) { exitGame = e; }
	int getSpeed() { return speed; }
	void increaseSpeed()
	{
		if (!slowActive)
			speed = static_cast<int>(speed * 0.8);
	}
	void resetSpeed() { speed = initialSpeed; }
	bool isSlowActive() { return slowActive; }
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
		table[slow.getRow() * width + slow.getCol()] = 's';

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
