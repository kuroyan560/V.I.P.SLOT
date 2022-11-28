#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
class Sprite;
class TextureBuffer;

class WaveMgr
{
	//�R�C���m���}
	int m_norma;
	//�f�o�b�O�p�A�m���}�B�������؂�
	bool m_isInfinity = true;

	/*--- �m���}�\�� ---*/
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

	//�N���A������
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};