#include "PlayerAttack.h"
#include"Collider.h"
#include"ModelAnimator.h"

void PlayerAttack::AnimPlay()
{
	auto animator = m_attachAnimator.lock();
	m_attackFrame = 0;
	animator->speed = m_animSpeed[m_nowIdx];
	animator->Play(GetAnimName(m_nowIdx), false, false);
	m_nextAttack = false;	//���̍U���\��t���O���Z�b�g
	m_momentum = 0.0f;	//�U���̐������Z�b�g
}

#include"HitEffect.h"
void PlayerAttack::OnCollision(const Vec3<float>& Inter, std::weak_ptr<Collider> OtherCollider)
{
	if (m_callBack)
	{
		HitEffect::Generate(Inter);
		m_callBack = false;
	}
}

void PlayerAttack::Attach(std::shared_ptr<ModelAnimator>& Animator, std::shared_ptr<Collider>& AttackCollider)
{
	m_attachAnimator = Animator;
	m_attackCollider = AttackCollider;
}

void PlayerAttack::Init()
{
	m_attackCollider.lock()->SetActive(false);
	m_isActive = false;
	m_momentum = 0.0f;
}

void PlayerAttack::Update()
{
	//�U�����łȂ�
	if (!m_isActive)return;

	//�R���C�_�[�擾
	auto col = m_attackCollider.lock();

	//�U�������������玟�̍U���܂ŃR���C�_�[��OFF�ɂ���
	if (col->GetIsHit())col->SetActive(false);


	//�U���̐����v�Z�i�C�[�W���O�j
	m_momentum = m_momentumEaseParameters[m_nowIdx].Calculate(
		static_cast<float>(m_attackFrame), 
		static_cast<float>(m_momentumFrameNum[m_nowIdx]),
		m_maxMomentum[m_nowIdx], 
		0.0f);
	 
	//�U�����n�܂��Ă���̃t���[�����L�^
	m_attackFrame++;

	auto animator = m_attachAnimator.lock();

	//���݂̍U���A�j���[�V�������I�������玟�̃A�j���[�V������
	if (!animator->IsPlay(GetAnimName(m_nowIdx)))
	{
		//�����̍U���A�j���[�V���������[�v
		m_nowIdx++;
		//���̍U�����͂��Ȃ���΍U���I���
		if (!m_nextAttack)
		{
			Stop();
		}
		else
		{
			//�A�j���[�V�����������Ă��烋�[�v
			if (m_attackAnimNum <= m_nowIdx)m_nowIdx = 0;
			AnimPlay();
			col->SetActive(true);
		}
	}
}

void PlayerAttack::Attack(const Vec3<float>& FrontVec)
{
	//�U���̌���
	m_attackVec = FrontVec;

	//���ɍU������
	if (m_isActive)
	{
		//���̍U���̗\����͂Ƃ��Ď󂯎��
		if(m_canNextInputFrame[m_nowIdx] < m_attackFrame)m_nextAttack = true;
	}
	//�U���J�n
	else
	{
		m_nowIdx = 0;
		AnimPlay();
		m_isActive = true;
		m_callBack = true;
		m_attackCollider.lock()->SetActive(true);
	}
}

void PlayerAttack::Stop()
{
	m_isActive = false;
	m_attackCollider.lock()->SetActive(false);
	m_momentum = 0.0f;
}

#include"imguiApp.h"
void PlayerAttack::ImguiDebug()
{
	static ImVec4 RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImGui::Text("nowAnimIdx : { %d }", m_nowIdx);
	ImGui::TextColored(m_attackFrame && m_isActive ? RED : WHITE, "attackFrame : { %d }", m_attackFrame);
	ImGui::TextColored(m_nextAttack ? RED : WHITE, "nextAttack : { %s }", m_nextAttack ? "TRUE" : "FALSE");

	static int nowSelectIdx = 0;
	ImGui::DragInt("AnimIdx", &nowSelectIdx, 1, 0, m_attackAnimNum);
	if (ImGui::DragInt("canNextInputFrame", &m_canNextInputFrame[nowSelectIdx]) && m_canNextInputFrame[nowSelectIdx] < 0)	//�}�C�i�X�h�~
	{
		m_canNextInputFrame[nowSelectIdx] = 0;
	}
	if (ImGui::DragFloat("speed", &m_animSpeed[nowSelectIdx]) && m_animSpeed[nowSelectIdx] < 0)		//�}�C�i�X�h�~
	{
		m_animSpeed[nowSelectIdx] = 0.0f;
	}
}
