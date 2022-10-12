#pragma once
#include<memory>
#include<array>
#include"ConstParameters.h"
class Pattern;
class TextureBuffer;
class Player;

//絵柄の定義
class PatternManager
{
private:
	using PATTERN = ConstParameter::Slot::PATTERN;

	//定義した絵柄配列
	std::array<std::shared_ptr<Pattern>, static_cast<int>(PATTERN::NUM)>m_patterns;

	//列挙子より絵柄取得
	std::shared_ptr<Pattern>& GetPattern(PATTERN arg_pattern);

public:
	PatternManager();
	//絵柄のテクスチャ取得
	std::shared_ptr<TextureBuffer>GetTex(PATTERN arg_pattern);
	//絵柄を指定して効果を発動させる
	void Invoke(PATTERN arg_pattern);
};