#include <Windows.h>
#include <d3dx9.h>
#include <tchar.h>
#include <dinput.h>

//DirectXの本体
LPDIRECT3D9 pDirect3d;
//描画のためのデバイス情報
LPDIRECT3DDEVICE9 pDevice;
LPDIRECTINPUT8 pDinput = NULL;
LPDIRECTINPUTDEVICE8 pkey = NULL;
HRESULT InitDinput(HWND hWnd);
//入力キーの最大数

void UpdateKeyStatus();
//キー入力関数

bool GetKeyStatus(int KeyNumber);
HRESULT InitDinput(HWND hInst);

HRESULT InitD3d(HWND hInst, const TCHAR* filePath);
const int D3DFVF_CUSTOMVERTEX(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
LPDIRECT3DTEXTURE9 pTexture;
void InitPresentParameters(HWND);
HRESULT BuildDxDevice(HWND hInst, const TCHAR* filePath);
//const int D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
static const int MAX_KEY_NUMBER = 256; //入力キー判定のマスク値
const int MASK_NUM = 0x80;
BYTE KeyState[MAX_KEY_NUMBER];
//キーステータス更新関数
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

struct CUSTOMVERTEX {
	float x, y, z; //頂点座標
	float rhw; // 除算数
	DWORD dwColor; //頂点の色
	float tu, tv; // テクスチャの座標
};

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szstr, INT iCmdshow)
{
	MSG msg;
	static char szAppName[] = "Exercise";
	WNDCLASSEX wndclass;

	CUSTOMVERTEX v[4] =
	{
	{170.0f, 110.0f, 0.0f, 1.0f, 0xffffffff,0.0f,0.0f},
	{470.0f, 110.0f, 0.0f, 1.0f, 0xffffffff,1.0f,0.0f},
	{470.0f, 410.0f, 0.0f, 1.0f, 0xffffffff,1.0f,1.0f},
	{170.0f, 410.0f, 0.0f, 1.0f, 0xffffffff,0.0f,1.0f}
	}; //ポリゴンの描画

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = wndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);
	//	ウィンドウ生成
	HWND hInsT = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInst, NULL);

	ShowWindow(hInsT, SW_SHOW);
	UpdateWindow(hInsT);
	if (FAILED(InitDinput(hInsT))) {
		return 0;
	}

	//BuildDxDevice(hInsT, _T("hopping.jpg"));
	BuildDxDevice(hInsT, _T("hopping.jpg"));

	timeBeginPeriod(1);
	//今の時間をtimeに保存。
	DWORD time = timeGetTime();
	DWORD prevtime = 0;

	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			time = timeGetTime();
			if (time - prevtime < 1000 / 60) {

				pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0, 0);
				pDevice->BeginScene();

				UpdateKeyStatus();
				if (GetKeyStatus(DIK_RETURN)) {
					break;
				}
				if (GetKeyStatus(DIK_UP))
				{
					v[0].y -= 5.0f;
					v[1].y -= 5.0f;
					v[2].y -= 5.0f;
					v[3].y -= 5.0f;
				}
				if (GetKeyStatus(DIK_DOWN)) {
					v[0].y += 5.0f;
					v[1].y += 5.0f;
					v[2].y += 5.0f;
					v[3].y += 5.0f;
				}
				if (GetKeyStatus(DIK_LEFT)) {
					v[0].x -= 5.0f;
					v[1].x -= 5.0f;
					v[2].x -= 5.0f;
					v[3].x -= 5.0f;
				}
				if (GetKeyStatus(DIK_RIGHT)) {
					v[0].x += 5.0f;
					v[1].x += 5.0f;
					v[2].x += 5.0f;
					v[3].x += 5.0f;
				}
				pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
				pDevice->SetTexture(0, pTexture);
				pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(CUSTOMVERTEX));
				pDevice->EndScene();
				pDevice->Present(0, 0, 0, 0);
			}


			prevtime = time;

		}
		timeEndPeriod(1);

	}
	if (pkey)
	{
		pkey->Unacquire();
	}
	pkey->Release();
	pkey = nullptr;
	pDinput->Release();
	pDinput = nullptr;
	pDevice->Release();
	pDevice = nullptr;
	pDirect3d->Release();
	pDirect3d = nullptr;
	pTexture->Release();
	pTexture = nullptr;

	return (int)msg.wParam;
}

HRESULT BuildDxDevice(HWND hInst, const TCHAR* filePath) {
	if (FAILED(InitD3d(hInst, filePath))) {
		return E_FAIL;
	}
	if (FAILED(InitDinput(hInst))) {
		return E_FAIL;
	}
	//D3Dのポインタ変数にDirect3DCreate9関数(Direct3Dを作る関数)で作成したものを代入
	pDirect3d = Direct3DCreate9(D3D_SDK_VERSION);

	//D3Dのポインタ変数がNULLだった時
	if (pDirect3d == NULL) {
		//D3Dの作成に失敗した時の処理
		MessageBox(0, _T("Direct3Dの作成に失敗しました"), NULL, MB_OK);
		return E_FAIL;
	}
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//頂点情報をセット
	//pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	return S_OK;
}

LRESULT CALLBACK wndProc(HWND hInst, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch ((CHAR)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	}

	return DefWindowProc(hInst, iMsg, wParam, lParam);
}

	HRESULT InitD3d(HWND hInst, const TCHAR * filePath)
	{
		if (NULL == (pDirect3d = Direct3DCreate9(D3D_SDK_VERSION)))
		{
			MessageBox(0, "Direct3D失敗", "", MB_OK);
			return E_FAIL;
		}

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = true;

		if (FAILED(pDirect3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hInst, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice))) {
			MessageBox(0, "HALモードでDIRECT３D", NULL, MB_OK);
			return E_FAIL;
		}
		if (FAILED(D3DXCreateTextureFromFileEx(pDevice, "pinos.jpg", 450, 351, 0, 0, D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT,
			0xff000000, NULL, NULL, &pTexture)))
		{
			MessageBox(0, _T("テクスチャオブジェクトの作成に失敗しました"), NULL, MB_OK);
			return E_FAIL;
		}
		return S_OK;
	}

	void initrender()
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}
	//描画するための自作関数
	//VOID draw()
	//{
	//	
	//
	//	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0, 0);
	//	if (SUCCEEDED(pDevice->BeginScene()))
	//	{
	//		UpdateKeyStatus();
	//		if (GetKeyStatus(DIK_RETURN)) {
	//			
	//		}
	//		if (GetKeyStatus(DIK_UP))
	//		{
	//			v[0].y -= 5.0f;
	//			v[1].y -= 5.0f;
	//			v[2].y -= 5.0f;
	//			v[3].y -= 5.0f;
	//		}
	//		if (GetKeyState(DIK_DOWN)) {
	//			v[0].y += 5.0f;
	//			v[1].y += 5.0f;
	//			v[2].y += 5.0f;
	//			v[3].y += 5.0f;
	//		}
	//		if (GetKeyState(DIK_LEFT)) {
	//			v[0].x -= 5.0f;
	//			v[1].x -= 5.0f;
	//			v[2].x -= 5.0f;
	//			v[3].x -= 5.0f;
	//		}
	//		if (GetKeyState(DIK_RIGHT)) {
	//			v[0].x += 5.0f;
	//			v[1].x += 5.0f;
	//			v[2].x += 5.0f;
	//			v[3].x += 5.0f;
	//		}
	//		pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	//		pDevice->SetTexture(0, pTexture);
	//		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(CUSTOMVERTEX));
	//
	//		pDevice->EndScene();
	//	}
	//	pDevice->Present(0, 0, 0, 0);
	//}

/* 
HINSTANCE アプリハンドル, DWORD Direct Input バージョン, REFIID インターフェースの識別子,
LPVOID* IDirectInput8 インターフェースポインタの受け取り用, LPUNKNOWN 基本的にNULL
*/

/* 
if (FAILED(hr))
{
	//　失敗
}
*/

// LPDIRECTINPUTDEVICE device;

/*
hr = g_pInputInterface->CreateDevice(
	GUID_SysKeyboard,
	&device,
	NULL); 
*/

/*
REFGUID 使用するデバイスの指定　EX) Keyboard の場合 GUID_SysKeyBoard,
LPDIRECTINPUTDEVICE* IDirectInputDevice8インターフェース受け取り用,
LPUNKNOWN 基本的にNULL
*/

/*
if (FAILED(hr))
{
	//失敗
}
*/

//Device Format の設定

/*
HRESULT hr = g_pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
if (FAILED(hr))
*/

/*
LPCDIDATAFORMAT 設定する入力デバイスのフォーマット（デバイスはDirectInput側で用意されてる)
Keyboard => c_dfDIKeyboard
Mouse => c_dfDIMouse
Joystick => c_dfDIJoystick
*/

/*
{
	//失敗
}
*/

//協調モードの設定

/*
HRESULT hr = g_pKeyDevice->SetCooperativeLevel(window_handle,
	DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); //　バックグラウンド　、　非排他的
*/

/*
HWND 入力デバイスが関連付けられてるWindowsHandle(入力を受けるWindow),
DWORD 協調レベルの設定フラグ　（FOREGROUND , BACKGROUND と　EXCLUSIVE , NONEXCLUSIVE)
*/

/*
if (FAILED(hr))
{
	//失敗
}
*/

// HRESULT hr = g_pKeyDevice->Acquire();
/*
Acquire 制御開始の成否(HRESULT)が返る
*/

/*
if (FAILED(hr))
{
	//　制御開始失敗
}
*/

HRESULT InitDinput(HWND hInst)
{
	HRESULT hr;
	//DirectInput８の作成
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID * *)& pDinput, NULL)))
	{
		return hr;
	}
	//InputDeviceを作成
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysKeyboard, &pkey, NULL)))
	{
		return hr;
	}
	//フォーマットの設定,形式
	if (FAILED(hr = pkey->SetDataFormat(&c_dfDIKeyboard)))
	{
		return hr;
	}
	//協調レベルを設定
	if (FAILED(hr = pkey->SetCooperativeLevel(hInst, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return hr;
	}
	//権限の取得
	pkey->Acquire();
	return S_OK;
}

//キーステータス更新関数
void UpdateKeyStatus()
{
	HRESULT hr = pkey->Acquire();
	if ((hr == DI_OK) || (hr == S_FALSE)) {
		pkey->GetDeviceState(sizeof(KeyState), &KeyState);

	}
}

//キー入力関数
bool GetKeyStatus(int KeyNumber)
{
	if (KeyState[KeyNumber] & 0x80)
	{
		return true;
	}
	return false;
}