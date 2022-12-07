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
	struct DamageEffect
	{
	private:
		//�ҋ@����
		Timer m_waitTimer;
		//�`��ł̕ω��^�C�}�[
		Timer m_drawChangeTimer;
		//�h��
		ImpactShake m_shake = ImpactShake(Vec3<float>(1.0f, 1.0f, 0.0f));

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;
	public:
		//HP�o�[�i�_���[�W�j
		DrawContents m_hpBarDamage;
		void Init()
		{
			m_shake.Init();
			m_hpBarDamage.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate, std::weak_ptr<Sprite>arg_hpBarSprite);
		void Update(float arg_timeScale);
		void Interruput() { Init(); }
		Vec2<float>GetOffset()const
		{
			return { m_shake.GetOffset().x,m_shake.GetOffset().y };
		}
	}m_damageEffect;

	/*--- HP�񕜉��o ---*/
	struct HealEffect
	{
	private:
		//�ҋ@����
		Timer m_waitTimer;
		//�`��ł̕ω��^�C�}�[
		Timer m_drawChangeTimer;
		//�p�[�e�B�N���̕��o�X�p��
		Timer m_ptEmitTimer;

		//�_�Ń��W�A��
		float m_flashRadian;

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;

		//ParticleMgr����󂯎�����p�[�e�B�N��ID
		int m_particleID;

		void EmitParticle(Vec2<float>arg_basePos, float arg_randMaxPosX);

	public:
		//HP�o�[�i�񕜁j
		DrawContents m_hpBarHeal;

		HealEffect();

		void Init()
		{
			m_hpBarHeal.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate);
		void Update(float arg_timeScale, Vec2<float>arg_uiPos, std::weak_ptr<Sprite>arg_hpBarSprite);
		void Interruput() { Init(); }
	}m_healEffect;

	/*--- ���C�t����o ---*/
	struct ConsumeLifeEffect
	{
		//���C�����C�t�i�_���[�W�j
		DrawContents m_damageHeart;

		void Init()
		{
			m_damageHeart.m_active = false;
		}
		void Start(DrawContents* arg_hpBar);
		void Update(float arg_timeScale, DrawContents* arg_hpBar);
	}m_consumeLifeEffect;

	//HP�o�[�T�C�Y�擾
	static Vec2<float>CalculateHpBarSize(Vec2<float>arg_texSize, float arg_rate);

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