#pragma once
#include<memory>
class TextureBuffer;
#include<string>

//�A�r���e�B�Ăяo���ʒm
class Ability
{
private:
	friend class AbilitySystem;
	//�����ʒm�t���O
	bool m_invokeNotify = false;
	//�A�C�R���e�N�X�`��
	std::shared_ptr<TextureBuffer>m_iconTex;

protected:
	void Finish()
	{
		m_invokeNotify = false;
	}
	const bool& GetInvokeNotify()const { return m_invokeNotify; }

public:
	Ability(const std::string& TexName);

	//�C���^�[�o���iMAX�Ŕ����\�j
	virtual float IntervalRate() { return 1.0f; }
	//�񐔐����őS���g���؂�����
	virtual bool IsCountEmpty() { return false; }

	virtual void ImguiDebug() {}
};