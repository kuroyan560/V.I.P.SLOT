#pragma once
#include<vector>
#include<memory>
class Sprite;
class TextureBuffer;

class WaveMgr
{
	//�R�C���m���}
	int m_norma;
	//�f�o�b�O�p�A�m���}�B�������؂�
	bool m_isInfinity = true;

	/*--- �m���}�\�� ---*/
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;

public:
	WaveMgr();
	void Init(int arg_norma);

	void ImguiDebug();

	//�N���A������
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};