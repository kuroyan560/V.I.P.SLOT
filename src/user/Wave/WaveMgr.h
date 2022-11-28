#pragma once
#include<vector>
#include<memory>
class Sprite;
class TextureBuffer;

class WaveMgr
{
	//コインノルマ
	int m_norma;
	//デバッグ用、ノルマ達成判定を切る
	bool m_isInfinity = true;

	/*--- ノルマ表示 ---*/
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;

public:
	WaveMgr();
	void Init(int arg_norma);

	void ImguiDebug();

	//クリアしたか
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};