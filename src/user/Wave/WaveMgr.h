#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
#include"Debugger.h"
#include"HUDInterface.h"
class Sprite;
class TextureBuffer;

class WaveMgr : public Debugger, public HUDInterface
{
	//�R�C���m���}
	int m_norma;
	//�f�o�b�O�p�A�m���}�B�������؂�
	bool m_isInfinity = false;

	/*--- �m���}�\�� ---*/
	Vec2<float>m_numPos = { 1271.0f,10.0f };
	Vec2<float>m_numPosOffset = { 47.0f, -17.0f };
	float m_numScale = 0.85f;
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;
	int m_useSpriteNum = 0;

	void OnImguiItems()override;
	void OnDraw2D()override;

public:
	WaveMgr();
	void Init(int arg_norma);

	//�N���A������
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};