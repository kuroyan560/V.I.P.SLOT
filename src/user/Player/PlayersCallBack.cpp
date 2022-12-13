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
#include"Object.h"

void PlayersNormalAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_myCollider, std::weak_ptr<Collider> arg_otherCollider)
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
		m_hitEffect.lock()->Emit(arg_info.m_inter);
	}
}

void PlayersParryAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_myCollider, std::weak_ptr<Collider> arg_otherCollider)
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

void PlayersCounterAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_myCollider, std::weak_ptr<Collider> arg_otherCollider)
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
	m_hitEffect->Emit(arg_info.m_inter, m_hitCount);
}


void DamagedCallBack::Execute(bool arg_overlapped)
{
	using namespace ConstParameter::Player;

	if (!arg_overlapped)
	{
		//���G���Ԓ���
		if (!m_invincibleTimer.IsTimeUp())return;

		//�U������
		if (m_parent->IsAttack())return;
	}

	//HP����
	bool consumeLife = m_parent->Damage(1);

	//���G���Ԑݒ�
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�v
	m_hitStopTimer.Reset(consumeLife ? HIT_STOP_TIME_ON_DAMAGED_CONSUME_LIFE : HIT_STOP_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�vSE�Đ�
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//�_��
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
}

void DamagedCallBack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_myCollider, std::weak_ptr<Collider>arg_otherCollider)
{
	this->Execute(false);
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

void PushBackCallBack::OnCollisionEnter(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_myCollider, std::weak_ptr<Collider> arg_otherCollider)
{
	//�v���C���[�̍��W�擾
	auto pos = m_player->m_modelObj->m_transform.GetPos();
	//�v���C���[�̍��W�̕ω��ʎ擾
	Vec3<float>playerMove = pos - m_player->m_oldPos;

	//�ړ����Ă��Ȃ�
	if (playerMove.Length() <= FLT_EPSILON)return;

	//���ꉻ
	bool isScaffold = arg_otherCollider.lock()->HaveTag("Scaffold");

	//���ꂩ
	if (isScaffold)
	{
		//����Ƃ̔����؂��Ă���
		if (!m_notPushBackWithScaffoldTimer.IsTimeUp())return;
	}

	const auto otherAABB = (CollisionAABB*)arg_info.m_hitOtherPrimitive;
	const auto otherMinPt = otherAABB->GetTransformedMin(arg_otherCollider.lock()->GetTransformMat());
	const auto otherMaxPt = otherAABB->GetTransformedMax(arg_otherCollider.lock()->GetTransformMat());

	const auto myMinPt = m_player->m_bodyAABBColPrim->GetTransformedMin(m_player->m_bodyAABBCollider->GetTransformMat());
	const auto myMaxPt = m_player->m_bodyAABBColPrim->GetTransformedMax(m_player->m_bodyAABBCollider->GetTransformMat());
	const auto myAABBSizeHalf = m_player->m_bodyAABBColPrim->GetTransformedSize(arg_otherCollider.lock()->GetTransformMat()) * 0.5f;

	//����łȂ��A��
	if (!isScaffold)
	{
		//X�����������߂�(��)
		if ((myMaxPt.x - playerMove.x) <= otherMinPt.x && otherMinPt.x <= myMaxPt.x)
		{
			//pos.x = otherMinPt.x - myAABBSizeHalf.x;
			pos.x -= abs(otherMinPt.x - myMaxPt.x);
		}
		//X�����������߂�(�E)
		else if (otherMaxPt.x <= (myMinPt.x - playerMove.x) && myMinPt.x <= otherMaxPt.x)
		{
			//pos.x = otherMaxPt.x + myAABBSizeHalf.x;
			pos.x += abs(otherMaxPt.x - myMinPt.x);
		}
		//Y�����������߂�(���j
		if ((myMaxPt.y - playerMove.y) <= otherMinPt.y && otherMinPt.y <= myMaxPt.y)
		{
			//pos.y = otherMinPt.y - myAABBSizeHalf.y;
			pos.y -= abs(otherMinPt.y - myMaxPt.y);
		}
		//Y�����������߂�(��j
		else if (otherMaxPt.y <= (myMinPt.y - playerMove.y) && myMinPt.y <= otherMaxPt.y)
		{
			//pos.y = otherMaxPt.y + myAABBSizeHalf.y;
			pos.y += abs(otherMaxPt.y - myMinPt.y);
			m_player->OnLanding(true);
		}
	}
	//�����Y�������A�������̂݉����߂�
	else
	{
		//Y�����������߂�(��j
		if (otherMaxPt.y <= (myMinPt.y - playerMove.y) && myMinPt.y <= otherMaxPt.y)
		{
			pos.y += abs(otherMaxPt.y - myMinPt.y);
			m_player->OnLanding(false);
		}
	}

	//�����߂���̍��W�K�p
	m_player->m_modelObj->m_transform.SetPos(pos);
}
