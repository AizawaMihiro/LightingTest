// WindowsFirstGame.cpp : アプリケーションのエントリ ポイントを定義します。

/*DirectX 11 課題
①
各ウィンドウ クラスには、同じクラスのすべてのウィンドウで共有される関連付けられたウィンドウ プロシージャがあります。 ウィンドウ プロシージャは、そのクラスのすべてのウィンドウのメッセージを処理するため、動作と外観を制御します。
プロセスでは、そのクラスのウィンドウを作成する前に、ウィンドウ クラスを登録する必要があります。 ウィンドウ クラスを登録すると、ウィンドウ プロシージャ、クラス スタイル、およびその他のクラス属性がクラス名に関連付けられます。 プロセスが CreateWindow 関数または CreateWindowEx 関数でクラス名を指定すると、そのクラス名に関連付けられたウィンドウ プロシージャ、スタイル、およびその他の属性を含むウィンドウが作成されます。

要素	目的
クラス名	クラスを他の登録済みクラスと区別します。

ウィンドウ プロシージャ のアドレス	        クラス内のウィンドウに送信されるすべてのメッセージを処理し、ウィンドウの動作を定義する関数へのポインター。
インスタンス ハンドル	                    クラスを登録したアプリケーションまたは.dllを識別します。

クラス カーソル	                            クラスのウィンドウに対してシステムが表示するマウス カーソルを定義します。

クラス アイコン	                            大きなアイコンと小さいアイコンを定義します。

クラスの背景ブラシ	                        ウィンドウを開くか塗りつぶすときにクライアント領域を塗りつぶす色とパターンを定義します。

[クラス] メニュー	                        メニューを明示的に定義しないウィンドウの既定のメニューを指定します。

クラス スタイル	                            ウィンドウを移動またはサイズ変更した後にウィンドウを更新する方法、マウスのダブルクリックを処理する方法、デバイス コンテキストの領域を割り当てる方法、ウィンドウのその他の側面を定義します。

追加のクラス メモリ                         システムがクラス用に予約する必要がある余分なメモリの量をバイト単位で指定します。 クラス内のすべてのウィンドウは追加のメモリを共有し、アプリケーション定義の目的で使用できます。 システムは、このメモリを 0 に初期化します。

追加のウィンドウ メモリ	                    クラスに属する各ウィンドウに対してシステムが予約する必要がある余分なメモリの量をバイト単位で指定します。 余分なメモリは、任意のアプリケーション定義の目的で使用できます。 システムは、このメモリを 0 に初期化します。

*/

#include "framework.h"
#include "Main.h"
#include "Engine/Direct3D.h"
#include "Engine/Camera.h"
#include "Engine/Transform.h"
#include "Engine/Input.h"
#include "Engine/RootJob.h"
#include "Engine/Model.h"
#include "Stage.h"
#include "resource.h"

#pragma comment(lib,"winmm.lib") //timeGetTimeを使うために必要

HWND hWnd = nullptr;//ウィンドウの管理を行う番号を入れる型を定義（複数窓用意する場合は複数個宣言）


#define MAX_LOADSTRING 100

const wchar_t* WIN_CLASS_NAME = L"SAMPLE GAME WINDOW";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

RootJob* pRootJob = nullptr;

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ManuProc(HWND, UINT, WPARAM, LPARAM);

/*②
[in] hInstance
型: HINSTANCE    アプリケーションの現在のインスタンスへのハンドル。

[in, optional] hPrevInstance
型: HINSTANCE    アプリケーションの前のインスタンスへのハンドル。 このパラメーターは常に NULL。 別のインスタンスが既に存在するかどうかを検出する必要がある場合は、CreateMutex 関数を使用して一意の名前付きミューテックスを作成します。 ミューテックスが既に存在する場合でも、CreateMutex は成功しますが、関数は ERROR_ALREADY_EXISTSを返します。 これは、最初にミューテックスが作成されたため、アプリケーションの別のインスタンスが存在することを示します。 ただし、悪意のあるユーザーは、実行する前にこのミューテックスを作成し、アプリケーションが起動しないようにすることができます。 このような状況を回避するには、ランダムに名前付きミューテックスを作成し、承認されたユーザーのみが取得できるように名前を格納します。 または、この目的でファイルを使用することもできます。 アプリケーションをユーザーごとに 1 つのインスタンスに制限するには、ユーザーのプロファイル ディレクトリにロックされたファイルを作成します。

[in] lpCmdLine
型: LPSTR    プログラム名を除く、アプリケーションのコマンド ライン。 コマンド ライン全体を取得するには、GetCommandLine 関数を使用します。

[in] nShowCmd
型: int      ウィンドウの表示方法を制御します。 このパラメーターには、ShowWindow 関数の nCmdShow パラメーターで指定できる任意の値を指定できます。
*/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    //szWindowClass = WIN_CLASS_NAME;

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);//タイトルバー部分の文字列の設定
    LoadStringW(hInstance, IDC_WINDOWSFIRSTGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    //ウィンドウサイズの計算
    RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
    int winW = winRect.right - winRect.left;     //ウィンドウ幅
    int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HRESULT hr;
    hr = Direct3D::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, hWnd);
    if (FAILED(hr))
    {
        return 0;
    }

	//int angle = 0;

    Input::Initialize(hWnd);
    Camera::Initialize();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSFIRSTGAME));

    MSG msg = {};

	pRootJob = new RootJob(nullptr);
	pRootJob->Initialize();

	HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ManuProc);
	ShowWindow(hDialog, SW_SHOW);

    if (FAILED(hr))
    {
		return 0;
    }

    // メイン メッセージ ループ:
    while (msg.message != WM_QUIT)
    {
        //メッセージあり
        while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        //メッセージなし
        {
            timeBeginPeriod(1);

			static int FrameCnt = 0;
			static DWORD startTime = timeGetTime();
			DWORD nowTime = timeGetTime();

			static DWORD lastUpdateTime = nowTime;

            timeEndPeriod(1);

            if (nowTime - startTime >= 1000.f)
            {
                string str = "FPS:" + std::to_string(FrameCnt);
                SetWindowTextA(hWnd, str.c_str());

				FrameCnt = 0;
				startTime = nowTime;
            }
			//60FPS制限
            if (nowTime - lastUpdateTime <= 1000.f/60)
            {
                continue;
            }
			lastUpdateTime = nowTime;
			FrameCnt++;

            //ゲームの処理
            Camera::Update();

            //入力情報の更新
			Input::Update();

			pRootJob->UpdateSub();

            //描画処理
            Direct3D::BeginDraw();

			//pRootJobから全てのオブジェクトを辿って描画していく
			pRootJob->DrawSub();

			Direct3D::EndDraw();

            if (Input::IsKeyDown(DIK_D))
            {
                HWND hdialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DlgProc);
                ShowWindow(hdialog, SW_SHOW);
            }


            if (Input::IsKeyDown(DIK_ESCAPE))
            {
                static int cnt = 0;
                cnt++;
                if (cnt >= 3)
                {
                    PostQuitMessage(0);
                }
            }
        }
    }
	Model::Release();
	Input::Release();
	pRootJob->ReleaseSub();
    Direct3D::Release();

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
    /*③
    [in] dwExStyle
    型: DWORD
    作成されるウィンドウの拡張ウィンドウ スタイル。 使用可能な値の一覧については、「拡張ウィンドウ スタイルの」を参照してください。

    [in, optional] lpClassName
    型: LPCTSTR
    null-terminated 文字列またはクラスアトム。
    null-terminated 文字列の場合は、ウィンドウ クラス名を指定します。 クラスを登録するモジュールがウィンドウを作成するモジュールでもある場合、クラス名には、RegisterClass または RegisterClassEx 関数に登録されている任意の名前を指定できます。

    [in, optional] lpWindowName
    型: LPCTSTR
    ウィンドウ名。 ウィンドウ スタイルでタイトル バーが指定されている場合、lpWindowName 指すウィンドウ タイトルがタイトル バーに表示されます。

    [in] dwStyle
    型: DWORD
    作成するウィンドウのスタイル。 このパラメーターには、ウィンドウ スタイル値と、コントロール スタイルの組み合わせを指定できます。

    [in] X
    型: int
    ウィンドウの最初の水平位置。 重なったウィンドウまたはポップアップ ウィンドウの場合、x パラメーターは、ウィンドウの左上隅の最初の x 座標 (画面座標) です。

    [in] Y
    型: int
    ウィンドウの最初の垂直方向の位置。 重なったウィンドウまたはポップアップ ウィンドウの場合、y パラメーターは、ウィンドウの左上隅の初期 y 座標 (画面座標) です。

    [in] nWidth
    型: int
    ウィンドウの幅 (デバイス単位)。 重なり合うウィンドウの場合、nWidth はウィンドウの幅、画面座標、または CW_USEDEFAULTです。

    [in] nHeight
    型: int
    ウィンドウの高さ (デバイス単位)。 重なり合うウィンドウの場合、nHeight はウィンドウの高さ (画面座標) です。

    [in, optional] hWndParent
    型: HWND
    作成されるウィンドウの親ウィンドウまたは所有者ウィンドウへのハンドル。 子ウィンドウまたは所有ウィンドウを作成するには、有効なウィンドウ ハンドルを指定します。

    [in, optional] hMenu
    型: HMENU
    メニューのハンドル。または、ウィンドウのスタイルに応じて、子ウィンドウ識別子を指定します。

    [in, optional] hInstance
    型: HINSTANCE
    ウィンドウに関連付けるモジュールのインスタンスへのハンドル。

    [in, optional] lpParam
    型: LPVOID
    WM_CREATE メッセージの lParam パラメーターが指す CREATESTRUCT 構造体 (lpCreateParams メンバー) を介してウィンドウに渡される値へのポインター。 このメッセージは、戻る前に、この関数によって作成されたウィンドウに送信されます。

    戻り値
    型: HWND
    関数が成功した場合、戻り値は新しいウィンドウへのハンドルです。
    関数が失敗した場合、戻り値は NULL。 拡張エラー情報を取得するには、GetLastError呼び出します。
    */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSFIRSTGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
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

   RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
   AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
   int winW = winRect.right - winRect.left;     //ウィンドウ幅
   int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);//ここでウィンドウを作るので返り値をもらっておく

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

//各ウィンドウに何かが起こったことを通知する関数　OSから勝手に呼ばれる
//hWnd...受け取ったウィンドウのハンドル
//massage...何が起きたか
//wParam・lParam...追加情報のパラメータ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)//送られたメッセージで処理を分岐
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
    case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
           Input::SetMouseWindowPos(x, y);
            //mousePosition = XMVectorSet((float)x, (float)y, 0.0f, 0.0f);
            OutputDebugStringA((std::to_string(x) + "," + std::to_string(y) + "\n").c_str());
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/*
④
メッセージループとコールバックは、GUIアプリケーションやイベント駆動型プログラミングにおいて、非同期処理を実現するための重要な要素です。メッセージループは、アプリケーションがイベントやメッセージを処理するための中心的な仕組みであり、コールバック関数は、特定のイベントや条件が発生したときに実行される関数です。﻿
メッセージループは、コールバック関数を呼び出すための基盤を提供し、コールバック関数は、メッセージループによって処理されるイベントに対応する処理を実装します。
*/

//基本的なダイアログ ボックスのメッセージ ハンドラーです。
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

// ダイアログプロシージャ
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ((Stage*)pRootJob->FindObject("Stage"))->localProc(hDlg, message, wParam, lParam);
}

INT_PTR CALLBACK ManuProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ((Stage*)pRootJob->FindObject("Stage"))->manuProc(hDlg, message, wParam, lParam);
}
