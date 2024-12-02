#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include"WinApp.h"
#include"Logger.h"



//DirectX基盤
class DirectXCommon {
public:
	void Initialize(WinApp* winapp);

	void DeviceInitialize();

	void CommandInitialize();

	void SwapChainCreate(WinApp* winApp);

	void CreateDepthStencilTextureResource();

	void DescriptorHeapCreate();

	void RTVInitialize();

	void ZStencilViewInitialize();

	void FenceCreate();

	void ViewPortInitialize();

	void ScissorRectInitialize();

	void DXCCompilerCreate();

	void ImGuiInitilalize();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private:
	

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	//windowsAPI
	WinApp* winApp_ = nullptr;

	HRESULT hr_;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
};