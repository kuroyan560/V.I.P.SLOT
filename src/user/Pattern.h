#pragma once
#include<memory>
#include"CoinVault.h"
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

//�{��UP
class MultiplyPattern : public Pattern
{
	float m_rate = 1.0f;
	CoinVault* m_slotCoinVault = nullptr;
public:
	MultiplyPattern(float arg_rate, CoinVault* arg_target) :m_rate(arg_rate), m_slotCoinVault(arg_target) {}
	void Invoke()override
	{
		m_slotCoinVault->Add(m_slotCoinVault->GetNum() * m_rate);
	}
};