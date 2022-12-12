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

void PlayersNormalAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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

void PlayersParryAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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

void PlayersCounterAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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


void DamagedCallBack::Execute()
{
	using namespace ConstParameter::Player;

	//���G���Ԓ���
	if (!m_invincibleTimer.IsTimeUp())return;

	//�U������
	if (m_parent->IsAttack())return;

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

void DamagedCallBack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_otherCollider)
{
	this->Execute();
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

void PushBackCallBack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
{

}

void PushBackCallBack::OnCollisionEnter(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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
		//�������Ă��Ȃ�
		if (0.0f <= playerMove.y)return;
	}

	
	//�ړ��x�N�g���Ƃ̏Փ˓_�����߂�
	CollisionResultInfo info;
	CollisionLine moveRay(arg_info.m_inter - playerMove, playerMove.GetNormal(), 20.0f);
	moveRay.HitCheckDispatch(XMMatrixIdentity(),
		arg_otherCollider.lock()->GetTransformMat(),
		arg_info.m_hitOtherPrimitive,
		&info);

	float radius = m_player->m_bodySphereCol->m_radius;
	auto offset = m_player->m_bodySphereCol->m_offset;
	auto aabb = (CollisionAABB*)arg_info.m_hitOtherPrimitive;

	//�����Y�������A�������̂݉����߂�
	if (!isScaffold)
	{
		//X�����������߂�
		if (playerMove.x && (m_player->m_oldPos.x < aabb->GetPtValue().x.m_min || aabb->GetPtValue().x.m_max < m_player->m_oldPos.x))pos.x = info.m_inter.x + radius * -(playerMove.GetNormal().x / abs(playerMove.GetNormal().x));
		//if (playerMove.x)pos.x = arg_info.m_inter.x + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().x / abs(playerMove.GetNormal().x));
		//Z�����������߂�
		if (playerMove.z)pos.z = info.m_inter.z + radius * -(playerMove.GetNormal().z / abs(playerMove.GetNormal().z));
		//if (playerMove.z)pos.z = arg_info.m_inter.z + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().z / abs(playerMove.GetNormal().z));
	}
	//Y�����������߂�
	if (playerMove.y)pos.y = info.m_inter.y + radius * -(playerMove.GetNormal().y / abs(playerMove.GetNormal().y));
	//if (playerMove.y)pos.y = arg_info.m_inter.y + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().y / abs(playerMove.GetNormal().y));
	
	pos += offset;

	m_player->OnLanding(!isScaffold);

	//�����߂���̍��W�K�p
	m_player->m_modelObj->m_transform.SetPos(pos);
}
