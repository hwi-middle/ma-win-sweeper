#pragma once
enum class CellState
{
	Closed,
	Opened,
	MarkedAsMine,
};

class Cell
{
private:
	int m_mineNum;
	CellState m_cellState;
public:
	Cell();

	void IncreaseMineNum(int amount = 1);
	int GetMineNum() const;
	void SetMineNum(int n);
	CellState GetCellState() const;
	void SetCellState(CellState state);
};
