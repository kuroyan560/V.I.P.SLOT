#include"D3D12Data.h"

void GPUResource::Mapping(const size_t& DataSize, const int& ElementNum, const void* SendData)
{
	//����f�[�^��null�Ȃ牽�����Ȃ�
	assert(SendData);

	//�܂��}�b�s���O���Ă��Ȃ���΃}�b�s���O
	if (!m_mapped)
	{
		//�}�b�v�A�A���}�b�v�̃I�[�o�[�w�b�h���y�����邽�߂ɂ͂��̃C���X�^���X�������Ă���Ԃ�Unmap���Ȃ�
		auto hr = m_buff->Map(0, nullptr, (void**)&m_buffOnCPU);
		assert(SUCCEEDED(hr));
		m_mapped = true;
	}

	memcpy(m_buffOnCPU, SendData, DataSize * ElementNum);
}

void GPUResource::ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& NewBarrier)
{
    //���\�[�X�o���A�ω��Ȃ�
    if (m_barrier == NewBarrier)return;

    //���\�[�X�o���A�ύX
	auto changeBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_buff.Get(),
		m_barrier,
		NewBarrier);

    CmdList->ResourceBarrier(
        1,
        &changeBarrier);

    //���\�[�X�o���A��ԋL�^
    m_barrier = NewBarrier;
}

void GPUResource::CopyGPUResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& CopySource)
{
	//�R�s�[���̃��\�[�X�̏�Ԃ��L�^
	auto oldBarrierCopySource = CopySource.m_barrier;
	//�R�s�[���̃��\�[�X�o���A���R�s�[���p�ɕύX
	CopySource.ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//�R�s�[��ł��鎩�g�̃��\�[�X�̏�Ԃ��L�^
	auto oldBarrierThis = m_barrier;
	//�R�s�[��ł��鎩�g�̃��\�[�X�o���A���R�s�[��p�ɕύX
	this->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_COPY_DEST);

	//�R�s�[���s
	CmdList->CopyResource(this->m_buff.Get(), CopySource.m_buff.Get());

	//�R�s�[�������̃��\�[�X�o���A�ɖ߂�
	CopySource.ChangeBarrier(CmdList, oldBarrierCopySource);

	//�R�s�[��ł��鎩�g�����̃��\�[�X�o���A�ɖ߂�
	this->ChangeBarrier(CmdList, oldBarrierThis);
}

void DescriptorData::SetGraphicsDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam, D3D12_RESOURCE_STATES* Barrier)
{
	if (Barrier != nullptr)
	{
		m_resource->ChangeBarrier(CmdList, *Barrier);
	}
	else OnSetDescriptorBuffer(CmdList, Type);
	CmdList->SetGraphicsRootDescriptorTable(RootParam, m_handles.GetHandle(Type));
}

void DescriptorData::SetComputeDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam, D3D12_RESOURCE_STATES* Barrier)
{
	if (Barrier != nullptr)
	{
		m_resource->ChangeBarrier(CmdList, *Barrier);
	}
	else OnSetDescriptorBuffer(CmdList, Type);
	CmdList->SetComputeRootDescriptorTable(RootParam, m_handles.GetHandle(Type));
}

void TextureBuffer::CopyTexResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, TextureBuffer* CopySource)
{
	m_resource->CopyGPUResource(CmdList, *CopySource->m_resource);
}

void RenderTarget::Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	m_resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CmdList->ClearRenderTargetView(
		m_handles.GetHandle(RTV),
		&m_clearValue[0],
		0,
		nullptr);
}

void DepthStencil::Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	CmdList->ClearDepthStencilView(
		m_handles.GetHandle(DSV),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		m_clearValue,
		0,
		0,
		nullptr);
}

int GraphicsPipeline::s_pipelineNum = 0;

void GraphicsPipeline::SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	CmdList->SetPipelineState(m_pipeline.Get());
	//���[�g�V�O�l�`���̐ݒ�
	CmdList->SetGraphicsRootSignature(m_rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	CmdList->IASetPrimitiveTopology(m_topology);
}

int ComputePipeline::s_pipelineNum = 0;

void ComputePipeline::SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	CmdList->SetPipelineState(m_pipeline.Get());
	//���[�g�V�O�l�`���̐ݒ�
	CmdList->SetComputeRootSignature(m_rootSignature.Get());
}

void IndirectCommandBuffer::ResetCounterBuffer()
{
	assert(m_counterBuffer);
	UINT num = 0;
	m_counterBuffer->Mapping(sizeof(UINT), 1, &num);
}

void IndirectDevice::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, std::shared_ptr<IndirectCommandBuffer> CmdBuff, UINT ArgBufferOffset)
{
	//�R�}���h�V�O�l�`���ƃo�b�t�@�� Indirect �`������v
	assert(m_indirectType == CmdBuff->m_indirectType);

	CmdBuff->GetBuff()->GetResource()->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

	if (CmdBuff->m_counterBuffer)CmdBuff->m_counterBuffer->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

	//���s
	CmdList->ExecuteIndirect(
		m_cmdSignature.Get(),
		CmdBuff->m_maxCommandCount,
		CmdBuff->m_buff->GetResource()->GetBuff().Get(),
		ArgBufferOffset,
		CmdBuff->m_counterBuffer ? CmdBuff->m_counterBuffer->GetBuff().Get() : nullptr,
		0);
}
