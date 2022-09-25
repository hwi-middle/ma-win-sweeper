#include <queue>
#include <string>
#include <vector>
#include <Windows.h>
#include "Board.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("ma-win-sweeper");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 900,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	constexpr int MINE = -1;
	constexpr int MINE_NUM = 40;
	constexpr int BOARD_SIZE = 16;

	HBRUSH defaultBrush;
	HBRUSH openedBrush;
	HBRUSH markBrush;
	HBRUSH mineBrush;

	HDC hdc;
	PAINTSTRUCT ps;

	static bool bIsGameOver = false;
	static Board board(BOARD_SIZE, MINE_NUM);
	static RECT rects[BOARD_SIZE][BOARD_SIZE];
	static std::pair<int, int> leftClickedRect = { -1,-1 };
	static std::pair<int, int> rightClickedRect = { -1,-1 };
	static int remainMine = MINE_NUM;

	switch (iMessage)
	{
	case WM_CREATE:
	{
		constexpr int OFFSET_X = 15;
		constexpr int OFFSET_Y = 80;
		constexpr int CELL_SIZE = 40;
		int left = OFFSET_X;
		int top = OFFSET_Y;
		int right = CELL_SIZE + OFFSET_X;
		int bottom = CELL_SIZE + OFFSET_Y;
		for (int r = 0; r < BOARD_SIZE; r++)
		{
			for (int c = 0; c < BOARD_SIZE; c++)
			{
				rects[r][c] = { left, top, right, bottom };

				left += CELL_SIZE;
				right += CELL_SIZE;
			}

			left = OFFSET_X;
			right = CELL_SIZE + OFFSET_X;
			top += CELL_SIZE;
			bottom += CELL_SIZE;
		}

		return 0;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		openedBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		markBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(255, 0, 0));
		mineBrush = CreateSolidBrush(RGB(255, 0, 0));
		constexpr std::pair<int, int> NOT_CLICKED = { -1,-1 };

		const int leftClickedR = leftClickedRect.first;
		const int leftClickedC = leftClickedRect.second;
		if (leftClickedRect != NOT_CLICKED && board.GetCellState(leftClickedR, leftClickedC) == CellState::Closed && !bIsGameOver)
		{
			constexpr int DIRECTION_NUM = 8;
			std::queue<std::pair<int, int>> bfsQueue;

			board.SetCellState(leftClickedR, leftClickedC, CellState::Opened);
			if (board.GetCellMineNum(leftClickedR, leftClickedC) == 0)
			{
				bfsQueue.push({ leftClickedR , leftClickedC });
				constexpr int deltaR[DIRECTION_NUM] = { 1,0,-1,0,1,-1,-1,1 };
				constexpr int deltaC[DIRECTION_NUM] = { 0,1,0,-1,-1,1,-1,1 };

				while (!bfsQueue.empty())
				{
					std::pair<int, int> cur = bfsQueue.front();
					bfsQueue.pop();

					const int curR = cur.first;
					const int curC = cur.second;

					if (board.GetCellMineNum(curR, curC) != 0)
					{
						board.SetCellState(curR, curC, CellState::Opened);
						continue;
					}

					for (int dir = 0; dir < DIRECTION_NUM; dir++)
					{
						const int newR = curR + deltaR[dir];
						const int newC = curC + deltaC[dir];
						if (newR < 0 || newR >= BOARD_SIZE || newC < 0 || newC >= BOARD_SIZE) continue;
						if (board.GetCellState(newR, newC) != CellState::Closed) continue;

						board.SetCellState(newR, newC, CellState::Opened);
						bfsQueue.push({ newR,newC });
					}
				}
			}
			else if (board.GetCellMineNum(leftClickedR, leftClickedC) == MINE)
			{
				bIsGameOver = true;
			}
		}

		const int rightClickedR = rightClickedRect.first;
		const int rightClickedC = rightClickedRect.second;
		if (rightClickedRect != NOT_CLICKED && !bIsGameOver)
		{
			if (board.GetCellState(rightClickedR, rightClickedC) == CellState::Closed)
			{
				board.SetCellState(rightClickedR, rightClickedC, CellState::MarkedAsMine);
				remainMine--;
			}
			else if (board.GetCellState(rightClickedR, rightClickedC) == CellState::MarkedAsMine)
			{
				board.SetCellState(rightClickedR, rightClickedC, CellState::Closed);
				remainMine++;
			}
		}

		if (bIsGameOver)
		{
			for (int r = 0; r < BOARD_SIZE; r++)
			{
				for (int c = 0; c < BOARD_SIZE; c++)
				{
					if (board.GetCellMineNum(r, c) == MINE)
					{
						defaultBrush = (HBRUSH)SelectObject(hdc, mineBrush);
						Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
						SelectObject(hdc, defaultBrush);
					}
					else
					{
						if (board.GetCellState(r, c) == CellState::MarkedAsMine)
						{
							defaultBrush = (HBRUSH)SelectObject(hdc, markBrush);
							Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
							SelectObject(hdc, defaultBrush);
						}
						else
						{
							defaultBrush = (HBRUSH)SelectObject(hdc, openedBrush);
							Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
							int cellMineNum = board.GetCellMineNum(r, c);
							std::wstring cellMineNumText = std::to_wstring(cellMineNum);
							DrawText(hdc, cellMineNumText.c_str(), cellMineNumText.length(), &rects[r][c], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
							SelectObject(hdc, defaultBrush);
						}
					}
				}
			}
		}
		else
		{
			for (int r = 0; r < BOARD_SIZE; r++)
			{
				for (int c = 0; c < BOARD_SIZE; c++)
				{
					if (board.GetCellState(r, c) == CellState::Opened)
					{
						defaultBrush = (HBRUSH)SelectObject(hdc, openedBrush);
						Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
						int cellMineNum = board.GetCellMineNum(r, c);
						std::wstring cellMineNumText = std::to_wstring(cellMineNum);
						DrawText(hdc, cellMineNumText.c_str(), cellMineNumText.length(), &rects[r][c], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						SelectObject(hdc, defaultBrush);
					}
					else if (board.GetCellState(r, c) == CellState::MarkedAsMine)
					{
						defaultBrush = (HBRUSH)SelectObject(hdc, markBrush);
						Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
						DrawText(hdc, TEXT("¡Ü"), lstrlen(TEXT("¡Ü")), &rects[r][c], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						SelectObject(hdc, defaultBrush);
					}
					else
					{
						Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
					}
				}
			}
		}



		LPCTSTR description = TEXT("Press 'R' to Restart");

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		RECT descriptionRect = { 0, 20, clientRect.right, 50 };
		DrawText(hdc, description, lstrlen(description), &descriptionRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		RECT mineNumRect = { 400, 20, clientRect.right, 50 };
		std::wstring remainMineText = std::to_wstring(remainMine);
		DrawText(hdc, remainMineText.c_str(), remainMineText.length(), &mineNumRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		leftClickedRect = NOT_CLICKED;
		rightClickedRect = NOT_CLICKED;
		DeleteObject(markBrush);
		DeleteObject(mineBrush);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		for (int r = 0; r < BOARD_SIZE; r++)
		{
			for (int c = 0; c < BOARD_SIZE; c++)
			{
				RECT rect = rects[r][c];

				if (rect.left < mouseX && rect.right > mouseX && rect.top < mouseY && rect.bottom > mouseY)
				{
					leftClickedRect = { r, c };
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;
				}
			}
		}

		leftClickedRect = { -1,-1 };
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		for (int r = 0; r < BOARD_SIZE; r++)
		{
			for (int c = 0; c < BOARD_SIZE; c++)
			{
				RECT rect = rects[r][c];

				if (rect.left < mouseX && rect.right > mouseX && rect.top < mouseY && rect.bottom > mouseY)
				{
					rightClickedRect = { r, c };
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;
				}
			}
		}

		rightClickedRect = { -1,-1 };
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	case WM_CHAR:
	{
		switch (wParam)
		{
		case 'r':
			board.InitMap();
			bIsGameOver = false;
			remainMine = MINE_NUM;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
