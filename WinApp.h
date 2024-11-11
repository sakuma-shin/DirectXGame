#pragma once
#include<cstdint>
#include<Windows.h>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



class WinApp
{
public: 
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	//初期化
	void Initialize();
	//更新
	void Update();

	//クライアント領域のサイズ
	static const int32_t kCilentWidth = 1280;
	static const int32_t kCilentHeight = 720;

	HWND GetHwnd()const { return hwnd; }
	HINSTANCE GetHinstance()const { return wc.hInstance; }

private:
	//ウインドウハンドル
	HWND hwnd = nullptr;

	WNDCLASS wc{};
};

