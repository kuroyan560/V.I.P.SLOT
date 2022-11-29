#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
class TextureBuffer;
class Sprite;

//�v���C���[�̏�����UI
class PlayersCoinUI
{
	//�������\���Ɏg�������̃e�N�X�`���z��
	std::vector<std::shared_ptr<TextureBuffer>>m_numTexArray;
	//�X�v���C�g�z��
	std::vector<std::shared_ptr<Sprite>>m_spriteArray;
	//�g�p����X�v���C�g�̐�
	int m_useSpriteNum;

	//���W
	Vec2<float>m_numPos = { 1163.0f,42.0f };
	//�����Ԃ̃I�t�Z�b�g
	Vec2<float>m_numPosOffset = { 63.0f, -8.0f };
	//�X�P�[��
	float m_numScale = 1.25f;

	//�R�C�������Ď��A�L�^����
	int m_pastCoinNum = 0;

	//�X�v���C�g�ɐ����𔽉f
	void SpriteUpdate(int arg_num);

	//���o�X�^�[�g
	void Execute(int arg_num);

public:
	PlayersCoinUI();

	//������
	void Init(int arg_initNum = 0);
	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="arg_monitorNum">�Ď����鐔��</param>
	void Update(int arg_monitorNum);
	//�`��
	void Draw2D();

	//imgui�f�o�b�O
	void ImguiDebug();
};