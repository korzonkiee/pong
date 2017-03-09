// Pong.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Pong.h"

#define MAX_LOADSTRING 100

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 300

#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 20

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szPaddleWindowClass[MAX_LOADSTRING];


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM				PaddleRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void				CalculatorPaddleInitialPosition(HWND hWnd, int *x, int *y);
void				AddWindowTransparency(HWND hWnd, int alpha);
void				CenterWindow(HWND hWnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PONG, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PADDLE, szPaddleWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	PaddleRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PONG));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONG));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 0));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PONG);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM PaddleRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONG));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_ACTIVEBORDER + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PADDLE);
	wcex.lpszClassName = szPaddleWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_EX_LAYERED | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	AddWindowTransparency(hWnd, 80);
	CenterWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	int paddleX, paddleY;
	CalculatorPaddleInitialPosition(hWnd, &paddleX, &paddleY);

	HWND paddlehWnd = CreateWindowExW(0, szPaddleWindowClass, szTitle, WS_CHILD | WS_VISIBLE | WS_OVERLAPPED,
		paddleX, paddleY, PADDLE_WIDTH, PADDLE_HEIGHT, hWnd, nullptr, hInstance, nullptr);

	if (!paddlehWnd)
	{
		return FALSE;
	}

	AddWindowTransparency(paddlehWnd, 100);

	ShowWindow(paddlehWnd, nCmdShow);
	UpdateWindow(paddlehWnd);


	return TRUE;
}

void CalculatorPaddleInitialPosition(HWND hWnd, int *x, int *y)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	*x = (rc.right - rc.left) / 2 - PADDLE_WIDTH / 2;
	*y = rc.bottom - rc.top - PADDLE_HEIGHT;
}

void AddWindowTransparency(HWND hWnd, int alpha)
{
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) |
		WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, (255 * alpha) / 100, LWA_ALPHA);
}

void CenterWindow(HWND hWnd)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT rect;
	GetWindowRect(hWnd, &rect);
	rect.left = (screenWidth / 2) - (rect.left / 2);
	rect.top = (screenHeight / 2) - (rect.top / 2);

	SetWindowPos(hWnd, 0,
		(screenWidth - rect.right) / 2,
		(screenHeight - rect.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// SWP_NOSIZE   Retains current size (ignores the cx and cy parameters).
	// SWP_NOZORDER   Retains current ordering (ignores pWndInsertAfter).
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
