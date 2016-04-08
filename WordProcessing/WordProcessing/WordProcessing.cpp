// WordProcessing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
/*#include "WordProcessing.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WORDPROCESSING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WORDPROCESSING));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORDPROCESSING));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WORDPROCESSING);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
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
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
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
*/

LONG APIENTRY MainWndProc(
							HWND hwnd, /* window handle */
							UINT message, /* type of message */
							UINT wParam, /* additional information */
							LONG lParam) /* additional information */
{
	static HWND hwndEdit;
	CHAR lpszTrouble[] = "When in the Course of human Events "
						 "it becomes necessary for one People "
						 "to dissolve the Political Bands which "
						 "have connected them with another, and "
						 "to assume among the Powers of the "
						 "Earth, the separate and equal Station "
						 "to which the Laws of Nature and of "
						 "Nature's God entitle them, a decent "
						 "Respect to the Opinions of Mankind "
						 "requires that they should declare the "
						 "causes which impel them to the "
                         "Separation. ";
	switch (message) {
	case WM_CREATE:
					hwndEdit = CreateWindow("EDIT", /* predefined class */
											NULL, /* no window title */
											WS_CHILD | WS_VISIBLE | WS_VSCROLL |
											ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
											0, 0, 0, 0, /* set size in WM_SIZE message */
											hwnd, /* parent window */
											(HMENU) ID_EDITCHILD, /* edit control ID */
											(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
											NULL); /* pointer not needed */
					/* Add text to the window. */
					SendMessage(hwndEdit, WM_SETTEXT, 0,
					(LPARAM) lpszTrouble);
					return 0;
	case WM_COMMAND:
					switch (wParam)
					{
					case IDM_EDUNDO:
									/*
									* Send WM_UNDO only if there is something
									* to be undone.
									*/
									if (SendMessage(hwndEdit, EM_CANUNDO, 0, 0))
										SendMessage(hwndEdit, WM_UNDO, 0, 0);
									else
										MessageBox(hwndEdit,
													"Nothing to undo.",
													"Undo notification", MB_OK);
									break;
					case IDM_EDCUT:
									SendMessage(hwndEdit, WM_CUT, 0, 0);
									break;
					case IDM_EDCOPY:
									SendMessage(hwndEdit, WM_COPY, 0, 0);
									break;
					case IDM_EDPASTE:
									SendMessage(hwndEdit, WM_PASTE, 0, 0);
									break;
					case IDM_EDDEL:
									SendMessage(hwndEdit, WM_CLEAR, 0, 0);
									break;
					case IDM_PASSWORD:
									DialogBox(hinst, /* current instance */
												"PassBox", /* resource to use */
												hwnd, /* parent handle */
												(DLGPROC) PassProc);
									break;
					case IDM_WRAP:
									SendMessage(hwndEdit,
												EM_SETWORDBREAKPROC,
												(WPARAM) 0,
												(LPARAM) (EDITWORDBREAKPROC) WordBreakProc);
									SendMessage(hwndEdit,
												EM_FMTLINES,
												(WPARAM) TRUE,
												(LPARAM) 0);
									SendMessage(hwndEdit,
												EM_SETSEL,
												0, -1); /* select all text */
												SendMessage(hwndEdit, WM_CUT, 0, 0);
												SendMessage(hwndEdit, WM_PASTE, 0, 0);
												break;
					case IDM_ABOUT:
									DialogBox(hinst, /* current instance */
											 "AboutBox", /* resource to use */
					                         hwnd, /* parent handle */
					                         (DLGPROC) About);
					               break;
	               default:
			               return DefWindowProc(hwnd, message, wParam, lParam);
                   }
	break;
	case WM_SETFOCUS:
						SetFocus(hwndEdit);
						return 0;
	case WM_SIZE:
					/*
					* Make the edit control the size of the window's
					* client area.
					*/
					MoveWindow(hwndEdit,
								0, 0, /* starting x- and y-coordinates */
								LOWORD(lParam), /* width of client area */
								HIWORD(lParam), /* height of client area */
								TRUE); /* repaint window */
					return 0;
	case WM_DESTROY:
					PostQuitMessage(0);
					return 0;
	default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return NULL;
}
