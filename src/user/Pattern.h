#pragma once
#include<memory>
#include"CoinVault.h"
class TextureBuffer;

//スロットの絵柄
class Pattern
{
	//絵柄のテクスチャ
	std::shared_ptr<TextureBuffer>m_tex;
protected:
	Pattern() {}

public:
	virtual ~Pattern() {}
	//効果発動
	virtual void Invoke() = 0;
	//テクスチャの取得
	std::shared_ptr<TextureBuffer>GetTex() { return m_tex; }
};

//倍率UP
class MultiplyPattern : public Pattern
{
	float m_rate = 1.0f;
	CoinVault* m_slotCoinVault = nullptr;
public:
	MultiplyPattern(float arg_rate, CoinVault* arg_target) :m_rate(arg_rate), m_slotCoinVault(arg_target) {}
	void Invoke()override
	{
		int oldCoinNum = m_slotCoinVault->GetNum();
		m_slotCoinVault->Set(oldCoinNum * m_rate);
		printf("Invoke : Multiply %f : %d to %d\n", m_rate, oldCoinNum, m_slotCoinVault->GetNum());
	}
};