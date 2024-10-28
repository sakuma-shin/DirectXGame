#pragma once
#include<Windows.h>
#include<dinput.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
using namespace Microsoft::WRL;

class Input
{
public:
	//namespace省略
	template<class T>using Comptr = Microsoft::WRL::ComPtr<T>;

	//初期化
	void Initialize(HINSTANCE hInstance,HWND hwnd);

	//更新
	void Update();

private:
	ComPtr<IDirectInputDevice8> keyboard = nullptr;
};

