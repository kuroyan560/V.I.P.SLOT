#pragma once
#include<memory>
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

//特に何もしない
class NonePattern : public Pattern
{
public:
	void Invoke()override {}
};