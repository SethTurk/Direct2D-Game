#pragma once

#include <windows.h>

#include <stdlib.h>
#include <iostream>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class Game {
public:
	Game();
	~Game();

	HRESULT Initialize();
	
	void RunMessageLoop();
	void CheckInput();
	void TranslatePlayer();

private:
	int framesRendered = 0;

	float PosX = 0;
	float PosY = 0;

	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT OnRender();

	void DiscardDeviceResources();
	void OnResize(UINT width, UINT height);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_hwnd;
	ID2D1Factory* m_pDirect2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
};