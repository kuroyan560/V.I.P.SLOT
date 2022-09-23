#pragma once
#include<memory>
class TextureBuffer;
#include<string>

//アビリティ呼び出し通知
class Ability
{
private:
	friend class AbilitySystem;
	//発動通知フラグ
	bool m_invokeNotify = false;
	//アイコンテクスチャ
	std::shared_ptr<TextureBuffer>m_iconTex;

protected:
	void Finish()
	{
		m_invokeNotify = false;
	}
	const bool& GetInvokeNotify()const { return m_invokeNotify; }

public:
	Ability(const std::string& TexName);

	//インターバル（MAXで発動可能）
	virtual float IntervalRate() { return 1.0f; }
	//回数制限で全部使い切ったか
	virtual bool IsCountEmpty() { return false; }

	virtual void ImguiDebug() {}
};