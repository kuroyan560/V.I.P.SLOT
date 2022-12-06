#include "PlayersCallBack.h"
#include"Collider.h"
#include"GameObject.h"
#include"TexHitEffect.h"
#include"AudioApp.h"
#include"ObjectManager.h"
#include"Player.h"
#include"Yoyo.h"
#include"TimeScale.h"
#include"GameCamera.h"
#include"CoinVault.h"

void PlayersNormalAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//�R���C�_�[�̐e�ɐݒ肳��Ă���I�u�W�F�N�g�̃|�C���^�擾
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();
	//�I�u�W�F�N�g�Ƀ_���[�W��^����
	if (int coinNum = obj->Damage(*m_offensive))
	{
		//���SSE
		AudioApp::Instance()->PlayWave(m_killSE);
		//�v���C���[�ɂ�����n��
		m_playersVault->Add(coinNum);
	}
	else
	{
		//�_���[�WSE
		AudioApp::Instance()->PlayWave(m_hitSE);
		//�q�b�g�G�t�F�N�g
		m_hitEffect.lock()->Emit(arg_inter);
	}
}

void PlayersParryAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//�R���C�_�[�̐e�ɐݒ肳��Ă���I�u�W�F�N�g�̃|�C���^�擾
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();
	//�I�u�W�F�N�g�Ƀ_���[�W��^����
	obj->Damage(*m_offensive);
	//�G�̍U���̐e�I�u�W�F�N�g�擾
	auto parentObj = obj->GetParentObj();
	//�e�I�u�W�F�N�g�Ɍ������Ēe����
	if (parentObj)
	{
		//�p���B�e�̔���
		m_objMgr.lock()->AppearParryBullet(
			m_colMgr,
			obj->m_transform.GetPos(),
			parentObj);
	}
	//SE
	AudioApp::Instance()->PlayWave(m_parrySE);
}

void PlayersCounterAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//�R���C�_�[�̐e�ɐݒ肳��Ă���I�u�W�F�N�g�̃|�C���^�擾
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();

	//�q�b�g�J�E���g�C���N�������g
	m_hitCount++;

	//�I�u�W�F�N�g�Ƀ_���[�W��^����
	if (int coinNum = obj->Damage(*m_offensive))
	{
		//���SSE
		AudioApp::Instance()->PlayWave(m_killSE);
		//�v���C���[�ɂ�����n���i���Ă�����������قǓ�����R�C����������j
		m_playersVault->Add(coinNum * m_hitCount);
	}
	else
	{
		//�_���[�WSE
		AudioApp::Instance()->PlayWave(m_hitSE);
	}

	//�q�b�g�G�t�F�N�g
	m_hitEffect->Emit(arg_inter, m_hitCount);
}


void DamagedCallBack::OnCollisionTrigger(const Vec3<float>& arg_inter,
	std::weak_ptr<Collider>arg_otherCollider)
{
	using namespace ConstParameter::Player;

	//���G���Ԓ���
	if (!m_invincibleTimer.IsTimeUp())return;

	//�U������
	if (m_parent->IsAttack())return;

	//HP����
	m_parent->Damage(1);

	//���G���Ԑݒ�
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�v
	m_hitStopTimer.Reset(HIT_STOP_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�vSE�Đ�
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//�_��
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
}

void DamagedCallBack::Update(TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	const float& timeScale = arg_timeScale.GetTimeScale();

	//�q�b�g�X�g�b�v�n��
	if (m_hitStopTimer.IsTimeStartOnTrigger())
	{
		arg_timeScale.Set(0.0f);
	}
	//�q�b�g�X�g�b�v�I���
	else if (m_hitStopTimer.IsTimeUpOnTrigger())
	{
		arg_timeScale.Set(1.0f);
		AudioApp::Instance()->PlayWave(m_damageSE);
		//�J�����U��
		m_cam.lock()->Shake(60, 2, 2.0f, 1.0f);
	}

	m_hitStopTimer.UpdateTimer();

	//���G����
	if (m_invincibleTimer.UpdateTimer(timeScale))
	{
		//�_�ŏI��
		m_isDraw = true;
	}
	else
	{
		//�_��
		if (m_flashTimer.UpdateTimer(timeScale))
		{
			m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
			m_isDraw = !m_isDraw;
		}
	}
}

