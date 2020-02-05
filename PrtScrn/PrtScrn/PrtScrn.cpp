// PrtScrn.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "PrtScrn.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool ScreenShot(HWND hWnd);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください
    ScreenShot(NULL);

    return FALSE;
    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PRTSCRN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRTSCRN));

    MSG msg;

    // メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRTSCRN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PRTSCRN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
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
            // TODO: HDC を使用する描画コードをここに追加してください...
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

// バージョン情報ボックスのメッセージ ハンドラーです。
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

// スクリーンショットを取る
bool ScreenShot(HWND hWnd)
{
    // スクリーンサイズ
    int x = ::GetSystemMetrics(SM_CXSCREEN);
    int y = ::GetSystemMetrics(SM_CYSCREEN);
    int x1 = 0, y1 = 0;

    if (hWnd != NULL) {
        // Alt+PrintScreenだったら作業中のウィンドウのみを取る
        WINDOWINFO wi;
        ::ZeroMemory(&wi, sizeof(wi)); // ねんのため
        wi.cbSize = sizeof(WINDOWINFO);
        if (!::GetWindowInfo(hWnd, &wi)) return false;
        x = wi.rcWindow.right - wi.rcWindow.left;
        y = wi.rcWindow.bottom - wi.rcWindow.top;
        x1 = wi.rcWindow.left;
        y1 = wi.rcWindow.top;
    }

    HDC hdc = ::GetDC(NULL);
    if (hdc == NULL) return false;
    HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, x, y);

    // エラー判定用
    bool bSuccess = (hBitmap != NULL);

    // ここから先は成功したかを判定しながら進む。

    // メモリデバイスコンテキストを作成
    HDC hdcMem;
 //   if (bSuccess) {
        hdcMem = ::CreateCompatibleDC(hdc);
        bSuccess = (hdcMem != NULL);
 //   }

    // メモリデバイスコンテキストを選択
    HGDIOBJ hgdiobj;
 //   if (bSuccess) {
        bSuccess = ((hgdiobj = ::SelectObject(hdcMem, hBitmap)) != NULL);
 //   }

    // メモリデバイスコンテキストへビットマップを転送
    if (bSuccess) {
        bSuccess = (::BitBlt(hdcMem, 0, 0, x, y, hdc, x1, y1, SRCCOPY) != 0);
    }

    // クリップボードへコピー
    if (bSuccess && ::OpenClipboard(NULL)) {
        bSuccess = (::EmptyClipboard() != 0);
        if (bSuccess) bSuccess = (::SetClipboardData(CF_BITMAP, hBitmap) != NULL);
        ::CloseClipboard();
    }
    else bSuccess = false;

    // 後片付け
    if (hgdiobj != NULL) ::SelectObject(hdcMem, hgdiobj);
    if (hdcMem != NULL) ::DeleteDC(hdcMem);
    if (hBitmap != NULL) ::DeleteObject(hBitmap);
    if (hdc != NULL) ::ReleaseDC(hWnd, hdc);

    return bSuccess;
}
