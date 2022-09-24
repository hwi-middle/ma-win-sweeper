#pragma once
#include <iostream>
#include "Cell.h"

class Board
{
private:
	Cell** m_map;
	int m_size;
	int m_mineAmount;
	static constexpr int MINE = -1;
public:
	Board(const int size, const int mineAmount);

	~Board();

	int GetCellMineNum(int r, int c);
};
