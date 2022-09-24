#include "Cell.h"

Cell::Cell()
{

	m_mineNum = 0;
	m_isOpened = false;

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

