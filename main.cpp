#include <Windows.h>
#include "WindowsMessageMap.h"
#include <sstream>
#include <math.h>
#include <stdlib.h>

static int width = 800;
static int height = 450;
COLORREF colors[] = {
	RGB(0, 0, 0),       // 0 - Black (not used in cycling)
	RGB(0, 0, 255),     // 1 - Blue
	RGB(0, 255, 0),     // 2 - Green
	RGB(0, 255, 255),   // 3 - Cyan
	RGB(255, 0, 0),     // 4 - Red
	RGB(255, 0, 255),   // 5 - Magenta
	RGB(255, 255, 0),   // 6 - Yellow
	RGB(255, 255, 255), // 7 - White
	RGB(128, 128, 128), // 8 - Gray
	RGB(0, 128, 128),   // 9 - Dark Cyan
	RGB(128, 0, 0),     // 10 - Dark Red
	RGB(128, 0, 128),   // 11 - Dark Magenta
	RGB(128, 128, 0),   // 12 - Olive
	RGB(0, 128, 0),     // 13 - Dark Green
	RGB(0, 0, 128),     // 14 - Dark Blue
	RGB(255, 165, 0)    // 15 - Orange
};

void triangle(float x, float y,
	float h, int colorVal, HDC hdc)
{
	int colorIndex = (colorVal % 15) + 1;
	HPEN hPen = CreatePen(PS_SOLID, 1, colors[colorIndex]);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	for (float delta = 0; delta > -5; delta -= 1) {
		MoveToEx(hdc, x - (h + delta) / sqrt(3), y - (h + delta) / 3, NULL);
		LineTo(hdc, x + (h + delta) / sqrt(3), y - (h + delta) / 3);

		MoveToEx(hdc, x - (h + delta) / sqrt(3), y - (h + delta) / 3, NULL);
		LineTo(hdc, x, y + 2 * (h + delta) / 3);

		MoveToEx(hdc, x, y + 2 * (h + delta) / 3, NULL);
		LineTo(hdc, x + (h + delta) / sqrt(3), y - (h + delta) / 3);
	}
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

void trianglev2(float x, float y,
				float h, int colorVal, HDC hdc)
{
	int colorIndex = (colorVal % 15) + 1;
	HPEN hPen = CreatePen(PS_SOLID, 1, colors[colorIndex]);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	for (float delta = 0; delta > -1 + 5; delta -= 1) {
		MoveToEx(hdc, x - (h + delta) / sqrt(3),
			y + (h + delta) / 3, NULL);
		LineTo(hdc, x + (h + delta) / sqrt(3),
			 y + (h + delta) / 3);
		MoveToEx(hdc, x - (h + delta) / sqrt(3),
			y + (h + delta) / 3, NULL);
		LineTo(hdc, x,
			 y - 2 * (h + delta) / 3);
		MoveToEx(hdc, x,
			y - 2 * (h + delta) / 3, NULL);
		LineTo(hdc,x + (h + delta) / sqrt(3),
			 y + (h + delta) / 3);
	}
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

// A recursive function to draw out
// three adjacent smaller triangles
// while the height is greater than 5 pixels.
int drawTriangles(float x,float y,float h,int colorVal, HDC hdc)
{

	if (h < 5) {
		return 0;
	}

	if (x > 0 && y > 0 && x < width && y < height) {
		triangle(x, y, h, colorVal,hdc);
	}

	drawTriangles(x,
				  y - 2 * h / 3,
				  h / 2,
				  colorVal + 1,hdc);
	drawTriangles(x - h / sqrt(3),
				  y + h / 3,
				  h / 2,
				  colorVal + 1,hdc);
	drawTriangles(x + h / sqrt(3),
				  y + h / 3,
				  h / 2,
				  colorVal + 1,hdc);

	return 0;
}


const int MAX_TEXT_LENGTH = 256;
char textBuffer[MAX_TEXT_LENGTH] = { 0 };
void SetupConsole()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	static WindowsMessageMap mm;
	printf(mm(msg, lParam, wParam).c_str());
	//OutputDebugStringA(mm(msg,lParam,wParam).c_str());

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, L"WINDOWWERER");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, L"WAGABOOGA");
		}
		break;
	case WM_CHAR:
		{
			if (wParam >= 32 && wParam <= 126) // Printable ASCII characters
			{
				// Append the character to the text buffer
				size_t len = strlen(textBuffer);
				if (len < MAX_TEXT_LENGTH - 1) // Check for buffer overflow
				{
					textBuffer[len] = static_cast<char>(wParam);
					textBuffer[len + 1] = '\0'; // Null-terminate the string

					// Invalidate the window to trigger WM_PAINT
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}

		}
	break;
	case WM_LBUTTONDOWN:
		{
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "(" << pt.x << "," << pt.y << ")";
		SetWindowTextA(hWnd, oss.str().c_str());
		}
	break;
	case WM_SIZE:
	{
		width = LOWORD(lParam);  // New width of client area
		height = HIWORD(lParam); // New height of client area

		// Optionally store width and height for use in drawing
		// You can repaint or adjust drawing positions based on the new size
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		float x = width / 2;
		float y = 2 * height / 3;
		float h = height / 2;
		int colorVal = 0;
		// Draw text directly
		static std::string title;
		title.push_back((char)wParam);
		const char* text = "gasgasgag";
		TextOutA(hdc, 50, 80, textBuffer, strlen(textBuffer));
		trianglev2(width / 2, 2 * height / 3, height, 2, hdc);
		drawTriangles(x,y,h,colorVal, hdc);
		EndPaint(hWnd, &ps);

		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetupConsole();
	const auto pClassName = L"windowwindow";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindowEx(0, pClassName, L"windowwindow", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW, 200, 200, 800, 450, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, SW_SHOW);
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	if(gResult == -1)
	{
		return -1;

	}
	else
	{
		return msg.wParam;
	}
}

