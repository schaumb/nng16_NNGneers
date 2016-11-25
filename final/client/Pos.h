#pragma once
#include <cmath>

struct POS
{
	enum eDirection
	{
		SHIFT_UP = 0,
		SHIFT_RIGHT = 1,
		SHIFT_DOWN = 2,
		SHIFT_LEFT = 3
	};
	int x, y;
	POS() { x = y = 0; }
	POS(int _x, int _y) { x = _x; y = _y; }
	bool operator== (POS const &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!= (POS const &rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
	bool operator< (POS const &rhs) const
	{
		if (y != rhs.y)
			return y<rhs.y;
		return x<rhs.x;
	}
	bool IsNear(POS const &rhs) const
	{
		return abs(x - rhs.x) + abs(y - rhs.y) <= 1;
	}
	bool IsValid() const
	{
		return x != 0;
	}
	POS ShiftXY(int dx, int dy) { return POS(x + dx, y + dy); }
	POS ShiftDir(int dir)
	{
		switch (dir)
		{
		case SHIFT_UP: return POS(x, y - 1);
		case SHIFT_RIGHT: return POS(x + 1, y);
		case SHIFT_DOWN: return POS(x, y + 1);
		default: return POS(x - 1, y);
		}
	}
	
	friend std::ostream& operator<<(std::ostream& out, const POS& o) {
	    return out << o.x << " " << o.y;
	}
};
