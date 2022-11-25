#pragma once
#include"Singleton.h"
#include<memory>
#include<vector>
class RenderTarget;
class DepthStencil;
class TextureBuffer;

enum struct DRAW_TARGET_TAG { BACK_BUFF, DEPTH_STENCIL, EMISSIVE_MAP, DEPTH_MAP, EDGE_COLOR_MAP };

class RenderTargetManager : public Singleton<RenderTargetManager>
{
	friend class Singleton<RenderTargetManager>;
	RenderTargetManager();

	//�f�v�X�X�e���V��
	std::shared_ptr<DepthStencil>m_depthStencil;

	//�G�~�b�V�u�}�b�v
	std::shared_ptr<RenderTarget>m_emissiveMap;

	//�f�v�X�}�b�v
	std::shared_ptr<RenderTarget>m_depthMap;

	//�G�b�W�J���[�}�b�v
	std::shared_ptr<RenderTarget>m_edgeColorMap;

public:
	void Clear();
	void Clear(DRAW_TARGET_TAG arg_tag);
	void Set(bool arg_depthStencil,
		std::vector<DRAW_TARGET_TAG>arg_tag = {
			DRAW_TARGET_TAG::BACK_BUFF,
			DRAW_TARGET_TAG::EMISSIVE_MAP,
			DRAW_TARGET_TAG::DEPTH_MAP,
			DRAW_TARGET_TAG::EDGE_COLOR_MAP,
		});

	std::shared_ptr<TextureBuffer>GetDepthMap();
	std::shared_ptr<TextureBuffer>GetEdgeColorMap();
};