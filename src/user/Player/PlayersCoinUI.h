#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
#include"Timer.h"
#include"HUDInterface.h"
class TextureBuffer;
class Sprite;

//�v���C���[�̏�����UI
class PlayersCoinUI : public HUDInterface
{
	//�������\���Ɏg�������̃e�N�X�`���z��
	std::vector<std::shared_ptr<TextureBuffer>>m_numTexArray;
	//�X�v���C�g�z��
	std::vector<std::shared_ptr<Sprite>>m_spriteArray;
	//�g�p����X�v���C�g�̐�
	int m_useSpriteNum;

	//���W
	Vec2<float>m_numPos = { 1155.0f,42.0f };
	//�����Ԃ̃I�t�Z�b�g
	Vec2<float>m_numPosOffset = { 63.0f, -8.0f };
	//�X�P�[��
	float m_numScale = 1.25f;

	//�R�C�������Ď��A�L�^����
	int m_pastCoinNum = 0;

	//���o
	struct
	{
		Timer m_timer;
		float m_interval = 45.0f;
		float m_numScaleOffsetMax = 0.5f;
	}m_staging;

	//�X�v���C�g�ɐ����𔽉f
	void SpriteUpdate(int arg_num);

	//���o�X�^�[�g
	void Execute(int arg_num);

	//�`��
	void OnDraw2D()override;

public:
	PlayersCoinUI();

	//������
	void Init(int arg_initNum = 0);
	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="arg_monitorNum">�Ď����鐔��</param>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	void Update(int arg_monitorNum, const float& arg_timeScale);


	//imgui�f�o�b�O
	void ImguiDebug();
};