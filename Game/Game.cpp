#include "Game.h"

Game::Game() : m_hwnd(NULL), m_pDirect2DFactory(NULL), m_pRenderTarget(NULL), m_pLightSlateGrayBrush(NULL), m_pCornflowerBlueBrush(NULL) {

}

Game::~Game() {
	SafeRelease(&m_pDirect2DFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
}

void Game::TranslatePlayer() {
	if (m_pRenderTarget) {
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(PosX, PosY));
	}
}

void Game::CheckInput() {
	if (GetKeyState(VK_RIGHT) & 0x8000) {
		PosX++;
	}

	if (GetKeyState(VK_DOWN) & 0x8000) {
		PosY++;
	}

	if (GetKeyState(VK_LEFT) & 0x8000) {
		PosX--;
	}

	if (GetKeyState(VK_UP) & 0x8000) {
		PosY--;
	}
}

HRESULT Game::OnRender() {
	HRESULT hr = S_OK;
	hr = CreateDeviceResources();

	if (SUCCEEDED(hr)) {
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

		for (int x = 0; x < width; x += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f), D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height), m_pLightSlateGrayBrush, 0.5f);
		}

		for (int y = 0; y < height; y += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)), D2D1::Point2F(rtSize.width, static_cast<float>(y)), m_pLightSlateGrayBrush, 0.5f);
		}

		D2D1_RECT_F rectangle1 = D2D1::RectF(rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f, rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f);
		D2D1_RECT_F rectangle2 = D2D1::RectF(rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f, rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f);
		
		m_pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush);
		TranslatePlayer();

		m_pRenderTarget->FillRectangle(&rectangle2, m_pCornflowerBlueBrush);

		hr = m_pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET) {
			hr = S_OK;
			DiscardDeviceResources();
		}

		framesRendered++;
	}

	return hr;
}

void Game::RunMessageLoop() {
	MSG msg;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			if (GetActiveWindow() == m_hwnd)
				CheckInput();
			OnRender();
		}
	}
}

HRESULT Game::Initialize() {
	HRESULT hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr)) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Game::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DGame";

		RegisterClassEx(&wcex);

		FLOAT dpiX, dpiY;

		m_pDirect2DFactory->GetDesktopDpi(&dpiX, &dpiY);
		m_hwnd = CreateWindow(
			L"D2DGame",
			L"Direct2D Game",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(480.f * dpiX / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this);

		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL))) {
		{
			Game game;

			if (SUCCEEDED(game.Initialize())) {
				game.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

HRESULT Game::CreateDeviceIndependentResources() {
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2DFactory);
	return hr;
}

HRESULT Game::CreateDeviceResources() {
	HRESULT hr = S_OK;
	if (!m_pRenderTarget) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = m_pDirect2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}

		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
	}

	return hr;
}

void Game::DiscardDeviceResources() {
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
}

LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Game *pGame = (Game *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pGame));

		result = 1;
	}
	else {
		Game *pGame = reinterpret_cast<Game *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pGame) {
			switch (message) {
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pGame->OnResize(width, height);
			}

			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			{
				pGame->OnRender();
				ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DESTROY: 
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}

		if (!wasHandled) {
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}

void Game::OnResize(UINT width, UINT height) {
	if (m_pRenderTarget) {
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}