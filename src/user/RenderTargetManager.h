#pragma once
#include"Singleton.h"
#include<memory>
#include<vector>
class RenderTarget;
class DepthStencil;

enum struct DRAW_TARGET_TAG { BACK_BUFF, DEPTH_STENCIL, EMISSIVE_MAP, DEPTH_MAP };

class RenderTargetManager : public Singleton<RenderTargetManager>
{
	friend class Singleton<RenderTargetManager>;
	RenderTargetManager();

	//デプスステンシル
	std::shared_ptr<DepthStencil>m_depthStencil;

	//エミッシブマップ
	std::shared_ptr<RenderTarget>m_emissiveMap;

	//デプスマップ
	std::shared_ptr<RenderTarget>m_depthMap;

public:
	void Clear();
	void Clear(DRAW_TARGET_TAG arg_tag);
	void Set(bool arg_depthStencil,
		std::vector<DRAW_TARGET_TAG>arg_tag = {
			DRAW_TARGET_TAG::BACK_BUFF,
			DRAW_TARGET_TAG::EMISSIVE_MAP,
			DRAW_TARGET_TAG::DEPTH_MAP,
		});
};