#pragma once
#include<d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include<dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include<wrl.h>
#include <cassert>
#include"Vec.h"
#include"KuroFunc.h"
#include"Color.h"


enum DESC_HANDLE_TYPE { CBV, SRV, UAV, RTV, DSV, DESC_HANDLE_NUM };

//�f�B�X�N���v�^�n���h��
class DescHandles
{
private:
	//����������Ă��邩
	bool m_invalid = true;
	//CPU�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
	//GPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;

public:
	//�R���X�g���N�^
	DescHandles() {}
	DescHandles(const DescHandles& tmp)
	{
		assert(tmp);
		Initialize(tmp.m_cpuHandle, tmp.m_gpuHandle);
	}
	DescHandles(DescHandles&& tmp)
	{
		assert(tmp);
		Initialize(tmp.m_cpuHandle, tmp.m_gpuHandle);
	}
	DescHandles(const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHandle) { Initialize(CPUHandle, GPUHandle); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle()const { assert(!m_invalid); return m_cpuHandle; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle()const { assert(!m_invalid); return m_gpuHandle; }

	//������
	void Initialize(const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHandle)
	{
		m_cpuHandle = CPUHandle;
		m_gpuHandle = GPUHandle;
		m_invalid = false;
	}

	// if ���ɂ��̂܂܂����ď����ς݂�����o����悤��
	operator bool()const { return !m_invalid; }
	//������Z�q
	void operator=(const DescHandles& rhs)
	{
		assert(!rhs.m_invalid);
		m_cpuHandle = rhs.m_cpuHandle;
		m_gpuHandle = rhs.m_gpuHandle;
		m_invalid = false;
	}

	operator const D3D12_CPU_DESCRIPTOR_HANDLE& ()const
	{
		assert(!m_invalid);
		return m_cpuHandle;
	}
	operator const D3D12_CPU_DESCRIPTOR_HANDLE* ()const
	{
		assert(!m_invalid);
		return &m_cpuHandle;
	}
	operator const D3D12_GPU_DESCRIPTOR_HANDLE& ()const
	{
		assert(!m_invalid);
		return m_gpuHandle;
	}
	operator const D3D12_GPU_DESCRIPTOR_HANDLE* ()const
	{
		assert(!m_invalid);
		return &m_gpuHandle;
	}
};

class DescHandlesContainer
{
private:
	DescHandles m_handles[DESC_HANDLE_NUM];

public:
	void Initialize(const DESC_HANDLE_TYPE& Type, const DescHandles& Handle)
	{
		m_handles[Type] = Handle;
	}
	const DescHandles& GetHandle(const DESC_HANDLE_TYPE& Type)const { return m_handles[Type]; }
};

//GPU���\�[�X�i�f�[�^�R���e�i�j
class GPUResource
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GPUResource() = delete;
	GPUResource(const GPUResource& tmp) = delete;
	GPUResource(GPUResource&& tmp) = delete;

	void* m_buffOnCPU = nullptr;//CPU������A�N�Z�X�ł��邷��o�b�t�@�̃A�h���X
	bool m_mapped = false;
	ComPtr<ID3D12Resource>m_buff = nullptr;	//���\�[�X�o�b�t�@
	D3D12_RESOURCE_STATES m_barrier = D3D12_RESOURCE_STATE_COMMON;	//���\�[�X�o���A�̏��

public:
	GPUResource(const ComPtr<ID3D12Resource>& Buff, const D3D12_RESOURCE_STATES& Barrier,const wchar_t* Name = nullptr) 
	{
		m_buff = Buff;
		if (Name != nullptr)m_buff.Get()->SetName(Name);

		m_barrier = Barrier;
	}
	~GPUResource()
	{
		if (m_mapped)m_buff->Unmap(0, nullptr);
	}

	//�o�b�t�@���̃Z�b�^
	void SetName(const wchar_t* Name)
	{
		m_buff->SetName(Name);
	}

	//�o�b�t�@�擾
	const ComPtr<ID3D12Resource>& GetBuff() { return m_buff; }
	//���݂̃��\�[�X�o���A�擾
	const D3D12_RESOURCE_STATES& GetResourceBarrier() { return m_barrier; }

	//�}�b�s���O
	void Mapping(const size_t& DataSize, const int& ElementNum, const void* SendData);
	//���\�[�X�o���A�̕ύX
	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& NewBarrier);
	//�o�b�t�@�̃R�s�[�i�C���X�^���X�͕ʕ��A�����ɃR�s�[���j
	void CopyGPUResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& CopySource);

	template<typename T>
	T* GetBuffOnCpu() { return (T*) m_buffOnCPU; }
};

//�����_�����O�̍ۂɃZ�b�g����o�b�t�@�i�f�B�X�N���v�^�o�^�̕K�v������^�C�v�̃f�[�^�j
class DescriptorData
{
	DescriptorData() = delete;
	DescriptorData(const DescriptorData& tmp) = delete;
	DescriptorData(DescriptorData&& tmp) = delete;
protected:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::shared_ptr<GPUResource> m_resource;	//GPU�o�b�t�@
	DescHandlesContainer m_handles;	//�f�B�X�N���v�^�n���h��

	//�o�b�t�@�Z�b�g�̃^�C�~���O�ŌĂ΂��֐��A���\�[�X�o���A��ς���Ȃ�
	virtual void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type) {};

public:
	DescriptorData(const ComPtr<ID3D12Resource>& Buff, const D3D12_RESOURCE_STATES& Barrier) :m_resource(std::make_shared<GPUResource>(Buff, Barrier)) {}
	DescriptorData(const std::shared_ptr<GPUResource>& GPUResource) :m_resource(GPUResource) {}	//�������̂�����
	virtual ~DescriptorData() {}
	//�o�b�t�@�Z�b�g
	void SetGraphicsDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam, D3D12_RESOURCE_STATES* Barrier = nullptr);
	void SetComputeDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam, D3D12_RESOURCE_STATES* Barrier = nullptr);
	std::shared_ptr<GPUResource>& GetResource() { return m_resource; }

	void InitDescHandle(const DESC_HANDLE_TYPE& Type, const DescHandles& Handle)
	{
		m_handles.Initialize(Type, Handle);
	}

	const DescHandlesContainer& GetDescHandlesContainer()const { return m_handles; }
};

//�Z�b�g����f�B�X�N���v�^�o�b�t�@�̏��
struct RegisterDescriptorData
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	std::shared_ptr<DescriptorData>m_descData;
	DESC_HANDLE_TYPE m_descHandleType;
	D3D12_RESOURCE_STATES* m_barrier = nullptr;

	RegisterDescriptorData() = delete;
	RegisterDescriptorData(const RegisterDescriptorData& rhs) : m_descData(rhs.m_descData), m_descHandleType(rhs.m_descHandleType)
	{
		if (rhs.m_barrier)
		{
			m_barrier = new D3D12_RESOURCE_STATES(*rhs.m_barrier);
		}
	}
	RegisterDescriptorData(RegisterDescriptorData&& rhs) : m_descData(rhs.m_descData), m_descHandleType(rhs.m_descHandleType)
	{
		if (rhs.m_barrier)
		{
			m_barrier = new D3D12_RESOURCE_STATES(*rhs.m_barrier);
		}
	}

	RegisterDescriptorData(const std::shared_ptr<DescriptorData>& DescData, const DESC_HANDLE_TYPE& Type)
		:m_descData(DescData), m_descHandleType(Type) {}
	RegisterDescriptorData(const std::shared_ptr<DescriptorData>& DescData, const DESC_HANDLE_TYPE& Type, const D3D12_RESOURCE_STATES& Barrier)
		:m_descData(DescData), m_descHandleType(Type) 
	{
		m_barrier = new D3D12_RESOURCE_STATES(Barrier);
	}

	~RegisterDescriptorData()
	{
		delete m_barrier;
		m_barrier = nullptr;
	}

	void SetAsCompute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const int& RootParam)
	{
		m_descData->SetComputeDescriptorBuffer(CmdList, m_descHandleType, RootParam, m_barrier);
	}
	void SetAsGraphics(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const int& RootParam)
	{
		m_descData->SetGraphicsDescriptorBuffer(CmdList, m_descHandleType, RootParam, m_barrier);
	}
};

//�萔�o�b�t�@
class ConstantBuffer : public DescriptorData
{
	ConstantBuffer() = delete;
	ConstantBuffer(const ConstantBuffer& tmp) = delete;
	ConstantBuffer(ConstantBuffer&& tmp) = delete;
private:
	const size_t m_dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int m_elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

public:
	ConstantBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& CBVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), m_dataSize(DataSize), m_elementNum(ElementNum) 
	{
		m_handles.Initialize(CBV, CBVHandles);
	}
	void Mapping(const void* SendData)
	{
		m_resource->Mapping(m_dataSize, m_elementNum, SendData);
	}
};

//�\�����o�b�t�@�i��������͒萔�o�b�t�@�Ɠ����j
class StructuredBuffer : public DescriptorData
{
	StructuredBuffer() = delete;
	StructuredBuffer(const StructuredBuffer& tmp) = delete;
	StructuredBuffer(StructuredBuffer&& tmp) = delete;

private:
	const size_t m_dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int m_elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
public:
	StructuredBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& SRVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), m_dataSize(DataSize), m_elementNum(ElementNum) 
	{
		m_handles.Initialize(SRV, SRVHandles);
	}
	void Mapping(void* SendData)
	{
		m_resource->Mapping(m_dataSize, m_elementNum, SendData);
	}
};

//�o�͐�o�b�t�@
class RWStructuredBuffer : public DescriptorData
{
	RWStructuredBuffer() = delete;
	RWStructuredBuffer(const RWStructuredBuffer& tmp) = delete;
	RWStructuredBuffer(RWStructuredBuffer&& tmp) = delete;

private:
	const size_t m_dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int m_elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
public:
	RWStructuredBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& UAVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), m_dataSize(DataSize), m_elementNum(ElementNum) 
	{
		m_handles.Initialize(UAV, UAVHandles);
	}
	RWStructuredBuffer(const std::shared_ptr<GPUResource>& GPUResource, const DescHandles& UAVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(GPUResource), m_dataSize(DataSize), m_elementNum(ElementNum)
	{
		m_handles.Initialize(UAV, UAVHandles);
	}

	void CopyBuffOnGPU(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& Dest) { Dest.CopyGPUResource(CmdList, *this->m_resource); }

	void Mapping(void* SendData)
	{
		m_resource->Mapping(m_dataSize, m_elementNum, SendData);
	}
};

//�e�N�X�`�����\�[�X���N���X
class TextureBuffer : public DescriptorData
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	TextureBuffer() = delete;
	TextureBuffer(const TextureBuffer& tmp) = delete;
	TextureBuffer(TextureBuffer&& tmp) = delete;
	std::string m_name;
protected:
	CD3DX12_RESOURCE_DESC m_texDesc;	//�e�N�X�`�����i���A�����Ȃǁj

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		if (Type == SRV)
		{
			m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		else if (Type == UAV)
		{
			m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		}
	}

public:
	TextureBuffer(const ComPtr<ID3D12Resource>& Buff, const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& SRVHandles, const CD3DX12_RESOURCE_DESC& Desc, const std::string& Name)
		:DescriptorData(Buff, Barrier), m_name(Name), m_texDesc(Desc)
	{
		m_handles.Initialize(SRV, SRVHandles);
	}
	TextureBuffer(const ComPtr<ID3D12Resource>& Buff, const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& SRVHandles, const D3D12_RESOURCE_DESC& Desc, const std::string& Name)
		:DescriptorData(Buff, Barrier), m_name(Name), m_texDesc(CD3DX12_RESOURCE_DESC(Desc))
	{
		m_handles.Initialize(SRV, SRVHandles);
	}
	TextureBuffer(const std::shared_ptr<GPUResource>& GPUResource, const DescHandles& SRVHandles,
		const D3D12_RESOURCE_DESC& Desc, const std::string& Name)
		: DescriptorData(GPUResource), m_name(Name), m_texDesc(CD3DX12_RESOURCE_DESC(Desc))
	{
		m_handles.Initialize(SRV, SRVHandles);
	}

	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& Barrier) { m_resource->ChangeBarrier(CmdList, Barrier); }
	void CopyTexResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, TextureBuffer* CopySource);
	const CD3DX12_RESOURCE_DESC& GetDesc() { return m_texDesc; }
	void SetUAVHandles(const DescHandles& UAVHandles) { m_handles.Initialize(UAV, UAVHandles); }
	Vec2<int>GetGraphSize()
	{
		return Vec2<int>(static_cast<int>(m_texDesc.Width), static_cast<int>(m_texDesc.Height));
	}
};

//�����_�[�^�[�Q�b�g
class RenderTarget : public TextureBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	RenderTarget() = delete;
	RenderTarget(const RenderTarget& tmp) = delete;
	RenderTarget(RenderTarget&& tmp) = delete;
private:
	float m_clearValue[4] = { 0.0f };	//�N���A�l
	//�s�N�Z���V�F�[�_�[���\�[�X�Ƃ��Ďg����
	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
public:
	RenderTarget(const ComPtr<ID3D12Resource1>& Buff,
		const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& SRVHandles,
		const DescHandles& RTVHandles,
		const CD3DX12_RESOURCE_DESC& Desc,
		const Color& ClearValue = Color(0.0f, 0.0f, 0.0f, 0.0f),
		const std::string& Name = "")
		:TextureBuffer(Buff, Barrier, SRVHandles, Desc, Name)
	{
		m_handles.Initialize(RTV, RTVHandles);
		m_clearValue[0] = ClearValue.m_r;
		m_clearValue[1] = ClearValue.m_g;
		m_clearValue[2] = ClearValue.m_b;
		m_clearValue[3] = ClearValue.m_a;
	}

	RenderTarget(const std::shared_ptr<GPUResource>& GPUResource,
		const DescHandles& SRVHandles,
		const DescHandles& RTVHandles,
		const CD3DX12_RESOURCE_DESC& Desc,
		const Color& ClearValue = Color(0.0f, 0.0f, 0.0f, 0.0f),
		const std::string& Name = "")
		:TextureBuffer(GPUResource, SRVHandles, Desc, Name)
	{
		m_handles.Initialize(RTV, RTVHandles);
		m_clearValue[0] = ClearValue.m_r;
		m_clearValue[1] = ClearValue.m_g;
		m_clearValue[2] = ClearValue.m_b;
		m_clearValue[3] = ClearValue.m_a;
	}


	//�����_�[�^�[�Q�b�g���N���A
	void Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//���\�[�X�o���A�ύX
	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& Barrier) { m_resource->ChangeBarrier(CmdList, Barrier); }

	//�����_�[�^�[�Q�b�g�Ƃ��ăZ�b�g���鏀��
	const DescHandles& AsRTV(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		return m_handles.GetHandle(RTV);
	}
};

//�f�v�X�X�e���V��
class DepthStencil
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	DepthStencil() = delete;
	DepthStencil(const DepthStencil& tmp) = delete;
	DepthStencil(DepthStencil&& tmp) = delete;
private:
	const CD3DX12_RESOURCE_DESC m_desc;
	float m_clearValue = 1.0f;

	GPUResource m_resource;
	DescHandlesContainer m_handles;	//�f�B�X�N���v�^�n���h��

public:
	DepthStencil(const ComPtr<ID3D12Resource1>& Buff,
		const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& DSVHandles,
		const CD3DX12_RESOURCE_DESC& Desc,
		const float& ClearValue = 1.0f)
		:m_resource(Buff, Barrier), m_desc(Desc), m_clearValue(ClearValue)
	{
		m_handles.Initialize(DSV, DSVHandles);
	}

	//�f�v�X�X�e���V�����N���A
	void Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//DSV�擾
	const DescHandles& AsDSV(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		m_resource.ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		return m_handles.GetHandle(DSV);
	}
};

//���_�o�b�t�@
class VertexBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	VertexBuffer() = delete;
	VertexBuffer(const VertexBuffer& tmp) = delete;
	VertexBuffer(VertexBuffer&& tmp) = delete;

	//���_�o�b�t�@
	std::shared_ptr<GPUResource> m_resource;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	//�ǂݎ���p
	std::shared_ptr<RWStructuredBuffer>m_rwBuff;
public:
	//���_�T�C�Y
	const size_t m_vertexSize;
	//�����������_��
	const unsigned int m_vertexNum;
	//���M���钸�_��
	unsigned int m_sendVertexNum;

	VertexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_VERTEX_BUFFER_VIEW& VBView)
		:m_vbView(VBView), m_vertexSize(VBView.StrideInBytes), m_vertexNum(VBView.SizeInBytes / VBView.StrideInBytes), m_sendVertexNum(m_vertexNum) 
	{
		m_resource = std::make_shared<GPUResource>(Buff, Barrier);
	}
	VertexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_VERTEX_BUFFER_VIEW& VBView, const DescHandles& UAVHandle)
		:m_vbView(VBView), m_vertexSize(VBView.StrideInBytes), m_vertexNum(VBView.SizeInBytes / VBView.StrideInBytes), m_sendVertexNum(m_vertexNum)
	{
		m_resource = std::make_shared<GPUResource>(Buff, Barrier);
		m_rwBuff = std::make_shared<RWStructuredBuffer>(m_resource, UAVHandle, m_vertexSize, m_sendVertexNum);
	}
	void Mapping(void* SendData)
	{
		m_resource->Mapping(m_vertexSize, m_sendVertexNum, SendData);
	}
	void SetName(const wchar_t* Name)
	{
		m_resource->SetName(Name);
	}

	//�ǂݎ���p�\�����o�b�t�@�擾
	std::weak_ptr<RWStructuredBuffer>GetRWStructuredBuff()
	{
		assert(m_rwBuff);
		return m_rwBuff;
	}
	//���_�o�b�t�@�Ƃ��Ďg�����߂Ƀ��\�[�X�o���A�ύX
	void ChangeBarrierForVertexBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
	void SetView(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		CmdList->IASetVertexBuffers(0, 1, &m_vbView);
	}
};

//�C���f�b�N�X�o�b�t�@
class IndexBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IndexBuffer() = delete;
	IndexBuffer(const IndexBuffer& tmp) = delete;
	IndexBuffer(IndexBuffer&& tmp) = delete;

	//�C���f�b�N�X�o�b�t�@
	GPUResource m_resource;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};

public:
	//�C���f�b�N�X�T�C�Y
	const size_t m_indexSize;
	//�C���f�b�N�X��
	const unsigned int m_indexNum;

	IndexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_INDEX_BUFFER_VIEW& IBView, const size_t& IndexSize)
		:m_resource(Buff, Barrier), m_ibView(IBView), m_indexSize(IndexSize), m_indexNum(static_cast<unsigned int>(IBView.SizeInBytes / IndexSize)) {}
	void Mapping(void* SendData)
	{
		m_resource.Mapping(m_indexSize, m_indexNum, SendData);
	}
	void SetName(const wchar_t* Name)
	{
		m_resource.SetName(Name);
	}
	void SetView(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		CmdList->IASetIndexBuffer(&m_ibView);
	}
};

//�V�F�[�_�[���
class Shaders
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	ComPtr<IDxcBlob>m_vs;	//���_�V�F�[�_�[
	ComPtr<IDxcBlob>m_ps;	//�s�N�Z���V�F�[�_�[
	ComPtr<IDxcBlob>m_ds;	//�h���C���V�F�[�_�[
	ComPtr<IDxcBlob>m_hs;	//�n���V�F�[�_�[
	ComPtr<IDxcBlob>m_gs;	//�W�I���g���V�F�[�_�[
};

//���_���C�A�E�g�p�����[�^
class InputLayoutParam
{
	InputLayoutParam() = delete;
public:
	const std::string m_semantics;
	const DXGI_FORMAT m_format;
	const int m_semanticIdx = 0;
	InputLayoutParam(const std::string& Semantics,
		const DXGI_FORMAT& Format,
		const int& SemanticIdx = 0
	) :m_semantics(Semantics), m_format(Format), m_semanticIdx(SemanticIdx) {}
};


#define D3D12_DESCRIPTOR_RANGE_TYPE_NUM D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER+1
//���[�g�p�����[�^(SRV,UAV,CBV,SAMPLER ��GPU�f�[�^�w��j
class RootParam
{
private:
	RootParam() = delete;

public:
	std::string m_comment;		//���߁@�����Ă��Ȃ��Ă��ǂ�
	D3D12_DESCRIPTOR_RANGE_TYPE m_descriptorRangeType;
	DESC_HANDLE_TYPE m_viewType;
	bool m_descriptor = false;	//�f�B�X�N���v�^�Ƃ��ēo�^����Ă��邩
	int m_descNum = 1;

	RootParam(const D3D12_DESCRIPTOR_RANGE_TYPE& Range, const char* Comment = nullptr, const int& DescNum = 1)
		:m_descriptorRangeType(Range), m_descriptor(true), m_descNum(DescNum) {
		if (Comment != nullptr)m_comment = Comment;
		switch (Range)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
			m_viewType = CBV;
			break;
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
			m_viewType = SRV;
			break;
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
			m_viewType = UAV;
			break;
		default:
			break;
		}
	}
	RootParam(const DESC_HANDLE_TYPE& ViewType, const char* Comment = nullptr)
		:m_viewType(ViewType) {
		if (Comment != nullptr)m_comment = Comment;
		switch (ViewType)
		{
		case CBV:
			m_descriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;
		case SRV:
			m_descriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			break;
		case UAV:
			m_descriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			break;
		default:
			assert(0);
			break;
		}
	}
};

//�T���v���[
class WrappedSampler
{
	void Generate(const D3D12_TEXTURE_ADDRESS_MODE& TexAddressMode, const D3D12_FILTER& Filter)
	{
		m_sampler.AddressU = TexAddressMode;
		m_sampler.AddressV = TexAddressMode;
		m_sampler.AddressW = TexAddressMode;
		m_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		m_sampler.Filter = Filter;	//���
		m_sampler.MaxLOD = D3D12_FLOAT32_MAX;	//�~�b�v�}�b�v�ő�l
		m_sampler.MinLOD = 0.0f;	//�~�b�v�}�b�v�ŏ��l
		m_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		m_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		m_sampler.ShaderRegister = 0;
	}
public:
	D3D12_STATIC_SAMPLER_DESC m_sampler = {};
	WrappedSampler(const D3D12_TEXTURE_ADDRESS_MODE& TexAddressMode, const D3D12_FILTER& Filter)
	{
		Generate(TexAddressMode, Filter);
	}
	//�J��Ԃ����A��Ԃ������邩
	WrappedSampler(const bool& Wrap,const bool& Interpolation)
	{
		auto addressMode = !Wrap ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		auto interpolation = Interpolation ? D3D12_FILTER_MIN_MAG_MIP_LINEAR : D3D12_FILTER_MIN_MAG_MIP_POINT;
		Generate(addressMode, interpolation);
	}
	operator D3D12_STATIC_SAMPLER_DESC() { return m_sampler; }
};

//�p�C�v���C���e��ݒ�
class PipelineInitializeOption
{
	PipelineInitializeOption() = delete;
public:
	D3D12_CULL_MODE m_calling = D3D12_CULL_MODE_BACK;	//�J�����O
	D3D12_FILL_MODE m_fillMode = D3D12_FILL_MODE_SOLID;	//���C���[�t���[��
	bool m_depthTest = true;	//�[�x�e�X�g
	DXGI_FORMAT m_dsvFormat = DXGI_FORMAT_D32_FLOAT;	//�f�v�X�X�e���V���̃t�H�[�}�b�g
	bool m_depthWriteMask = true;	//�f�v�X�̏������݁i�[�x�e�X�g���s���ꍇ�j
	bool m_independetBlendEnable = true;		//���������_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ��邩
	bool m_frontCounterClockWise = false;	//�O�p�`�̕\���ǂ��炩���߂�ۂ̌���


	D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;	//�}�`�̌`��ݒ�

	//�p�C�v���C���̖��O�ŋ�ʂ���̂ŁA�K����������
	PipelineInitializeOption(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& TopologyType, const D3D_PRIMITIVE_TOPOLOGY& Topology)
		:m_primitiveTopologyType(TopologyType), m_primitiveTopology(Topology) {}
};

//�A���t�@�u�����f�B���O���[�h
enum AlphaBlendMode
{
	AlphaBlendMode_None,	//�A���t�@�u�����f�B���O�Ȃ�(�㏑��)�B
	AlphaBlendMode_Trans,	//����������
	AlphaBlendMode_Add,		//���Z����
	AlphaBlendModeNum
};

//�������ݐ惌���_�[�^�[�Q�b�g���
class RenderTargetInfo
{
	RenderTargetInfo() = delete;
public:
	DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
	AlphaBlendMode m_blendMode = AlphaBlendMode_None;
	RenderTargetInfo(const DXGI_FORMAT& Format, const AlphaBlendMode& BlendMode) :m_format(Format), m_blendMode(BlendMode) {}
};

//�O���t�B�b�N�X�p�C�v���C�����
class GraphicsPipeline
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	static int s_pipelineNum;	//�p�C�v���C�����������ꂽ���i�����ʔԍ��j

private:
	int	m_handle = -1;	//�p�C�v���C�����ʔԍ�

	ComPtr<ID3D12PipelineState>m_pipeline;			//�p�C�v���C��
	ComPtr<ID3D12RootSignature>m_rootSignature;	//���[�g�V�O�l�`��
	D3D_PRIMITIVE_TOPOLOGY m_topology;
public:
	GraphicsPipeline(const ComPtr<ID3D12PipelineState>& Pipeline, const ComPtr<ID3D12RootSignature>& RootSignature, const D3D_PRIMITIVE_TOPOLOGY& Topology)
		:m_pipeline(Pipeline), m_rootSignature(RootSignature), m_topology(Topology), m_handle(s_pipelineNum++) {}

	void SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	const int& GetPipelineHandle() { return m_handle; }
};

//�R���s���[�g�p�C�v���C��
class ComputePipeline
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	static int s_pipelineNum;	//�p�C�v���C�����������ꂽ���i�����ʔԍ��j

private:
	int	m_handle = -1;	//�p�C�v���C�����ʔԍ�

	ComPtr<ID3D12PipelineState>m_pipeline;			//�p�C�v���C��
	ComPtr<ID3D12RootSignature>m_rootSignature;	//���[�g�V�O�l�`��
public:
	ComputePipeline(const ComPtr<ID3D12PipelineState>& Pipeline, const ComPtr<ID3D12RootSignature>& RootSignature)
		:m_pipeline(Pipeline), m_rootSignature(RootSignature), m_handle(s_pipelineNum++) {}

	void SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	const int& GetPipelineHandle() { return m_handle; }
};

template<int GpuAddressNum>
class IndirectDrawCommand
{
public:
	//�e�R�}���h�ł̕`��Ŏg�p����o�b�t�@
	std::array<D3D12_GPU_VIRTUAL_ADDRESS, GpuAddressNum>m_gpuAddressArray;
	//DrawInstanced �̈����ɂ�����p�����[�^
	D3D12_DRAW_ARGUMENTS m_drawArgs;
};

template<int GpuAddressNum>
class IndirectDrawIndexedCommand
{
public:
	//�e�R�}���h�ł̕`��Ŏg�p����o�b�t�@
	std::array<D3D12_GPU_VIRTUAL_ADDRESS, GpuAddressNum>m_gpuAddressArray;
	//DrawIndexedInstanced �̈����ɂ�����p�����[�^
	D3D12_DRAW_INDEXED_ARGUMENTS m_drawArgs;
};

template<int GpuAddressNum>
class IndirectDispatchCommand
{
public:
	//�e�R�}���h�ł̕`��Ŏg�p����o�b�t�@
	std::array<D3D12_GPU_VIRTUAL_ADDRESS, GpuAddressNum>m_gpuAddressArray;
	//Dispatch �̈����ɂ�����p�����[�^
	D3D12_DISPATCH_ARGUMENTS m_dispatchArgs;
};

enum EXCUTE_INDIRECT_TYPE { DRAW, DRAW_INDEXED, DISPATCH, EXCUTE_INDIRECT_TYPE_NUM, NONE };
class IndirectCommandBuffer
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	friend class IndirectDevice;

private:
	//Indirect�̌`��
	EXCUTE_INDIRECT_TYPE m_indirectType = EXCUTE_INDIRECT_TYPE::NONE;
	//�R�}���h�̍ő吔
	int m_maxCommandCount = 0;
	//�R�}���h�o�b�t�@
	std::shared_ptr<DescriptorData>m_buff;
	//�J�E���^�[�o�b�t�@
	std::shared_ptr<GPUResource>m_counterBuffer;
	//�R�}���h�P������ɃT�C�Y
	size_t m_commandSize;
public:
	IndirectCommandBuffer(EXCUTE_INDIRECT_TYPE IndirectType, int MaxCommandCount, size_t CommandSize,
		std::shared_ptr<DescriptorData>Buff, std::shared_ptr<GPUResource>CounterBuffer = nullptr)
		:m_indirectType(IndirectType), m_maxCommandCount(MaxCommandCount), m_commandSize(CommandSize), m_buff(Buff), m_counterBuffer(CounterBuffer)
	{
		assert(DRAW <= IndirectType && IndirectType < EXCUTE_INDIRECT_TYPE_NUM);
	}
	void ResetCounterBuffer();

	//�Q�b�^
	const EXCUTE_INDIRECT_TYPE& IndirectType() { return m_indirectType; }
	const std::shared_ptr<DescriptorData>& GetBuff()
	{
		return m_buff;
	}
	const size_t& GetCommandSize() { return m_commandSize; }
};


class IndirectDevice
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//Indirect�̌`��
	EXCUTE_INDIRECT_TYPE m_indirectType = NONE;
	//�R�}���h�V�O�l�`��
	ComPtr<ID3D12CommandSignature>m_cmdSignature;

public:
	IndirectDevice(EXCUTE_INDIRECT_TYPE IndirectType,const ComPtr<ID3D12CommandSignature>& CmdSignature) 
		:m_indirectType(IndirectType), m_cmdSignature(CmdSignature) 
	{
		assert(DRAW <= IndirectType && IndirectType < EXCUTE_INDIRECT_TYPE_NUM);
	}

	void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, std::shared_ptr<IndirectCommandBuffer>CmdBuff, UINT ArgBufferOffset = 0);

	const EXCUTE_INDIRECT_TYPE& IndirectType() { return m_indirectType; }
};