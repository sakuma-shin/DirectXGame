#pragma once
#include<Windows.h>
#include<dinput.h>
#include<wrl.h>
#include"WinApp.h"
#define DIRECTINPUT_VERSION 0x0800
using namespace Microsoft::WRL;

class Input
{
public:
	//namespace省略
	template<class T>using Comptr = Microsoft::WRL::ComPtr<T>;

	//初期化
	void Initialize(WinApp* winApp);

	//更新
	void Update();

	

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	bool PushKey(BYTE keyNumber);

private:
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	//前回のキーの状態
	BYTE keyPre[256] = {};

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput = nullptr;

	WinApp* winApp_ = nullptr;
};

