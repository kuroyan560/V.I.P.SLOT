#pragma once
#include<memory>
class TextureBuffer;

//�X���b�g�̊G��
class Pattern
{
	//�G���̃e�N�X�`��
	std::shared_ptr<TextureBuffer>m_tex;
protected:
	Pattern() {}

public:
	virtual ~Pattern() {}
	//���ʔ���
	virtual void Invoke() = 0;
	//�e�N�X�`���̎擾
	std::shared_ptr<TextureBuffer>GetTex() { return m_tex; }
};

//���ɉ������Ȃ�
class NonePattern : public Pattern
{
public:
	void Invoke()override {}
};