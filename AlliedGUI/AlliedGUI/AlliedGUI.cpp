// AlliedGUI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AlliedGUI.h"

//---------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include "resource.h"

//---------------------------------------------------------------------------
char AppCaption[40];
HINSTANCE hInst;
const int NUMBUTTONS = 7;
LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg,
			      WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------




INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	MSG        Msg;
	HWND       hWnd;
	WNDCLASSEX WndClsEx;
    LPCTSTR ClsName = L"ResFund";

	LoadString(hInstance, IDS_APP_NAME, AppCaption, 40);

	hInst = hInstance;

	// Create the application window
	WndClsEx.cbSize        = sizeof(WNDCLASSEX);
	WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc   = WndProcedure;
	WndClsEx.cbClsExtra    = 0;
	WndClsEx.cbWndExtra    = 0;
	WndClsEx.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RESFUND2));
	WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClsEx.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINFRAME);
	WndClsEx.lpszClassName = ClsName;
	WndClsEx.hInstance     = hInst;
	WndClsEx.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RESFUND2));

	// Register the application
	RegisterClassEx(&WndClsEx);

	// Create the window object
	hWnd = CreateWindowEx(0,
                          ClsName,
                          AppCaption,
               		  WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
               		  CW_USEDEFAULT,
                          CW_USEDEFAULT,
               		  CW_USEDEFAULT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
	
	// Find out if the window was created
	if( !hWnd ) // If the window was not created,
		return FALSE; // stop the application

	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_BAR_CLASSES;
	InitCommonControlsEx(&InitCtrlEx);

	TBBUTTON tbrButtons[7];
	
	tbrButtons[0].iBitmap   = 0;
	tbrButtons[0].idCommand = IDM_FILE_NEW;
	tbrButtons[0].fsState   = TBSTATE_ENABLED;
	tbrButtons[0].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[0].dwData    = 0L;
	tbrButtons[0].iBitmap   = 0;
	tbrButtons[0].iString   = 0;
	
	tbrButtons[1].iBitmap   = 1;
	tbrButtons[1].idCommand = IDM_FILE_OPEN;
	tbrButtons[1].fsState   = TBSTATE_ENABLED;
	tbrButtons[1].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[1].dwData    = 0L;
	tbrButtons[1].iString   = 0;

	tbrButtons[2].iBitmap   = 0;
	tbrButtons[2].idCommand = 0;
	tbrButtons[2].fsState   = TBSTATE_ENABLED;
	tbrButtons[2].fsStyle   = TBSTYLE_SEP;
	tbrButtons[2].dwData    = 0L;
	tbrButtons[2].iString   = 0;

	tbrButtons[3].iBitmap   = 2;
	tbrButtons[3].idCommand = IDM_ARROW;
	tbrButtons[3].fsState   = TBSTATE_ENABLED;
	tbrButtons[3].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[3].dwData    = 0L;
	tbrButtons[3].iString   = 0;

	tbrButtons[4].iBitmap   = 3;
	tbrButtons[4].idCommand = IDM_DRAW_LINE;
	tbrButtons[4].fsState   = TBSTATE_ENABLED;
	tbrButtons[4].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[4].dwData    = 0L;
	tbrButtons[4].iString   = 0;

	tbrButtons[5].iBitmap   = 4;
	tbrButtons[5].idCommand = IDM_DRAW_RECTANGLE;
	tbrButtons[5].fsState   = TBSTATE_ENABLED;
	tbrButtons[5].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[5].dwData    = 0L;
	tbrButtons[5].iString   = 0;
	
	tbrButtons[6].iBitmap   = 5;
	tbrButtons[6].idCommand = IDM_DRAW_ELLIPSE;
	tbrButtons[6].fsState   = TBSTATE_ENABLED;
	tbrButtons[6].fsStyle   = TBSTYLE_BUTTON;
	tbrButtons[6].dwData    = 0L;
	tbrButtons[6].iString   = 0;

	HWND hWndToolbar;
	hWndToolbar = CreateToolbarEx(hWnd,
		                      WS_VISIBLE | WS_CHILD | WS_BORDER,
	                              IDB_STANDARD,
	                              NUMBUTTONS,
				      hInst,
			              IDB_STANDARD,
				      tbrButtons,
				      NUMBUTTONS,
				      16, 16, 16, 16,
				      sizeof(TBBUTTON));

	// Display the window to the user
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Decode and treat the messages
	// as long as the application is running
	while( GetMessage(&Msg, NULL, 0, 0) )
	{
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
	}

    return 0;
}



//---------------------------------------------------------------------------
LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,
			   WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    default:
        // Process the left-over messages
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    // If something was not done, let it go
    return 0;
}




