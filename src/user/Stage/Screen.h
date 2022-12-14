#pragma once
#include<memory>
class ModelObject;
class RenderTarget;
class TextureBuffer;
class LightManager;
class Camera;

class Screen
{
	//�X�N���[���̃����_�[�^�[�Q�b�g
	std::shared_ptr<RenderTarget>m_renderTarget;
	//�����f���Ă��Ȃ���Ԃ̂Ƃ��̃e�N�X�`��
	std::shared_ptr<TextureBuffer>m_clearTex;
	//�`��t���O
	bool m_draw;

public:
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	Screen();

	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//�����_�[�^�[�Q�b�g�N���A
	void ClearTarget();
	//�����_�[�^�[�Q�b�g�Z�b�g
	void SetTarget();
	//�`��t���O�Z�b�^
	void SetDrawFlg(bool arg_draw) { m_draw = arg_draw; }

};