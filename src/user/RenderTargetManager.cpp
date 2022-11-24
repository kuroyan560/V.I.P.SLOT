#include "RenderTargetManager.h"
#include"KuroEngine.h"

RenderTargetManager::RenderTargetManager()
{
	auto& app = *D3D12App::Instance();

	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();

	//�o�b�N�o�b�t�@�̃T�C�Y�擾
	auto backBuffSize = backBuff->GetGraphSize();

	//�f�v�X�X�e���V������
	m_depthStencil = app.GenerateDepthStencil(backBuffSize);

	//�G�~�b�V�u�}�b�v����
	m_emissiveMap = app.GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0, 0, 0, 1), backBuffSize, L"EmissiveMap");

	//�f�v�X�}�b�v����
	m_depthMap = app.GenerateRenderTarget(DXGI_FORMAT_R32_FLOAT, Color(0, 0, 0, 0), backBuffSize, L"DepthMap");

}

void RenderTargetManager::Clear()
{
	auto & app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//�S�ăN���A
	graphics.ClearRenderTarget(app.GetBackBuffRenderTarget());
	graphics.ClearDepthStencil(m_depthStencil);
	graphics.ClearRenderTarget(m_emissiveMap);
	graphics.ClearRenderTarget(m_depthMap);
}

void RenderTargetManager::Clear(DRAW_TARGET_TAG arg_tag)
{
	auto& app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//�w�肵�ăN���A
	switch (arg_tag)
	{
	case DRAW_TARGET_TAG::BACK_BUFF:
		graphics.ClearRenderTarget(app.GetBackBuffRenderTarget());
		break;
	case DRAW_TARGET_TAG::DEPTH_STENCIL:
		graphics.ClearDepthStencil(m_depthStencil);
		break;
	case DRAW_TARGET_TAG::EMISSIVE_MAP:
		graphics.ClearRenderTarget(m_emissiveMap);
		break;
	case DRAW_TARGET_TAG::DEPTH_MAP:
		graphics.ClearRenderTarget(m_depthMap);
		break;
	}
}

void RenderTargetManager::Set(bool arg_depthStencil, std::vector<DRAW_TARGET_TAG> arg_tag)
{
	auto& app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//�Z�b�g���郌���_�[�^�[�Q�b�g���^�O�z�񂩂�m�F
	std::vector<std::weak_ptr<RenderTarget>>rts;
	for (auto& tag : arg_tag)
	{
		switch (tag)
		{
		case DRAW_TARGET_TAG::BACK_BUFF:
			rts.push_back(app.GetBackBuffRenderTarget());
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
	graphics.SetRenderTargets(rts, arg_depthStencil ? m_depthStencil : std::weak_ptr<DepthStencil>());
}

std::shared_ptr<TextureBuffer> RenderTargetManager::GetDepthMap()
{
	return m_depthMap;
}
