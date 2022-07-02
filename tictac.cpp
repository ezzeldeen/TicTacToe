// tictac.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "tictac.h"
#include <windowsx.h>
#include <cstdio>
#include <iostream>

#define MAX_LOADSTRING 100
HBRUSH blueBrush, redBrush;
int playerTurn = 1;
int gameBoard[9] = {0};
int winner = 0;
int wins[3];

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
const int CELL_SIZE = 100;
HICON oIcon, xIcon;

// function defenetion
bool GetGameBoardRect (HWND hwnd, RECT* pRect)
{
    RECT rc;

    if (GetClientRect (hwnd, &rc))
    {
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        pRect->left = (width - CELL_SIZE * 3) / 2;
        pRect->top = (height - CELL_SIZE * 3) / 2;
        pRect->right = pRect->left + CELL_SIZE * 3;
        pRect->bottom = pRect->top + CELL_SIZE * 3;

        return TRUE;
    }
    SetRectEmpty (pRect);
    return FALSE;
}

void DrawLine (HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx (hdc, x1, y1, NULL);
LineTo (hdc, x2, y2);
}

int GetCellNumberFromPoint (HWND hwnd, int x, int y)
{
    POINT pt;
    RECT rc;

    pt.x = x;
    pt.y = y;

    if (GetGameBoardRect (hwnd, &rc))
    {
        if (PtInRect (&rc, pt))
        {
            // user clicked inside game board
            // Normalize x,y (0 to 3*cell_size)

            x = pt.x - rc.left;
            y = pt.y - rc.top;

            int column = x / CELL_SIZE;
            int row = y / CELL_SIZE;

            // convert to index (1-9) square or 0-8
            return column + row * 3;
        }
    }
    return -1; // outside game board or failure
}

bool GetCellRect (HWND hWnd, int index, RECT* pRect)
{
    RECT rcBoard;
    SetRectEmpty (pRect);

    if (index < 0 || index >8)
    {
        return FALSE;
    }

    if (GetGameBoardRect (hWnd, &rcBoard))
    {

        int y = index / 3; // Row number
        int x = index % 3; // column number

        pRect->left = rcBoard.left + x * CELL_SIZE + 1;
        pRect->top = rcBoard.top + y * CELL_SIZE + 1;
        pRect->right = pRect->left + CELL_SIZE - 1;
        pRect->bottom = pRect->top + CELL_SIZE - 1;
        return TRUE;
    }

    return FALSE;
}

/*
returns :
0 no winner
1 player 1
2 player 2
3 draw

0,1,2
3,4,5
6,7,8

*/
int GetWinner (int wins[3])
{
    int cells[] = { 0,1,2,  3,4,5,  6,7,8,  0,3,6,  1,4,7,  2,5,8,  0,4,8,  2,4,6 };
    // check winner
    for (int i = 0; i < 24; i += 3)
    {
        if (gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]] && (gameBoard[cells[i]] != 0))
        {
            // winner
            wins[0] = cells[i];
            wins[1] = cells[i + 1];
            wins[2] = cells[i + 2];

            return gameBoard[cells[i]];
        }
    }

    // next see if we have any cells left empty
    for (int i = 0; i < 9; ++i)
    {
        if (gameBoard[i] == 0)
            return 0; // continue to play ...
    }
    return 3;// draw.

}

void ShowTurn (HWND hwnd, HDC hdc)
{
    const WCHAR* pTurntext = NULL;// = (playerTurn == 1) ? L"Turn: Player 1" : L"Turn: Player 2";
    switch (winner)
    {
    case 0: // continue play
        pTurntext = (playerTurn == 1) ? L"Turn: Player 1" : L"Turn: Player 2";
        break;

    case 1: // player 1 wins
        pTurntext = L"Player 1 is winner";
        break;

    case 2: // player 2 wins
        pTurntext = L"Player 2 is winner";
        break;

    case 3: // draw
        pTurntext = L"No one wins";
        break;

    }

    RECT rc;
    if (GetClientRect (hwnd, &rc) && (pTurntext != NULL))
    {
        rc.top = rc.bottom - 48;
        //FillRect (hdc, &rc, (HBRUSH)GetStockObject (GRAY_BRUSH));
        SetBkColor (hdc, TRANSPARENT);
        SetTextColor (hdc, RGB (255, 255, 255));
        DrawText (hdc, pTurntext, lstrlen(pTurntext) ,&rc,DT_CENTER);
    }

}

void DrawIconCentered (HDC hdc, RECT * pRect,HICON hIcon )
{
    if (NULL != pRect)
    {
        // 32 is ~ icon width or
        const int ICON_WIDTH = GetSystemMetrics (SM_CXICON);
        const int ICON_HEIGHT = GetSystemMetrics (SM_CYICON);

        int left = pRect->left + ((pRect->right - pRect->left) - ICON_WIDTH) / 2;
        int top  = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGHT) / 2;
        DrawIcon (hdc, left, top, hIcon);
    }
}

void ShowWinner (HWND hwnd, HDC hdc)
{
    RECT rcWin;
    for (int i = 0; i < 3; i++)
    {
        if (GetCellRect (hwnd, wins[i], &rcWin))
        {
            FillRect (hdc, &rcWin, (winner == 1) ? redBrush : blueBrush);
            DrawIconCentered (hdc, &rcWin, (winner == 1) ? xIcon : oIcon);
        }
    }
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
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
    LoadStringW(hInstance, IDC_TICTAC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTAC));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTAC));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTAC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  PURPOSE: Processes messages for the main window.
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

        case WM_CREATE:
        {
            redBrush = CreateSolidBrush (RGB(255,0,0));
            blueBrush = CreateSolidBrush (RGB (0, 0, 255));

            // load player icons
            oIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_O));
            xIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_X));

        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_FILE_NEWGAME:
            {
                int ret = MessageBox (hWnd, L"ARE you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
                if (IDYES == ret)
                {
                    playerTurn = 1;
                    // reset and start a new game
                    ZeroMemory (gameBoard, sizeof (gameBoard));
                    winner = 0;

                    // force a paint message 
                    InvalidateRect (hWnd, NULL, TRUE); // post WM_PAINT to our windowProc it getts queued in our msg queue
                    UpdateWindow (hWnd); // force immediate handling 
                }
            }
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
    
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM (lParam);
        int yPos = GET_Y_LPARAM (lParam);

        // only handle clicks if its a player urn
        if (playerTurn == 0)
                    break;
        

        int index = GetCellNumberFromPoint (hWnd, xPos, yPos);
        HDC hdc = GetDC (hWnd);
        if (NULL != hdc)
        {
            WCHAR temp[100];
            
            //wsprintf(temp, L"Index = %d",index);
            //TextOut (hdc, xPos, yPos, temp, lstrlen(temp));

            // Get cell dimensio from its index
            if (index != -1)
            {
                RECT rcCell;
                if (gameBoard[index] == 0 && GetCellRect (hWnd, index, &rcCell))
                {
                    gameBoard[index] = playerTurn;
                    //FillRect (hdc, &rcCell, (playerTurn == 1) ? redBrush : blueBrush);
                    DrawIconCentered (hdc, &rcCell, ((playerTurn == 1) ? xIcon : oIcon));

                    // check for a winner
                    
                    winner = GetWinner (wins);
                    
                    if (winner == 1 || winner == 2)
                    {
                        ShowWinner (hWnd, hdc);

                        // we have winner 
                        MessageBox (hWnd,
                            (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
                            L"You Win!",
                            MB_OK | MB_ICONINFORMATION);

                        playerTurn = 0;
                    }
                    else if (winner == 3)
                    {
                        // It's a draw
                        MessageBox (hWnd,
                            L"No one wins",
                            L"It's a draw",                    
                            MB_OK | MB_ICONEXCLAMATION);
                    }
                    else if (winner == 0)
                    {
                        playerTurn = (playerTurn == 1) ? 2 : 1;
                    }
                    
                    //Display turn 
                    ShowTurn (hWnd, hdc);
                }
            }

            ReleaseDC (hWnd, hdc);
        }
    }
    break;

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
        pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
        pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
        pMinMax->ptMaxTrackSize.x = CELL_SIZE * 6;
        pMinMax->ptMaxTrackSize.y = CELL_SIZE * 6;
    }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

            RECT rc;
            if (GetGameBoardRect (hWnd, &rc))
            {
                RECT rcClient;
                if (GetClientRect (hWnd, &rcClient))
                {
                    //SetBkColor (hdc, RGB (128, 128, 128));
                    SetBkMode (hdc, TRANSPARENT);
                    SetTextColor (hdc, RGB (255, 255, 0));
                    // Draw player 1 and player 2 text
                    TextOut (hdc, 16, 16, L"Player 1", 8);

                    DrawIcon (hdc, 24,40 ,xIcon );

                    SetTextColor (hdc, RGB (0,0,255,));
                    TextOut (hdc, rcClient.right -64, 16, L"Player 2", 8);
                    DrawIcon (hdc, rcClient.right - 50, 40, oIcon);
                    //Display turn 
                    ShowTurn (hWnd, hdc);
                }


                //FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
            }
            // Draw vertical Lines
            DrawLine (hdc, rc.left + CELL_SIZE, rc.top, rc.left+ CELL_SIZE, rc.bottom);
            DrawLine (hdc, rc.left + CELL_SIZE*2, rc.top, rc.left + CELL_SIZE*2, rc.bottom);

            // Draw Horizontal Lines
            DrawLine (hdc, rc.left , rc.top + CELL_SIZE, rc.right , rc.top + CELL_SIZE);
            DrawLine (hdc, rc.left, rc.top + CELL_SIZE*2, rc.right, rc.top + CELL_SIZE*2);

            // Draw all occiupied cells
            RECT rcCell;
            for (int i = 0; i < 9; ++i)
            {
                if ((gameBoard[i] !=0) && GetCellRect (hWnd, i, &rcCell))
                {
                    //FillRect (hdc, &rcCell, (gameBoard[i]==1)? redBrush : blueBrush);
                    DrawIconCentered (hdc, &rcCell, ((gameBoard[i] == 1) ? xIcon : oIcon));

                }
            }
            if (winner ==1 || winner ==2)
            {
                // show winner
                ShowWinner (hWnd, hdc);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DeleteObject (redBrush);
        DeleteObject (blueBrush);
        PostQuitMessage(0);
        DestroyIcon (oIcon);
        DestroyIcon (xIcon);

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
