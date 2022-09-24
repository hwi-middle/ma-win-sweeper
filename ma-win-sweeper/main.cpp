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
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 900,
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
	constexpr int MINE_NUM = 10;
	constexpr int BOARD_SIZE = 9;

	static HBRUSH defaultBrush;
	static HBRUSH mineBrush;

	HDC hdc;
	PAINTSTRUCT ps;

	static Board board(BOARD_SIZE, MINE_NUM);
	static RECT rects[BOARD_SIZE][BOARD_SIZE];
	static std::pair<int, int> clickedRect = { -1,-1 };

	switch (iMessage)
	{
	case WM_CREATE:
	{
		constexpr int OFFSET_X = 15;
		constexpr int OFFSET_Y = 80;
		constexpr int CELL_SIZE = 50;
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

		mineBrush = CreateSolidBrush(RGB(255, 0, 0));
		for (int r = 0; r < BOARD_SIZE; r++)
		{
			for (int c = 0; c < BOARD_SIZE; c++)
			{
				int cellMineNum = board.GetCellMineNum(r, c);
				if (cellMineNum == -1)
				{
					defaultBrush = (HBRUSH)SelectObject(hdc, mineBrush);
					Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
				}
				else
				{
					Rectangle(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
					std::wstring cellMineNumText = std::to_wstring(cellMineNum);
					DrawText(hdc, cellMineNumText.c_str(), cellMineNumText.length(), &rects[r][c], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}

				if (clickedRect == std::make_pair(r, c))
				{
					Ellipse(hdc, rects[r][c].left, rects[r][c].top, rects[r][c].right, rects[r][c].bottom);
				}

				SelectObject(hdc, defaultBrush);
			}
		}

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
					clickedRect = { r, c };
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;
				}
			}

		}

		clickedRect = { -1,-1 };
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
