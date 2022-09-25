#include "Cell.h"

Cell::Cell()
{
	m_mineNum = 0;
	m_cellState = CellState::Closed;
}

void Cell::IncreaseMineNum(int amount)
{
	m_mineNum += amount;
}

int Cell::GetMineNum() const
{
	return m_mineNum;
}

void Cell::SetMineNum(int n)
{
	m_mineNum = n;
}

CellState Cell::GetCellState() const
{
	return m_cellState;
}

void Cell::SetCellState(CellState state)
{
	m_cellState = state;
}

