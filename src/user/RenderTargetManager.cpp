#include "RenderTargetManager.h"
#include"KuroEngine.h"

void RenderTargetManager::Init(D3D12App& arg_app)
{
	//�o�b�N�o�b�t�@�擾
	m_backBuffer = arg_app.GetBackBuffRenderTarget();

	//�o�b�N�o�b�t�@�̃T�C�Y�擾
	auto backBuffSize = m_backBuffer.lock()->GetGraphSize();

	//�f�v�X�X�e���V������
	m_depthStencil = arg_app.GenerateDepthStencil(backBuffSize);

	//�G�~�b�V�u�}�b�v����
	m_emissiveMap = arg_app.GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0, 0, 0, 1), backBuffSize, L"EmissiveMap");

	//�f�v�X�}�b�v����
	m_depthMap = arg_app.GenerateRenderTarget(DXGI_FORMAT_R32_FLOAT, Color(0, 0, 0, 0), backBuffSize, L"DepthMap");

	m_invalid = false;
}

void RenderTargetManager::Clear(GraphicsManager& arg_graphics)
{
	//��������
	assert(!m_invalid);

	//�S�ăN���A
	arg_graphics.ClearRenderTarget(m_backBuffer.lock());
	arg_graphics.ClearDepthStencil(m_depthStencil);
	arg_graphics.ClearRenderTarget(m_emissiveMap);
	arg_graphics.ClearRenderTarget(m_depthMap);
}

void RenderTargetManager::Clear(GraphicsManager& arg_graphics, DRAW_TARGET_TAG arg_tag)
{
	//��������
	assert(!m_invalid);

	//�w�肵�ăN���A
	switch (arg_tag)
	{
	case DRAW_TARGET_TAG::BACK_BUFF:
		arg_graphics.ClearRenderTarget(m_backBuffer.lock());
		break;
	case DRAW_TARGET_TAG::DEPTH_STENCIL:
		arg_graphics.ClearDepthStencil(m_depthStencil);
		break;
	case DRAW_TARGET_TAG::EMISSIVE_MAP:
		arg_graphics.ClearRenderTarget(m_emissiveMap);
		break;
	case DRAW_TARGET_TAG::DEPTH_MAP:
		arg_graphics.ClearRenderTarget(m_depthMap);
		break;
	}
}

void RenderTargetManager::Set(GraphicsManager& arg_graphics, bool arg_depthStencil, std::vector<DRAW_TARGET_TAG> arg_tag)
{
	//��������
	assert(!m_invalid);

	//�Z�b�g���郌���_�[�^�[�Q�b�g���^�O�z�񂩂�m�F
	std::vector<std::weak_ptr<RenderTarget>>rts;
	for (auto& tag : arg_tag)
	{
		switch (tag)
		{
		case DRAW_TARGET_TAG::BACK_BUFF:
			rts.push_back(m_backBuffer);
			break;
		case DRAW_TARGET_TAG::DEPTH_STENCIL:
			printf("It's not RenderTarget but DepthStencil.Probably mistaking DepthStencil for RenderTarget.");
			assert(0);
			break;
		case DRAW_TARGET_TAG::EMISSIVE_MAP:
			rts.push_back(m_emissiveMap);
			break;
		case DRAW_TARGET_TAG::DEPTH_MAP:
			rts.push_back(m_depthMap);
			break;
		}
	}

	//�����_�[�^�[�Q�b�g�ƃf�v�X�X�e���V���̃Z�b�g
	arg_graphics.SetRenderTargets(rts, arg_depthStencil ? m_depthStencil : std::weak_ptr<DepthStencil>());
}
