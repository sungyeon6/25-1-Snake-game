#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "map.h"
using namespace std;

#define MAXlen 7
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
