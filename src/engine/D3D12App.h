#pragma once
//DirectX12�֘A�̋@�\���~���ɗ��p���邽�߂̃N���X================================================
#include"D3D12Data.h"
#include"DescriptorHeap.h"
#include"Color.h"

#include<map>
#include<functional>
#include<DirectXTex.h>
using namespace DirectX;

#include"Swapchain.h"

//D3D12App�𗘗p���̃N���X
class D3D12AppUser
{
protected:
	friend class D3D12App;
	D3D12AppUser() {}
	virtual ~D3D12AppUser(){}
	virtual void Render() = 0;	//�����_�����O�֘A�̏���
};

class D3D12App
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	static D3D12App* s_instance;
	//�_�u���o�b�t�@
	static const UINT s_frameBufferCount = 2;

	//���[�h�p�̃����_���̂��߂̌^
	using LoadLambda_t = std::function<HRESULT(const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)>;
	static std::map<std::string, LoadLambda_t>s_loadLambdaTable;

public:
	static D3D12App* Instance()
	{
		assert(s_instance != nullptr);
		return s_instance;
	}

private:
	//DirectX12�f�o�C�X
	ComPtr<ID3D12Device> m_device;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory6>m_dxgiFactory = nullptr;
	//�R�}���h�A���P�[�^
	std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
	ComPtr<ID3D12CommandAllocator>m_oneshotCommandAllocator;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> m_commandQueue;

	//�f�B�X�N���v�^�q�[�v
	std::unique_ptr<DescriptorHeap_CBV_SRV_UAV> m_descHeapCBV_SRV_UAV;
	std::unique_ptr<DescriptorHeap_RTV> m_descHeapRTV;
	std::unique_ptr<DescriptorHeap_DSV> m_descHeapDSV;

	//�X���b�v�`�F�C��
	std::unique_ptr<Swapchain>m_swapchain;
	DXGI_FORMAT m_backBuffFormat = DXGI_FORMAT_UNKNOWN;	//�o�b�N�o�b�t�@�̃t�H�[�}�b�g

	//���������J���[�e�N�X�`���L�^�p
	struct ColorTexture
	{
		Color m_color;
		int m_width;
		std::shared_ptr<TextureBuffer>m_tex;
	};
	std::list<ColorTexture>m_colorTextures;

	//���������摜�e�N�X�`���L�^�p
	struct LoadImgTexture
	{
		std::string m_path;
		std::vector<std::shared_ptr<TextureBuffer>>m_textures;
	};
	std::list<LoadImgTexture>m_loadImgTextures;

	//�摜�𕪊�����p�C�v���C��
	struct SplitImgConstData
	{
		Vec2<int> m_imgNum = { 0,0 };
		Vec2<int> m_splitSize;
	};
	int m_splitTexBuffCount = 0;	//SpliteTexBuffer���Ă΂ꂽ��
	std::vector<std::shared_ptr<ConstantBuffer>>m_splitImgConstBuff;
	std::shared_ptr<ComputePipeline>m_splitImgPipeline;

	void Initialize(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen);
	ComPtr<ID3D12RootSignature>GenerateRootSignature(const std::vector<RootParam>& RootParams, std::vector<D3D12_STATIC_SAMPLER_DESC>& Samplers);

public:
	D3D12App(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen = false)
	{
		assert(s_instance == nullptr);
		s_instance = this;
		Initialize(Hwnd, ScreenSize, UseHDR, ClearValue, IsFullScreen);
	}

	//�Q�b�^
	const ComPtr<ID3D12Device>& GetDevice() { return m_device; }
	const ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return m_commandList; }
	const DXGI_FORMAT& GetBackBuffFormat() { return m_backBuffFormat; }
	const std::shared_ptr<RenderTarget>& GetBackBuffRenderTarget() { return m_swapchain->GetBackBufferRenderTarget(); }
	ComPtr<ID3D12CommandAllocator>& GetOneShotCommandAllocator() { return m_oneshotCommandAllocator; }

	//�f�B�X�N���v�^�f�[�^�q�[�v�̃Z�b�g(�R���s���[�g�V�F�[�_�[�𗘗p����Ƃ��̓��[�U�[������Ăяo����悤��)
	void SetDescHeaps();
	//�����_�����O�iD3D12AppUser���p������ KuroEngine �ȊO�ł͌Ăяo���Ȃ��j
	void Render(D3D12AppUser* User);

	//CPU���\�[�X��GPU�ɏ��؂��ăR�s�[
	void UploadCPUResource(std::shared_ptr<GPUResource>& Dest, const size_t& DataSize, const int& ElementNum, const void* SendData);

	//���_�o�b�t�@����
	std::shared_ptr<VertexBuffer>GenerateVertexBuffer(const size_t& VertexSize, const int& VertexNum, void* InitSendData = nullptr, const char* Name = nullptr, const bool& RWStructuredBuff = false);
	//�C���f�b�N�X�o�b�t�@����
	std::shared_ptr<IndexBuffer>GenerateIndexBuffer(const int& IndexNum, void* InitSendData = nullptr, const char* Name = nullptr, const DXGI_FORMAT& IndexFormat = DXGI_FORMAT_R32_UINT);

	//�萔�o�b�t�@����
	std::shared_ptr<ConstantBuffer>GenerateConstantBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData = nullptr, const char* Name = nullptr);
	//�\�����o�b�t�@����
	std::shared_ptr<StructuredBuffer>GenerateStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData = nullptr, const char* Name = nullptr);
	//�o�͗p�o�b�t�@�쐬
	std::shared_ptr<RWStructuredBuffer>GenerateRWStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData = nullptr, const char* Name = nullptr);
	//�e�N�X�`���o�b�t�@����
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const Color& Color, const int& Width = 1, const DXGI_FORMAT& Format = DXGI_FORMAT_R32G32B32A32_FLOAT);	//�P�F�h��Ԃ��e�N�X�`������
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const std::string& LoadImgFilePath, const bool& SRVAsCube = false);	//�摜�t�@�C��
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const Vec2<int>&Size,const DXGI_FORMAT& Format,const char* Name = nullptr);	//�R���s���[�g�V�F�[�_�[�ł̕`�����ݐ�p�iUAV���쐬����j
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const std::vector<char>& ImgData, const int& Channel = -1);	//Channel�œ���̃`�����l���̂�R�`�����l���ɏo��
	//�e�N�X�`���𕪊����ăe�N�X�`������
	void GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name);
	/// <summary>
	/// �摜��ǂݍ���ŕ��� (LoadDivGraph)
	/// </summary>
	/// <param name="Array">���������摜�̏������ݐ�摜�z��</param>
	/// <param name="LoadImgFilePath">�摜�t�@�C���p�X</param>
	/// <param name="AllNum">��������</param>
	/// <param name="SplitNum">�c���̕�����</param>
	/// <param name="SplitSize">������̃T�C�Y</param>
	/// <returns></returns>
	void GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum);
	//Indirect�p�R�}���h�o�b�t�@����
	std::shared_ptr<IndirectCommandBuffer>GenerateIndirectCommandBuffer(const EXCUTE_INDIRECT_TYPE& IndirectType, const int& MaxCommandCount,
		const int& GpuAddressNum, const bool& CounterBuffer = false, const void* InitCommandData = nullptr, const char* Name = nullptr);

	//SRV�쐬�iD3D12App�o�R�j
	DescHandles CreateSRV(const ComPtr<ID3D12Resource>& Buff, const D3D12_SHADER_RESOURCE_VIEW_DESC& ViewDesc);
	//RTV�쐬�iD3D12App�o�R�j
	DescHandles CreateRTV(const ComPtr<ID3D12Resource>& Buff, const D3D12_RENDER_TARGET_VIEW_DESC* ViewDesc = nullptr);
	//DSV�쐬�iD3D12App�o�R�j
	DescHandles CreateDSV(const ComPtr<ID3D12Resource>& Buff, const D3D12_DEPTH_STENCIL_VIEW_DESC* ViewDesc = nullptr);
	//UAV�쐬�iD3D12App�o�R�j
	DescHandles CreateUAV(const ComPtr<ID3D12Resource>& Buff, const D3D12_UNORDERED_ACCESS_VIEW_DESC& ViewDesc, const ComPtr<ID3D12Resource>& CounterBuff = nullptr);

	//�����_�[�^�[�Q�b�g����
	std::shared_ptr<RenderTarget>GenerateRenderTarget(const DXGI_FORMAT& Format, const Color& ClearValue, const Vec2<int>Size,
	const wchar_t* TargetName = nullptr, D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_RENDER_TARGET, int MipLevel = 1, int ArraySize = 1);
	//�f�v�X�X�e���V���쐬
	std::shared_ptr<DepthStencil>GenerateDepthStencil(const Vec2<int>& Size, const DXGI_FORMAT& Format = DXGI_FORMAT_D32_FLOAT, const float& ClearValue = 1.0f);

	//�V�F�[�_�[�̃R���p�C��
	ComPtr<IDxcBlob>CompileShader(const std::string& FilePath, const std::string& EntryPoint, const std::string& ShaderModel);

	//�p�C�v���C������
	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	/// <param name="Option">�p�C�v���C���e��ݒ�</param>
	/// <param name="ShaderInfo">�V�F�[�_�[���</param>
	/// <param name="InputLayout">���V���C�A�E�g</param>
	/// <param name="RootParams">���[�g�p�����[�^�i�f�B�X�N���v�^���r���[����ɂȂ�̂Œ��Ӂj</param>
	/// <param name="RenderTargetFormat">�`��惌���_�[�^�[�Q�b�g�ݒ�</param>
	/// <param name="SamplerDesc">�T���v���[</param>
	std::shared_ptr<GraphicsPipeline>GenerateGraphicsPipeline(
		const PipelineInitializeOption& Option,
		const Shaders& ShaderInfo,
		const std::vector<InputLayoutParam>& InputLayout,
		const std::vector<RootParam>& RootParams,
		const std::vector<RenderTargetInfo>& RenderTargetFormat,
		std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers);

	//�R���s���[�g�p�C�v���C������
	std::shared_ptr<ComputePipeline>GenerateComputePipeline(
		const ComPtr<IDxcBlob>& ComputeShader,
		const std::vector<RootParam>& RootParams,
		std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers);

	//�C���_�C���N�g�@�\
	std::shared_ptr<IndirectDevice>GenerateIndirectDevice(const EXCUTE_INDIRECT_TYPE& ExcuteIndirectType, const std::vector<RootParam>& RootParams, std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers);

	//�o�b�N�o�b�t�@�����_�[�^�[�Q�b�g���Z�b�g
	void SetBackBufferRenderTarget();

	//�����V���b�g�R�}���h�A���P�[�^�[�𗘗p���ăR���s���[�g�p�C�v���C���𒼂��Ɏ��s
	void DispathOneShot(std::weak_ptr<ComputePipeline>Pipeline, Vec3<int>ThreadNum, std::vector<RegisterDescriptorData>& DescDatas);
};