#include "Enemy.h"
#include"EnemyBreed.h"
#include"ActPoint.h"
#include"Player.h"
#include"ColliderWithFloor.h"
#include"HitEffect.h"

void Enemy::Init()
{
	m_hp = m_breed.GetMaxHp();
	if (m_control)m_control->Init();

	m_withFloorCollider->Init();
}

void Enemy::Update(const Player& Player, const float& Gravity)
{
	//����ł���
	if (!IsAlive())return;

	//�ړ���
	Vec3<float>move = { 0,0,0 };

	//���g�̍��W
	Vec3<float>myPos = m_transform.GetPos();

	//�v���C���[�̃��b�V���V�X�e���擾
	const auto& rushSys = Player.GetRushSys();

	bool onLock = false;
	bool onRush = false;
	//ActPoint�̒ʒm���󂯎���ă��b�V�����ꂽ�u�ԂȂ烉�b�V���������܂�t���OON
	for (const auto& actPt : m_actPointArray)
	{
		if (!onRush)onRush = actPt->OnRushBlow();
		if (!onLock)onLock = actPt->OnLock();
	}

	//���b�V���Ɋ������܂�Ă��邩����
	//��Last���b�V���t���O�Ɠ�����ON�ɂȂ��Ă���\��������̂ŁA����if��(m_involvedRush)�ƌq���Ȃ�
	if (!m_involvedRush && onRush)
	{
		//�ӂ���΂���钼�O�̍��W
		m_beforeBlowPos = myPos;
		//
		m_withFloorCollider->Jump(rushSys.GetBlowPowerY());

		//�_���[�W
		Damage(rushSys.GetOneRushDamage());

		//�G�t�F�N�g
		HitEffect::Generate(myPos);

		m_involvedRush = true;
	}

	//���b�V���Ɋ������܂�ē����Ȃ��Ȃ�
	if (!m_involvedLocked && onLock)m_involvedLocked = true;

	//���b�V�����I�����Ă��邩
	if (m_involvedRush && rushSys.IsFinishRush())
	{
		//�������܂�t���Ooff��
		m_involvedRush = false;
		//�Ăѓ�����悤�ɂȂ�
		m_involvedLocked = false;
	}

	//���b�V���Ɋ������܂�Ă���Œ�
	if (m_involvedRush)
	{
		//�P���b�V�����I������^�C�~���O
		if (rushSys.IsOneRushTrigger())
		{
			//�ӂ���΂���钼�O�̍��W
			m_beforeBlowPos = myPos;
			//�_���[�W
			Damage(rushSys.GetOneRushDamage());
			//�G�t�F�N�g
			HitEffect::Generate(myPos);
		}

		//�Ō�̃��b�V������
		if (rushSys.IsLastRush())
		{
			//�v���C���[�������Ă�������̂ӂ���΂����
			move += Player.GetTransform().GetFront() * rushSys.GetNowBlowPower();
		}
		else
		{
			//�������܂�Ăӂ��Ƃ΂��ꂽ��̍��W
			Vec3<float>involvedTargetPos = rushSys.GetInvolvedObjsTargetPos();
			//�ӂ���΂��̎��Ԋ���
			float blowRate = rushSys.GetBlowRate();

			//�ӂ���΂����̍��W�ɋ߂Â�
			//m_transform.SetPos(KuroMath::Lerp(m_transform.GetPos(), involvedTargetPos, 0.1f));
			m_transform.SetPos(KuroMath::Ease(Out, Cubic, blowRate, m_beforeBlowPos, involvedTargetPos));
		}
	}

	//�������ݒ肳��Ă��āA���b�V���ɂ���ē��������b�N����Ă��Ȃ���΁A���̍X�V����
	if (m_control && !m_involvedLocked)m_control->Update();

	//����
	move.y += m_withFloorCollider->GetFallSpeed();
	m_withFloorCollider->AddFallSpeed(Gravity * (m_involvedRush ? rushSys.GetBlowGravityRate() : 1.0f));

	//�ړ��ʂ𔽉f������
	auto newPos = myPos;
	newPos += move;
	m_transform.SetPos(newPos);
}

void Enemy::Damage(const int& Amount)
{
	m_hp -= Amount;
}