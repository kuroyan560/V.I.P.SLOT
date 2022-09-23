#include "GraphicsManager.h"

void GraphicsManager::SetRenderTargetsCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
	for (auto ptr : m_renderTargets)
	{
		rtvs.emplace_back(ptr.lock()->AsRTV(CmdList));
	}

	const Vec2<float> targetSize = m_renderTargets[0].lock()->GetGraphSize().Float();
	//�r���[�|�[�g�ݒ�
	auto viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, targetSize.x, targetSize.y);
	CmdList->RSSetViewports(1, &viewPort);

	//�V�U�[��`�ݒ�
	auto rect = CD3DX12_RECT(0, 0, static_cast<LONG>(targetSize.x), static_cast<LONG>(targetSize.y));
	CmdList->RSSetScissorRects(1, &rect);

	//�f�v�X�X�e���V��������ꍇ
	if (auto ptr = m_depthStencil.lock())
	{
		CmdList->OMSetRenderTargets(static_cast<UINT>(rtvs.size()), &rtvs[0], FALSE, ptr->AsDSV(CmdList));
	}
	//�Ȃ��ꍇ
	else
	{
		CmdList->OMSetRenderTargets(static_cast<UINT>(rtvs.size()), &rtvs[0], FALSE, nullptr);
	}
}

void GraphicsManager::ClearRTVCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	m_renderTarget.lock()->Clear(CmdList);
}

void GraphicsManager::ClearDSVCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	m_depthStencil.lock()->Clear(CmdList);
}

void GraphicsManager::RenderCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�f�B�X�N���v�^�Z�b�g
	for (int i = 0; i < m_registerDescDatas.size(); ++i)
	{
		if (!m_registerDescDatas[i].m_descData)continue;
		m_registerDescDatas[i].SetAsGraphics(CmdList, i);
	}

	m_vertexBuff.lock()->ChangeBarrierForVertexBuffer(CmdList);

	//���_�r���[�Z�b�g
	m_vertexBuff.lock()->SetView(CmdList);

	//�C���f�b�N�X�Ȃ�
	if (m_idxBuff.expired())
	{
		CmdList->DrawInstanced(m_vertexBuff.lock()->m_sendVertexNum, m_instanceNum, 0, 0);
	}
	//�C���f�b�N�X����
	else
	{
		m_idxBuff.lock()->SetView(CmdList);
		CmdList->DrawIndexedInstanced(m_idxBuff.lock()->m_indexNum, m_instanceNum, 0, 0, 0);
	}
}

void GraphicsManager::DispatchCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�f�B�X�N���v�^�Z�b�g
	for (int i = 0; i < m_registerDescDatas.size(); ++i)
	{
		if (!m_registerDescDatas[i].m_descData)continue;
		m_registerDescDatas[i].SetAsCompute(CmdList, i);
	}
	CmdList->Dispatch(static_cast<UINT>(m_threadNum.x), static_cast<UINT>(m_threadNum.y), static_cast<UINT>(m_threadNum.z));
}


void GraphicsManager::SetRenderTargets(std::vector<std::weak_ptr<RenderTarget>> RTs, std::weak_ptr<DepthStencil> DS)
{
	if (!m_renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	m_gCommands.emplace_back(std::make_shared<SetRenderTargetsCommand>(RTs, DS));

	//�ŐV�̃����_�[�^�[�Q�b�g�̃t�H�[�}�b�g���L�^���Ă���
	m_recentRenderTargetFormat.clear();
	for (auto& rt : RTs)
	{
		m_recentRenderTargetFormat.emplace_back(rt.lock()->GetDesc().Format);
	}
}

void GraphicsManager::SetGraphicsPipeline(std::weak_ptr<GraphicsPipeline> Pipeline)
{
	//�V�����p�C�v���C���̃n���h���擾
	const int newPipelineHandle = Pipeline.lock()->GetPipelineHandle();

	//���ɃZ�b�g����Ă������̂Ɠ����Ȃ�X���[
	if (m_recentPipelineType == GRAPHICS && newPipelineHandle == m_recentPipelineHandle)return;

	if (!m_renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	m_gCommands.emplace_back(std::make_shared<SetGraphicsPipelineCommand>(Pipeline));

	//�p�C�v���C���^�C�v�L�^
	m_recentPipelineType = GRAPHICS;

	//�p�C�v���C���n���h���L�^
	m_recentPipelineHandle = newPipelineHandle;
}

void GraphicsManager::SetComputePipeline(std::weak_ptr<ComputePipeline> Pipeline)
{
	//�V�����p�C�v���C���̃n���h���擾
	const int newPipelineHandle = Pipeline.lock()->GetPipelineHandle();

	//���ɃZ�b�g����Ă������̂Ɠ����Ȃ�X���[
	if (m_recentPipelineType == COMPUTE && newPipelineHandle == m_recentPipelineHandle)return;

	if (!m_renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	m_gCommands.emplace_back(std::make_shared<SetComputePipelineCommand>(Pipeline));

	//�p�C�v���C���^�C�v�L�^
	m_recentPipelineType = COMPUTE;

	//�p�C�v���C���n���h���L�^
	m_recentPipelineHandle = newPipelineHandle;
}

void GraphicsManager::ClearRenderTarget(std::weak_ptr<RenderTarget> RenderTarget)
{
	if (!m_renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	m_gCommands.emplace_back(std::make_shared<ClearRTVCommand>(RenderTarget));
}

void GraphicsManager::ClearDepthStencil(std::weak_ptr<DepthStencil> DepthStencil)
{
	if (!m_renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	m_gCommands.emplace_back(std::make_shared<ClearDSVCommand>(DepthStencil));
}

void GraphicsManager::CopyTexture(std::weak_ptr<TextureBuffer> DestTex, std::weak_ptr<TextureBuffer> SrcTex)
{
	m_gCommands.emplace_back(std::make_shared<CopyTex>(DestTex, SrcTex));
}

void GraphicsManager::ObjectRender(std::weak_ptr<VertexBuffer> VertexBuff, std::vector<RegisterDescriptorData> DescDatas,
	const float& Depth, const bool& TransFlg, const int& InstanceNum)
{
	//�\�[�g����̂� gCommands �ł͂Ȃ��ꎞ�I��renderCommands�ɐς�
	m_renderCommands.emplace_back(std::make_shared<RenderCommand>(VertexBuff, DescDatas, Depth, TransFlg, InstanceNum));
}

void GraphicsManager::ObjectRender(std::weak_ptr<VertexBuffer> VertexBuff,
	std::weak_ptr<IndexBuffer> IndexBuff,
	std::vector<RegisterDescriptorData> DescDatas,
	const float& Depth, const bool& TransFlg, const int& InstanceNum)
{
	//�\�[�g����̂� gCommands �ł͂Ȃ��ꎞ�I��renderCommands�ɐς�
	m_renderCommands.emplace_back(std::make_shared<RenderCommand>(VertexBuff, IndexBuff, DescDatas, Depth, TransFlg, InstanceNum));
}

void GraphicsManager::Dispatch(const Vec3<int>& ThreadNum, std::vector<RegisterDescriptorData> DescDatas)
{
	m_gCommands.emplace_back(std::make_shared<DispatchCommand>(ThreadNum, DescDatas));
}

void GraphicsManager::ExecuteIndirectDispatch(const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset)
{
	assert(CmdBuff->IndirectType() == DISPATCH);
	assert(IndirectDevice->IndirectType() == DISPATCH);
	m_gCommands.emplace_back(std::make_shared<ExcuteIndirectCommand>(CmdBuff, IndirectDevice, ArgBufferOffset));
}

void GraphicsManager::ExecuteIndirectDraw(const std::shared_ptr<VertexBuffer>& VertexBuff, const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset)
{
	assert(CmdBuff->IndirectType() == DRAW);
	assert(IndirectDevice->IndirectType() == DRAW);
	m_gCommands.emplace_back(std::make_shared<ExcuteIndirectCommand>(CmdBuff, IndirectDevice, ArgBufferOffset, VertexBuff));
}

void GraphicsManager::ExecuteIndirectDrawIndexed(const std::shared_ptr<VertexBuffer>& VertexBuff, const std::shared_ptr<IndexBuffer>& IndexBuff, const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset)
{
	assert(CmdBuff->IndirectType() == DRAW_INDEXED);
	assert(IndirectDevice->IndirectType() == DRAW_INDEXED);
	m_gCommands.emplace_back(std::make_shared<ExcuteIndirectCommand>(CmdBuff, IndirectDevice, ArgBufferOffset, VertexBuff, IndexBuff));
}


void GraphicsManager::StackRenderCommands()
{
	//�\�[�g
	//Z�l��蓧�߂��邩�ǂ������D��x����
	m_renderCommands.sort([](std::shared_ptr<RenderCommand> a, std::shared_ptr<RenderCommand> b)
		{
			if (a->m_trans == b->m_trans)
			{
				//Z�\�[�g�i�Y�������� = ���j
				return b->m_depth < a->m_depth;
			}
			else
			{
				return !a->m_trans && b->m_trans;
			}
		});

	//�ꊇ�X�^�b�N
	for (auto ptr : m_renderCommands)
	{
		//�L���X�g���ăO���t�B�b�N�R�}���h�ɐς�
		m_gCommands.emplace_back(std::static_pointer_cast<GraphicsCommandBase>(ptr));
	}

	//�����_�����O�R�}���h�N���A
	m_renderCommands.clear();
}

void GraphicsManager::CommandsExcute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�Ō�ɐςݏグ��ꂽ�̂������_�����O�R�}���h�������ꍇ
	if (!m_renderCommands.empty())
	{
		//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
		StackRenderCommands();
	}

	int cmdIdx = 0;
	for (auto itr = m_gCommands.begin(); itr != m_gCommands.end(); ++itr)
	{
		(*itr)->Execute(CmdList);
		cmdIdx++;
	}

	//�R�}���h���X�g�N���A
	m_gCommands.clear();
	m_recentPipelineType = NONE;
	m_recentPipelineHandle = -1;
	m_recentRenderTargetFormat.clear();
}

void GraphicsManager::CopyTex::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	m_destTex.lock()->CopyTexResource(CmdList, m_srcTex.lock().get());
}

void GraphicsManager::ExcuteIndirectCommand::Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	if (auto buff = m_vertBuff.lock())buff->SetView(CmdList);
	if (auto buff = m_idxBuff.lock())buff->SetView(CmdList);

	m_indirectDevice.lock()->Execute(CmdList, m_cmdBuff.lock(), m_argBufferOffset);
}

const DXGI_FORMAT& GraphicsManager::GetRecentRenderTargetFormat(const int& Idx)
{
	assert(!m_recentRenderTargetFormat.empty());
	assert(m_recentRenderTargetFormat[Idx] != DXGI_FORMAT_UNKNOWN);
	return m_recentRenderTargetFormat[Idx];
}