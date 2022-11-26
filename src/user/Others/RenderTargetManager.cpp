#include "RenderTargetManager.h"
#include"KuroEngine.h"

RenderTargetManager::RenderTargetManager()
{
	auto& app = *D3D12App::Instance();

	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();

	//バックバッファのサイズ取得
	auto backBuffSize = backBuff->GetGraphSize();

	//デプスステンシル生成
	m_depthStencil = app.GenerateDepthStencil(backBuffSize);

	//エミッシブマップ生成
	m_emissiveMap = app.GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0.0f, 0.0f, 0.0f, 1.0f), backBuffSize, L"EmissiveMap");

	//デプスマップ生成
	m_depthMap = app.GenerateRenderTarget(DXGI_FORMAT_R32_FLOAT, Color(FLT_MAX, 0.0f, 0.0f, 0.0f), backBuffSize, L"DepthMap");
	//エッジカラーマップ用意
	m_edgeColorMap = app.GenerateRenderTarget(backBuff->GetDesc().Format, Color(0.0f, 0.0f, 0.0f, 1.0f), backBuffSize, L"EdgeColorMap");

}

void RenderTargetManager::Clear()
{
	auto & app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//全てクリア
	graphics.ClearRenderTarget(app.GetBackBuffRenderTarget());
	graphics.ClearDepthStencil(m_depthStencil);
	graphics.ClearRenderTarget(m_emissiveMap);
	graphics.ClearRenderTarget(m_depthMap);
	graphics.ClearRenderTarget(m_edgeColorMap);
}

void RenderTargetManager::Clear(DRAW_TARGET_TAG arg_tag)
{
	auto& app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//指定してクリア
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
	case DRAW_TARGET_TAG::EDGE_COLOR_MAP:
		graphics.ClearRenderTarget(m_edgeColorMap);
		break;
	}
}

void RenderTargetManager::Set(bool arg_depthStencil, std::vector<DRAW_TARGET_TAG> arg_tag)
{
	auto& app = *D3D12App::Instance();
	auto& graphics = KuroEngine::Instance()->Graphics();

	//セットするレンダーターゲットをタグ配列から確認
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
		case DRAW_TARGET_TAG::EDGE_COLOR_MAP:
			rts.push_back(m_edgeColorMap);
			break;
		}
	}

	//レンダーターゲットとデプスステンシルのセット
	graphics.SetRenderTargets(rts, arg_depthStencil ? m_depthStencil : std::weak_ptr<DepthStencil>());
}

std::shared_ptr<TextureBuffer> RenderTargetManager::GetDepthMap()
{
	return m_depthMap;
}

std::shared_ptr<TextureBuffer> RenderTargetManager::GetEdgeColorMap()
{
	return m_edgeColorMap;
}
