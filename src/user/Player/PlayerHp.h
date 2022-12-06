#pragma once
#include<memory>
#include"Transform2D.h"
#include<vector>
#include"HUDInterface.h"
#include"Timer.h"
#include"ImpactShake.h"
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
	//HP�o�[�i�_���[�W�j
	DrawContents m_hpBarDamage;
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

	/*--- �_���[�W���o ---*/
	struct
	{
		bool m_active = false;
		//�ҋ@����
		Timer m_waitTimer;
		//�`��ł̕ω��^�C�}�[
		Timer m_drawChangeTimer;
		//�h��
		ImpactShake m_shake = ImpactShake(Vec3<float>(1.0f, 1.0f, 0.0f));

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;

		void Init()
		{
			m_shake.Init();
			m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate)
		{
			m_active = true;
			m_drawChangeTimer.Reset(30.0f);
			m_waitTimer.Reset(45.0f);
			m_shake.Shake(30.0f, 3.0f, 0.0f, 32.0f);
			m_startHpRate = arg_startHpRate;
			m_endHpRate = arg_endHpRate;
		}
		void Update(float arg_timeScale)
		{
			if (m_drawChangeTimer.IsTimeUp())
			{
				m_active = false;
				return;
			}

			if (m_waitTimer.UpdateTimer(arg_timeScale))
			{
				m_drawChangeTimer.UpdateTimer(arg_timeScale);
			}

			//���^�C���X�P�[���̉e�����󂯂Ȃ�
			m_shake.Update(1.0f);
		}
	}m_damageEffect;

	//HP�o�[�T�C�Y�擾
	Vec2<float>CalculateHpBarSize(float arg_rate);

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

	/// <summary>
	/// HP�ʕϓ�
	/// </summary>
	/// <param name="arg_amount">�ϓ���</param>
	/// <returns>���C�t(�n�[�g)���������</returns>
	bool Change(int arg_amount);

	//���C�t�Q�b�^
	const int& GetLife()const { return m_life; }
	//���S������
	bool IsDead()const { return m_hp <= 0 && m_life <= 0; }
};