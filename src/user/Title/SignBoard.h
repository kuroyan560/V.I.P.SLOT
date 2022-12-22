#pragma once
#include<memory>
class RenderTarget;
class TextureBuffer;
class ModelObject;
#include"CRT.h"

class SignBoard
{
	//�X�N���[���̃����_�[�^�[�Q�b�g
	std::shared_ptr<RenderTarget>m_renderTarget;
	//�����f���Ă��Ȃ���Ԃ̂Ƃ��̃e�N�X�`��
	std::shared_ptr<TextureBuffer>m_clearTex;

	//�^�C�g���e�N�X�`��
	std::shared_ptr<TextureBuffer>m_titleTex;

	//CRT�|�X�g�G�t�F�N�g
	CRT m_crt;

public:
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject> m_modelObj;

	SignBoard();

	//��ʂ̉f���X�V
	void UpdateScreen();

	//��ʂ̉f���e�N�X�`���Q�b�^
	std::shared_ptr<TextureBuffer> GetScreenTex();
};

