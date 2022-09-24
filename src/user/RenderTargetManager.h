#pragma once
#include"Singleton.h"
#include<memory>
#include<vector>
class RenderTarget;
class DepthStencil;
class GraphicsManager;
class D3D12App;

enum struct DRAW_TARGET_TAG { BACK_BUFF, DEPTH_STENCIL, EMISSIVE_MAP, DEPTH_MAP };

class RenderTargetManager : public Singleton<RenderTargetManager>
{
	friend class Singleton<RenderTargetManager>;
	RenderTargetManager() {}

	bool m_invalid = true;

	//�X���b�v�`�F�C���̃����_�[�^�[�Q�b�g
	std::weak_ptr<RenderTarget>m_backBuffer;

	//�f�v�X�X�e���V��
	std::shared_ptr<DepthStencil>m_depthStencil;

	//�G�~�b�V�u�}�b�v
	std::shared_ptr<RenderTarget>m_emissiveMap;

	//�f�v�X�}�b�v
	std::shared_ptr<RenderTarget>m_depthMap;

public:
	void Init(D3D12App& arg_app);
	void Clear(GraphicsManager& arg_graphics);
	void Clear(GraphicsManager& arg_graphics, DRAW_TARGET_TAG arg_tag);
	void Set(GraphicsManager& arg_graphics, bool arg_depthStencil,
		std::vector<DRAW_TARGET_TAG>arg_tag = {
			DRAW_TARGET_TAG::BACK_BUFF,
			DRAW_TARGET_TAG::EMISSIVE_MAP,
			DRAW_TARGET_TAG::DEPTH_MAP,
		});
};