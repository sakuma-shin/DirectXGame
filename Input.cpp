#include "Input.h"
#include<cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")



void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{

	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);

	assert(SUCCEEDED(result));
	
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	//前回のキー入力状態を保存
	memcpy(keyPre, key, sizeof(key));

	//キーボード情報の取得開始
	keyboard->Acquire();
	
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (!keyPre[keyNumber]&&key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::PushKey(BYTE keyNumber)
{

	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalse
	return false;
}


