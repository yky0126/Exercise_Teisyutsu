#include <stdio.h>
#include <windows.h>
#include <d3dx9.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
	LPSTR cmdLine, INT nCmdShow)
{
	LPCSTR pClassName = "Sample Window Class";

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = pClassName;

	if (!RegisterClassEx(&wc)) return FALSE;

	HWND hWindow = CreateWindow(pClassName, "Exercise",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);

	if (!hWindow) return FALSE;
	ShowWindow(hWindow, nCmdShow);

	MSG msg;
	//Main Loop Start
	ZeroMemory(&msg, sizeof(msg));
	timeBeginPeriod(1);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		Sleep(1);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
	timeEndPeriod(1);
	//Main Loop End
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWindow, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWindow, msg, wp, lp);
	}
	return 0;
}