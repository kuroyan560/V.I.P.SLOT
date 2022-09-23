#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"d3dx12.h"
#include <cassert>
//�f�B�X�N���v�^�q�[�v���N���X
class DescriptorHeapBase
{
protected:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	static const int s_maxSrvCbvUav = 2048;
	static const int s_maxSampler = 100;
	static const int s_maxRtv = 100;
	static const int s_maxDsv = 100;

protected:
	DescriptorHeapBase(ComPtr<ID3D12Device> Device, const D3D12_DESCRIPTOR_HEAP_TYPE& Type, const int& MaxDescriptorNum);

	ComPtr<ID3D12DescriptorHeap>m_heap;	//�f�B�X�N���v�^�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE m_headHandleCpu;	//�q�[�v�擪��CPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE m_headHandleGpu;	//�q�[�v�擪��GPU�n���h��
	UINT m_incrementSize;	//�P�̃f�B�X�N���v�^�̃T�C�Y�i�q�[�v�^�C�v�Ɉˑ��j

	int m_descriptorNum = 0;	//�����σf�B�X�N���v�^�̐�
	void OnCreateView() { m_descriptorNum++; }

public:
	//�f�B�X�N���v�^�q�[�v�Q�b�^
	const ComPtr<ID3D12DescriptorHeap>&GetHeap() { return m_heap; }
	//GPU�n���h���Q�b�^
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(const int& Index)	//�ꏊ�w��
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_headHandleGpu, Index, m_incrementSize);
	}
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandleTail()		//�Ō�ɐ������ꂽ�f�B�X�N���v�^�n���h��
	{
		if (m_descriptorNum == 0)
		{
			printf("�܂��f�B�X�N���v�^�n���h������������Ă��܂���\n");
			assert(0);
		}
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_headHandleGpu, m_descriptorNum - 1, m_incrementSize);
	}
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandleEnd()		//�Ō��
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_headHandleGpu, m_descriptorNum , m_incrementSize);
	}

	//CPU�n���h���Q�b�^
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const int& Index)	//�ꏊ�w��
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_headHandleCpu, Index, m_incrementSize);
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandleTail()		//�Ō�ɐ������ꂽ�f�B�X�N���v�^�n���h��
	{
		if (m_descriptorNum == 0)
		{
			printf("�܂��f�B�X�N���v�^�n���h������������Ă��܂���\n");
			assert(0);
		}
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_headHandleCpu, m_descriptorNum - 1, m_incrementSize);
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandleEnd()	//�Ō��
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_headHandleCpu, m_descriptorNum, m_incrementSize);
	}
};

//D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
class DescriptorHeap_CBV_SRV_UAV : public DescriptorHeapBase
{
	static const int MAX_DESCRIPTOR_NUM = 2048;
public:
	DescriptorHeap_CBV_SRV_UAV(ComPtr<ID3D12Device> Device)
		:DescriptorHeapBase(Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_DESCRIPTOR_NUM) {}

	//�萔�o�b�t�@�r���[����
	void CreateCBV(const ComPtr<ID3D12Device>& Device, const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc);
	void CreateCBV(const ComPtr<ID3D12Device>& Device, const D3D12_GPU_VIRTUAL_ADDRESS& BuffGpuAddress, const size_t& SizeInBytes)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = BuffGpuAddress;
		desc.SizeInBytes = static_cast<UINT>(SizeInBytes);
		CreateCBV(Device, desc);
	}

	//�V�F�[�_�[���\�[�X�r���[����
	void CreateSRV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc);
	//�e�N�X�`���o�b�t�@
	void CreateSRV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const DXGI_FORMAT& Format)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
		srvDesc.Format = Format;	//RGBA
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;
		CreateSRV(Device, Buff, srvDesc);
	}
	//�\�����o�b�t�@
	void CreateSRV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const size_t& DataSize, const int& ElementNum)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = static_cast<UINT>(ElementNum);
		srvDesc.Buffer.StructureByteStride = static_cast<UINT>(DataSize);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		CreateSRV(Device, Buff, srvDesc);
	}

	//�A���I�[�_�[�h�A�N�Z�X�r���[����
	void CreateUAV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, const ComPtr<ID3D12Resource>& CounterBuff = nullptr);
	void CreateUAV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const size_t& DataSize, const int& ElementNum,
		const D3D12_UAV_DIMENSION& Dimension = D3D12_UAV_DIMENSION_BUFFER, const DXGI_FORMAT& Format = DXGI_FORMAT_UNKNOWN)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
		desc.ViewDimension = Dimension;
		desc.Format = Format;
		desc.Buffer.NumElements = static_cast<UINT>(ElementNum);
		desc.Buffer.StructureByteStride = static_cast<UINT>(DataSize);
		CreateUAV(Device, Buff, desc, nullptr);
	}
};

//D3D12_DESCRIPTOR_HEAP_TYPE_RTV
class DescriptorHeap_RTV : public DescriptorHeapBase
{
	static const int MAX_DESCRIPTOR_NUM = 2048;
public:
	DescriptorHeap_RTV(ComPtr<ID3D12Device> Device)
		:DescriptorHeapBase(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_DESCRIPTOR_NUM) {}

	void CreateRTV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_RENDER_TARGET_VIEW_DESC* ViewDesc = nullptr);
};

//D3D12_DESCRIPTOR_HEAP_TYPE_DSV
class DescriptorHeap_DSV : public DescriptorHeapBase
{
	static const int MAX_DESCRIPTOR_NUM = 100;
public:
	DescriptorHeap_DSV(ComPtr<ID3D12Device> Device)
		:DescriptorHeapBase(Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MAX_DESCRIPTOR_NUM) {}

	void CreateDSV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_DEPTH_STENCIL_VIEW_DESC* ViewDesc = nullptr);
};