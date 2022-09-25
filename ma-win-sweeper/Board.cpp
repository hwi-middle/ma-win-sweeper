#include "Board.h"

Board::Board(const int size, const int mineAmount)
{
	// 멤버 초기화
	m_size = size;
	m_mineAmount = mineAmount;

	// 동적할당
	m_map = new Cell * [m_size];
	for (int i = 0; i < m_size; i++)
	{
		m_map[i] = new Cell[m_size];
	}

	// 랜덤한 위치에 지뢰 심기
	srand(time(nullptr));
	int r = 0;
	int c = 0;
	int mineCount = 0;

	// 각 행과 열에 대한 변화값
	constexpr int deltaR[8] = { 1,0,-1,0,1,-1,-1,1 };
	constexpr int deltaC[8] = { 0,1,0,-1,-1,1,-1,1 };

	while (mineCount < m_mineAmount)
	{
		// 지뢰 심을 곳 선정
		r = rand() % m_size;
		c = rand() % m_size;

		// 이미 지뢰가 있으면 건너 뜀
		if (m_map[r][c].GetMineNum() == MINE)
		{
			continue;
		}

		// 지뢰 심기
		m_map[r][c].SetMineNum(MINE);

		// 주변 Cell 업데이트
		for (int dir = 0; dir < 8; dir++)
		{
			const int newR = r + deltaR[dir];
			const int newC = c + deltaC[dir];
			if (newR < 0 || newR >= m_size || newC < 0 || newC >= m_size) continue;
			if (m_map[newR][newC].GetMineNum() == MINE) continue;
			m_map[newR][newC].IncreaseMineNum();
		}

		// 지뢰 수 증가
		mineCount++;
	}

	// 임시로 생성자에서 보여줌, 추후 구현에 따라 분리
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
