#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
class Sprite;
class TextureBuffer;

class WaveMgr
{
	//コインノルマ
	int m_norma;
	//デバッグ用、ノルマ達成判定を切る
	bool m_isInfinity = true;

	/*--- ノルマ表示 ---*/
	Vec2<float>m_numPos = { 1276.0f,30.0f };
	Vec2<float>m_numPosOffset = { 61.0f, -19.0f };
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;
	int m_useSpriteNum = 0;

public:
	WaveMgr();
	void Init(int arg_norma);
	void Draw2D();

	void ImguiDebug();

	//クリアしたか
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};