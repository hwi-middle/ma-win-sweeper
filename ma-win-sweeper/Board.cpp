#include "Board.h"

Board::Board(const int size, const int mineAmount)
{
	// ��� �ʱ�ȭ
	m_size = size;
	m_mineAmount = mineAmount;

	// �����Ҵ�
	m_map = new Cell * [m_size];
	for (int i = 0; i < m_size; i++)
	{
		m_map[i] = new Cell[m_size];
	}

	// ������ ��ġ�� ���� �ɱ�
	srand(time(nullptr));
	int r = 0;
	int c = 0;
	int mineCount = 0;

	// �� ��� ���� ���� ��ȭ��
	constexpr int deltaR[8] = { 1,0,-1,0,1,-1,-1,1 };
	constexpr int deltaC[8] = { 0,1,0,-1,-1,1,-1,1 };

	while (mineCount < m_mineAmount)
	{
		// ���� ���� �� ����
		r = rand() % m_size;
		c = rand() % m_size;

		// �̹� ���ڰ� ������ �ǳ� ��
		if (m_map[r][c].GetMineNum() == MINE)
		{
			continue;
		}

		// ���� �ɱ�
		m_map[r][c].SetMineNum(MINE);

		// �ֺ� Cell ������Ʈ
		for (int dir = 0; dir < 8; dir++)
		{
			const int newR = r + deltaR[dir];
			const int newC = c + deltaC[dir];
			if (newR < 0 || newR >= m_size || newC < 0 || newC >= m_size) continue;
			if (m_map[newR][newC].GetMineNum() == MINE) continue;
			m_map[newR][newC].IncreaseMineNum();
		}

		// ���� �� ����
		mineCount++;
	}

	// �ӽ÷� �����ڿ��� ������, ���� ������ ���� �и�
	for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
		{
			std::cout << m_map[i][j].GetMineNum() << " ";
		}
		std::cout << "\n";
	}
}

Board::~Board()
{
	for (int i = 0; i < m_size; i++)
	{
		delete[] m_map[i];
	}

	delete[] m_map;
}

int Board::GetCellMineNum(int r, int c)
{
	return m_map[r][c].GetMineNum();
}

CellState Board::GetCellState(int r, int c)
{
	return m_map[r][c].GetCellState();
}

void Board::SetCellState(int r, int c, CellState state)
{
	m_map[r][c].SetCellState(state);
}
