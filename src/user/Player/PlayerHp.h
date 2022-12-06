#pragma once
#include<memory>
#include"Transform2D.h"
#include<vector>
#include"HUDInterface.h"
class TextureBuffer;
class Sprite;

class PlayerHp : public HUDInterface
{
	//HP��UI�S�̂̃g�����X�t�H�[��
	Transform2D m_transform;
	//�������ʒu
	Vec2<float>m_uiInitPos;

	struct DrawContents
	{
		Vec2<float>m_initPos;
		std::shared_ptr<Sprite>m_sprite;
		bool m_active = true;
	};

	//�uHP�v����
	DrawContents m_hpStr;
	//HP�o�[
	DrawContents m_hpBar;
	//HP�o�[�t���[��
	DrawContents m_hpBarFrame;

	struct Heart
	{
		Vec2<float>m_initPos;
		float m_initScale;
		Transform2D m_offsetTransform;
		DrawContents m_heart;
		DrawContents m_frame;
	};
	std::vector<Heart>m_heartArray;

	//���C�t�i�O�ŃQ�[���I�[�o�[�j
	int m_life;
	//HP�iHP�O�Ń��C�t���P����j
	int m_hp;

	//�`�悷��X�v���C�g���̔z��
	std::vector<DrawContents*>m_contents;

	//HP�o�[�T�C�Y�X�V
	void UpdateHpBarSize();

	//�`��
	void OnDraw2D()override;

public:
	PlayerHp();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="arg_initAbilityMaxLife">���݉������Ă���ő僉�C�t�i�\�͒l��̂̍ő�j</param>
	/// <param name="arg_initRemainLife">�c���C�t</param>
	void Init(int arg_initAbilityMaxLife, int arg_initRemainLife);

	//�X�V
	void Update(const float& arg_timeScale);

	//imgui�f�o�b�O
	void ImguiDebug();

	//HP�ʕϓ�
	void Change(int arg_amount);

	//���C�t�Q�b�^
	const int& GetLife()const { return m_life; }
	//���S������
	bool IsDead()const { return m_hp <= 0 && m_life <= 0; }
};