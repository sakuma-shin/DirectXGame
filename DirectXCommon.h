#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>


//DirectX基盤
class DirectXCommon {
public:
	void Initialize();

private:
	ID3D12Device* device = nullptr;
};