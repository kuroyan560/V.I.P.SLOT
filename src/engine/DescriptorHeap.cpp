#include "DescriptorHeap.h"

DescriptorHeapBase::DescriptorHeapBase(ComPtr<ID3D12Device>Device, const D3D12_DESCRIPTOR_HEAP_TYPE& Type, const int& MaxDescriptorNum)
{
    HRESULT result;

    D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || Type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
    {
        flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    }

    // SRV_CBV_UAV �̃f�B�X�N���v�^�q�[�v
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{
      Type,
      MaxDescriptorNum,
      flag,
      0
    };
    result = Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
    m_headHandleCpu = m_heap.Get()->GetCPUDescriptorHandleForHeapStart();
  /*  if(flag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)*/m_headHandleGpu = m_heap.Get()->GetGPUDescriptorHandleForHeapStart();
    m_incrementSize = Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
}

void DescriptorHeap_CBV_SRV_UAV::CreateCBV(const ComPtr<ID3D12Device>& Device, const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc)
{
    Device->CreateConstantBufferView(&Desc, GetCpuHandleEnd());
    OnCreateView();
}

void DescriptorHeap_CBV_SRV_UAV::CreateSRV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc)
{
    //�q�[�v�ɃV�F�[�_���\�[�X�r���[�쐬
    Device->CreateShaderResourceView(
        Buff.Get(),	//�r���[�Ɗ֘A�t����o�b�t�@
        &Desc,				//�e�N�X�`���ݒ���
        GetCpuHandleEnd());
    OnCreateView();
}

void DescriptorHeap_CBV_SRV_UAV::CreateUAV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, const ComPtr<ID3D12Resource>& CounterBuff)
{
    //�q�[�v�ɃA���I�[�_�[�h�A�N�Z�X�r���[�쐬
    Device->CreateUnorderedAccessView(
        Buff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
        CounterBuff ? CounterBuff.Get() : nullptr,
        &Desc,  //�f�[�^�ݒ���
        GetCpuHandleEnd());
    OnCreateView();
}

void DescriptorHeap_RTV::CreateRTV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_RENDER_TARGET_VIEW_DESC* ViewDesc)
{
    //�q�[�v�Ƀ����_�[�^�[�Q�b�g�r���[�쐬
    Device->CreateRenderTargetView(
        Buff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
        ViewDesc,
        GetCpuHandleEnd());
    OnCreateView();
}

void DescriptorHeap_DSV::CreateDSV(const ComPtr<ID3D12Device>& Device, const ComPtr<ID3D12Resource>& Buff, const D3D12_DEPTH_STENCIL_VIEW_DESC* ViewDesc)
{
    //�q�[�v�Ƀf�v�X�X�e���V���r���[�쐬
    Device->CreateDepthStencilView(
        Buff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
        ViewDesc,
        GetCpuHandleEnd());
    OnCreateView();
}