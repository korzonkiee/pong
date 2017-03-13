#include "stdafx.h"
#include "Pong.h"

#define MAX_LOADSTRING 100

#define MAX_FILELENGTH 256

#define MAX_SCORE_LENGTH 5

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 300

#define PADDLE_WIDTH 64
#define PADDLE_HEIGHT 16

#define BALL_RADIUS 16
#define BALL_STEP_LENGTH 5

#define SPEED 50

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WCHAR szPaddleWindowClass[MAX_LOADSTRING];
WCHAR szBallWindowClass[MAX_LOADSTRING];

HWND windowhWnd;
HWND paddlehWnd;


ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM				PaddleRegisterClass(HINSTANCE hInstance);
ATOM				BallRegisterClass(HINSTANCE hInstance);
VOID CALLBACK		BallTimerProc(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime);
BOOL                InitInstance(HINSTANCE, int);
void				CalculatePaddleInitialPosition(HWND hWnd, int *x, int *y);
void				CalculateBallInitialPosition(HWND hWnd, int *x, int *y);
void				SetWindowAlwaysOnTop(HWND hWnd);
void				AddWindowTransparency(HWND hWnd, int alpha);
void				CenterWindow(HWND hWnd);
void				MovePaddle();
void				DetectCollisionWithPaddle();
int					GetNumberOfDigits(int x);
void				ConvertIntToWChar(wchar_t *buffer, int x);
void				CreateNewGame();
void				PickAndChangeBackgroundColor();
void				ChooseFile(HWND hWnd, wchar_t **filePath);
void				DrawBitmap(HWND hWnd, HDC hdc, wchar_t *filePath);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	PaddleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK    BallWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool GAMEOVER = false;

short int BallDirX = 1;
short int BallDirY = 1;

int PaddleX;
int PaddleY;

int BallX = 100;
int BallY = 100;

int CurrentScore = 0;

wchar_t *filePath = NULL;

enum BackgroundMode
{
	TitledMode = 0,
	StretchedMode = 1
};

BackgroundMode CurrentBackgroundMode = TitledMode;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PONG, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PADDLE, szPaddleWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BALL, szBallWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	PaddleRegisterClass(hInstance);
	BallRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PONG));

	MSG msg;

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
	wcex.lpfnWndProc = PaddleWndProc;
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

ATOM BallRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = BallWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONG));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BALL);
	wcex.lpszClassName = szBallWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;


	// Create main window

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_EX_LAYERED | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}
	windowhWnd = hWnd;

	SetWindowAlwaysOnTop(hWnd);
	AddWindowTransparency(hWnd, 80);
	CenterWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	// Create paddle window

	CalculatePaddleInitialPosition(hWnd, &PaddleX, &PaddleY);

	paddlehWnd = CreateWindowExW(0, szPaddleWindowClass, szTitle, WS_CHILD | WS_VISIBLE | WS_OVERLAPPED,
		PaddleX, PaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, hWnd, nullptr, hInstance, nullptr);

	if (!paddlehWnd)
	{
		return FALSE;
	}

	AddWindowTransparency(paddlehWnd, 100);
	ShowWindow(paddlehWnd, nCmdShow);
	UpdateWindow(paddlehWnd);

	// Create ball window

	int ballX, ballY;
	CalculateBallInitialPosition(hWnd, &ballX, &ballY);

	HWND ballhWnd = CreateWindowExW(0, szBallWindowClass, szTitle, WS_CHILD | WS_VISIBLE | WS_OVERLAPPED,
		ballX, ballY, BALL_RADIUS, BALL_RADIUS, hWnd, nullptr, hInstance, nullptr);

	if (!ballhWnd)
	{
		return FALSE;
	}

	HRGN region = CreateEllipticRgn(0, 0, BALL_RADIUS, BALL_RADIUS);
	SetWindowRgn(ballhWnd, region, true);

	AddWindowTransparency(ballhWnd, 100);
	ShowWindow(ballhWnd, nCmdShow);
	UpdateWindow(ballhWnd);

	return TRUE;
}

void CalculatePaddleInitialPosition(HWND hWnd, int *x, int *y)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	*x = (rc.right - rc.left) / 2 - PADDLE_WIDTH / 2;
	*y = rc.bottom - rc.top - PADDLE_HEIGHT;
}

void CalculateBallInitialPosition(HWND hWnd, int *x, int *y)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	*x = (rc.right - rc.left) / 2 - BALL_RADIUS / 2;
	*y = (rc.bottom - rc.top) / 2 - BALL_RADIUS / 2;
}

void SetWindowAlwaysOnTop(HWND hWnd)
{
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT r;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_NEWGAME:
			CreateNewGame();
			break;
		case IDM_BGCOLOR:
			PickAndChangeBackgroundColor();
			break;
		case IDM_BGBMP:
			ChooseFile(hWnd, &filePath);
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, true);
			break;
		case IDM_BGSTRETCH:
			CurrentBackgroundMode = StretchedMode;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, true);
			break;
		case IDM_BGTILE:
			CurrentBackgroundMode = TitledMode;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, true);
			break;
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
		if (filePath != nullptr)
		{
			DrawBitmap(hWnd, hdc, filePath);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEMOVE:
		if (!GAMEOVER)
			MovePaddle();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK PaddleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_MOUSEMOVE:
		if (!GAMEOVER)
			MovePaddle();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT r;
		GetClientRect(hWnd, &r);
		SetBkMode(hdc, TRANSPARENT);

		wchar_t buffer[MAX_SCORE_LENGTH + 1];
		ConvertIntToWChar(buffer, CurrentScore);

		//In order to make DT_VCENTER flag work we have to ensure that text
		//is singlelined.
		DrawText(hdc, buffer, GetNumberOfDigits(CurrentScore),
			&r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

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

LRESULT CALLBACK BallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 0, SPEED, (TIMERPROC)BallTimerProc);
		break;
	case WM_TIMER:
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
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

VOID CALLBACK BallTimerProc(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime)
{
	if (GAMEOVER)
		return;


	RECT rect;
	GetClientRect(windowhWnd, &rect);

	DetectCollisionWithPaddle();

	if (BallX + BALL_RADIUS >= rect.right)
		BallDirX = -1;
	if (BallX <= rect.left)
		BallDirX = 1;
	if (BallY <= rect.top)
		BallDirY = 1;
	if (BallY + BALL_RADIUS >= rect.bottom)
		BallDirY = -1;

	BallX += BALL_STEP_LENGTH * BallDirX;
	BallY += BALL_STEP_LENGTH * BallDirY;

	MoveWindow(hWnd, BallX, BallY, BALL_RADIUS, BALL_RADIUS, TRUE);
}

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

void DetectCollisionWithPaddle()
{
	if (BallY + BALL_RADIUS >= PaddleY)
	{
		if (BallX + BALL_RADIUS / 2 < PaddleX || BallX + BALL_RADIUS / 2 > PaddleX + PADDLE_WIDTH)
		{
			GAMEOVER = true;
		}
		else
		{
			BallDirY *= -1;
			CurrentScore++;
			InvalidateRect(paddlehWnd, NULL, true);
		}
	}
}

void MovePaddle()
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	ScreenToClient(windowhWnd, &cursorPosition);

	RECT windowRect;
	GetClientRect(windowhWnd, &windowRect);

	if (cursorPosition.x - PADDLE_WIDTH / 2 < windowRect.left ||
		cursorPosition.x + PADDLE_WIDTH / 2 > windowRect.right)
		return;

	PaddleX = cursorPosition.x - PADDLE_WIDTH / 2;
	PaddleY = windowRect.bottom - PADDLE_HEIGHT;

	MoveWindow(
		paddlehWnd,
		PaddleX, PaddleY,
		PADDLE_WIDTH, PADDLE_HEIGHT,
		true
	);
}

int GetNumberOfDigits(int x)
{
	int numberOfDigits = 1;
	while (x /= 10)
		numberOfDigits++;

	return numberOfDigits;
}

void ConvertIntToWChar(wchar_t *buffer, int x)
{
	wsprintfW(buffer, L"%d", CurrentScore);
}

void CreateNewGame()
{
	BallX = 100;
	BallY = 100;
	BallDirX = 1;
	BallDirY = 1;

	CurrentScore = 0;

	GAMEOVER = false;

	InvalidateRect(paddlehWnd, NULL, true);
}

void PickAndChangeBackgroundColor()
{
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];
	HBRUSH hbrush;
	static DWORD rgbCurrent;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = windowhWnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE) {
		RECT rc;
		GetClientRect(windowhWnd, &rc);
		HBRUSH newcolor;
		newcolor = CreateSolidBrush(cc.rgbResult);
		SetClassLongPtr(windowhWnd, GCLP_HBRBACKGROUND, (LONG)newcolor);
		InvalidateRect(windowhWnd, &rc, TRUE);
	}
}

void ChooseFile(HWND hWnd, wchar_t **filePath)
{
	OPENFILENAME ofn;
	wchar_t szFile[MAX_FILELENGTH];
	HANDLE hf;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"*.BMP";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		hf = CreateFile(ofn.lpstrFile,
			GENERIC_READ,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);

		*filePath = (wchar_t*)malloc(sizeof(wchar_t) * sizeof(szFile));
		wcscpy_s(*filePath, sizeof(szFile), szFile);

		CloseHandle(hf);
	}

}

void DrawBitmap(HWND hWnd, HDC hdc, wchar_t *filePath)
{
	HBITMAP hbmPicture;
	hbmPicture = (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	HDC hdcNew = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcNew, hbmPicture);

	BITMAP bmInfo;
	GetObject(hbmPicture, sizeof(bmInfo), &bmInfo);

	RECT r;
	GetClientRect(hWnd, &r);

	BitBlt(hdc, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hdcNew, 0, 0, SRCCOPY);

	if (CurrentBackgroundMode == TitledMode)
	{
		for (int i = 0; i < r.right; i += bmInfo.bmWidth)
		{
			for (int j = 0; j < r.bottom; j += bmInfo.bmHeight)
			{
				BitBlt(hdc, i, j, bmInfo.bmWidth, bmInfo.bmHeight, hdcNew, 0, 0, SRCCOPY);
			}
		}
	}
	else if (CurrentBackgroundMode == StretchedMode)
	{
		StretchBlt(hdc, 0, 0, r.right, r.bottom, hdcNew, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY);
	}
	

	DeleteObject(hbmPicture);
	SelectObject(hdcNew, hbmOld);
	DeleteDC(hdcNew);
}
