#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include"WinApp.h"
#include"Logger.h"
#include<array>
#include <format>
#include <dxcapi.h>
#include"StringUtility.h"




//DirectX基盤
class DirectXCommon {
public:
	void Initialize(WinApp* winapp);

	void DeviceInitialize();

	void CommandInitialize();

	void SwapChainCreate(WinApp* winApp);

	void DepthBufferCreate();

	void DescriptorHeapCreate();

	void RTVInitialize();

	void DepthStencilViewInitialize();

	void FenceCreate();

	void ViewPortInitialize();

	void ScissorRectInitialize();

	void DXCCompilerCreate();

	void ImGuiInitilalize();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource( int32_t width, int32_t height);

	//描画前処理
	void PreDraw();

	//描画後処理
	void PostDraw();

private:
	

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	//windowsAPI
	WinApp* winApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2>swapChainResources;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	// DescriptorSizeを取得しておく
	uint32_t descriptorSizeSRV = 0;
	uint32_t descriptorSizeRTV = 0;
	uint32_t descriptorSizeDSV = 0;


};