#include <cassert>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <format>
#include <string>

#include "DirectXCommon.h"
#include "ExMath.h"
#include "Input.h"
#include "Vector2.h"
#include "Vector4.h"
#include "WinApp.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include"D3D12ResourceLeakChecker.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler")

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};


MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 1,中で必要となる変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line;          // ファイルから読んだ1行を格納するもの
	// 2,ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());                             // とりあえず開けなかったら止める
	// 3,実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	// 4,MaterialDataを返す
	return materialData;
}
//
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// 1.中で必要となる変数の宣言
	ModelData modelData;            // 構築するモデルデータ
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;   // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line;               // ファイルから読み込んだ1行を格納するもの

	// 2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());

	// 3.実際にファイルを読み込み
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				/*VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);*/

				position.x *= -1.0f;
				texcoord.y = 1.0f - texcoord.y;
				normal.x *= -1.0f;
				triangle[faceVertex] = {position, texcoord, normal};
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	// 4.ModelDataを返す
	return modelData;
}


void ReportLiveObjects() {
	ComPtr<ID3D12DebugDevice> debugDevice;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugDevice)))) {
		std::cout << "=== Reporting Live D3D12 Objects ===" << std::endl;
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	}
}

float Pay(int n, float firstPay) {
	if (n <= 1) {
		return firstPay;
	}

	return Pay(n - 1, firstPay * 2.0f - 50.0f);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3D12ResourceLeakChecker leakCheck;

	// ポインタ　
	WinApp* winApp = nullptr;

	// WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	// ポインタ　
	Input* input = nullptr;

	// 入力の初期化
	input = new Input();
	input->Initialize(winApp);

	// ポインタ
	DirectXCommon* dxCommon = nullptr;

	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	float pay = 100.0f;
	float hourlyPay = 1072.0f;
	int workTime = 8;

	// 再帰的な給料計算
	pay = Pay(workTime, pay);

	// 一般的な給料計算
	hourlyPay *= static_cast<float>(workTime);

	// 出力ウインドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	// 文字列を格納する
	std::string str0{"STRING!!!"};

	// 整数を文字列にする
	std::string str1{std::to_string(10)};

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Rootparameter作成。 複数設定できるので配列。 今回は結果1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;              // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           // PixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                              // レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;              // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;          // PixelShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                              // レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	descriptionRootSignature.pParameters = rootParameters;             // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // パイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//// バイナリを元に生成
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	assert(SUCCEEDED(hr));

	// InputLayOut
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// DepthStenicStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定

	// Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob = dxCommon->CompileShader(L"Resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob= dxCommon->CompileShader(L"Resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();                                           // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;                                                  // InputLayout
	graphicsPipelineStateDesc.VS = {vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize()}; // VertexShader
	graphicsPipelineStateDesc.PS = {pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize()};   // Pixel Shader
	graphicsPipelineStateDesc.BlendState = blendDesc;                                                         // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;                                               // RasterizerState

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ (形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// depthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	// モデル読み込み
	ModelData modelData = LoadObjFile("resources", "plane.obj");
	// 頂点リソースを作る

	ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	ComPtr<ID3D12Resource> materialResource = dxCommon->CreateBufferResource(sizeof(Vector4));

	ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);

	////dsvheapの先頭にdsvを作る
	/*device->createdepthstencilview(depthstencilresource, &dsvdesc, dsvdescriptorheap->getcpudescriptorhandleforheapstart());*/
	// dxCommon->CreateDepthStencilTextureResource(depthStencilResource, &dsvdesc, dsvdescriptorheap->getcpudescriptorhandleforheapstart())

	/*wvp用のリソースを作る。matarix4x41つ分にする*/
	ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	// データを書き込む
	TransformationMatrix* wvpData = nullptr;

	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	// 単位行列を書き込んでおく
	wvpData->World = MakeIdentity4x4();
	wvpData->WVP = MakeIdentity4x4();

	// マテリアルにデータを読み込む
	Vector4* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は赤を書き込んでみる
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースのコピー

	// 左下
	vertexData[0].position = {-0.5f, -0.5f, 0.0f, 1.0f};
	vertexData[0].texcoord = {0.0f, 1.0f};
	// 上
	vertexData[1].position = {0.0f, 0.5f, 0.0f, 1.0f};
	vertexData[1].texcoord = {0.5f, 0.0f};
	// 右下
	vertexData[2].position = {0.5f, -0.5f, 0.0f, 1.0f};
	vertexData[2].texcoord = {1.0f, 1.0f};

	// 左下2
	vertexData[3].position = {-0.5f, -0.5f, 0.5f, 1.0f};
	vertexData[3].texcoord = {0.0f, 1.0f};
	// 上2
	vertexData[4].position = {0.0f, 0.0f, 0.0f, 1.0f};
	vertexData[4].texcoord = {0.5f, 0.0f};
	// 右下2
	vertexData[5].position = {0.5f, -0.5f, -0.5f, 1.0f};
	vertexData[5].texcoord = {1.0f, 1.0f};

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	// 1枚目の三角形
	vertexDataSprite[0].position = {0.0f, 360.0f, 0.0f, 1.0f};
	vertexDataSprite[0].texcoord = {0.0f, 1.0f};
	vertexDataSprite[1].position = {0.0f, 0.0f, 0.0f, 1.0f};
	vertexDataSprite[1].texcoord = {0.0f, 0.0f};
	vertexDataSprite[2].position = {640.0f, 360.0f, 0.0f, 1.0f};
	vertexDataSprite[2].texcoord = {1.0f, 1.0f};
	vertexDataSprite[3].position = {640.0f, 0.0f, 0.0f, 1.0f};
	vertexDataSprite[3].texcoord = {1.0f, 0.0f};
	vertexDataSprite[4].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 左上
	vertexDataSprite[4].texcoord = {1.0f, 0.0f};
	vertexDataSprite[5].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
	vertexDataSprite[5].texcoord = {1.0f, 1.0f};

	ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するサイズはインデックス6個分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	Transform transform{
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 3.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
	Transform cameraTransform{
	    {1.0f, 1.0f, 1.0f },
        {0.0f, 0.0f, 0.0f },
        {0.0f, 0.0f, -5.0f}
    };
	Transform transformSprite{
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意
	ComPtr<ID3D12Resource> transformationMatrixResourceSprite = dxCommon->CreateBufferResource(sizeof(Matrix4x4));
	// データを書き込む
	Matrix4x4* transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	*transformationMatrixDataSprite = MakeIdentity4x4();

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon->LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
	dxCommon->UpLoadTextureData(textureResource.Get(), mipImages);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	// SRVを作成する DescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	/*D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetCPUDescriptorHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();*/
	//

	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	// ウインドウの×ボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		// ゲームの処理

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// 開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム用の処理に置き換える
		// ImGui::ShowDemoWindow();

		input->Update();

		// 数字のキーが押されていたら
		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n"); // 出力ウインドウに表示
		}

		ImGui::Begin("Window");
		ImGui::DragFloat3("modelScale", &transform.scale.x, 0.01f);
		ImGui::DragFloat3("modelRotate", &transform.rotate.x, 0.01f);
		ImGui::DragFloat3("modelTranslate", &transform.translate.x, 0.01f);
		ImGui::DragFloat3("spriteTranslate", &transformSprite.translate.x, 0.01f);
		ImGui::DragFloat3("spriteScale", &transformSprite.scale.x, 0.01f);
		ImGui::DragFloat3("spriteRotate", &transformSprite.rotate.x, 0.01f);
		ImGui::ColorEdit4("color", &materialData->x, 0.01f);
		ImGui::End();

		///*transform.rotate.y += 0.03f;*/
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kWindowWidth / WinApp::kWindowHeight, 0.1f, 100.0f);
		// WVPMatrixを作る
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		wvpData->World = worldViewProjectionMatrix;
		wvpData->WVP = worldViewProjectionMatrix;

		// sprite用のworldViewProjectionMatrixを作る
		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 100.0f);
		// WVPMatrixを作る
		Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

		*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;

		// 描画前処理
		dxCommon->PreDraw();

		////RootSignatureを設定。　PSOの設定しているけど別で設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); // PSOを設定
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);   // VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じ物を設定すると考えておけばいい。
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルCBufferの場所を設定
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。　2はrootParameter[2]である。
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		// ImGuiの内部コマンドを生成する
		ImGui::Render(); ///////////
		// 描画
		// dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
		// 描画
		dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		////Spriteの描画
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBVを設定
		dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);

		////TransformationMatrixCBufferの場所を設定
		/*dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());*/
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());

		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		// 描画
		dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		////実際のcomandListのImGuiの描画コマンドを挟む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList()); //////////

		dxCommon->PostDraw();
	}

	
	
	winApp->Finalize();
	//
	//	//入力関数
	delete input;
	delete winApp;
	delete dxCommon;

	return 0;
}

int main() {

	

	float pay = 100.0f;
	float hourlyPay = 1072.0f;
	int workTime = 8;

	// 再帰的な給料計算
	pay = Pay(workTime, pay);

	// 一般的な給料計算
	hourlyPay *= static_cast<float>(workTime);

	SetConsoleOutputCP(65001);

	printf("一般的な資金体系の%d時間の給料は%f円\n", workTime, hourlyPay);
	printf("再帰的な資金体系の%d時間の給料は%f円\n", workTime, pay);

	return 0;
}
