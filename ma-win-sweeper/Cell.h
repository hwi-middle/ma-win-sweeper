#pragma once

class Cell
{
private:
	int m_mineNum;
	bool m_isOpened;
public:
	Cell();
	void IncreaseMineNum(int amount = 1);
	int GetMineNum() const;
	void SetMineNum(int n);
};
