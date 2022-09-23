#include "PlayerAttack.h"
#include"Collider.h"
#include"ModelAnimator.h"

void PlayerAttack::AnimPlay()
{
	auto animator = m_attachAnimator.lock();
	m_attackFrame = 0;
	animator->speed = m_animSpeed[m_nowIdx];
	animator->Play(GetAnimName(m_nowIdx), false, false);
	m_nextAttack = false;	//次の攻撃予約フラグリセット
	m_momentum = 0.0f;	//攻撃の勢いリセット
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
	//攻撃中でない
	if (!m_isActive)return;

	//コライダー取得
	auto col = m_attackCollider.lock();

	//攻撃が当たったら次の攻撃までコライダーをOFFにする
	if (col->GetIsHit())col->SetActive(false);


	//攻撃の勢い計算（イージング）
	m_momentum = m_momentumEaseParameters[m_nowIdx].Calculate(
		static_cast<float>(m_attackFrame), 
		static_cast<float>(m_momentumFrameNum[m_nowIdx]),
		m_maxMomentum[m_nowIdx], 
		0.0f);
	 
	//攻撃が始まってからのフレーム数記録
	m_attackFrame++;

	auto animator = m_attachAnimator.lock();

	//現在の攻撃アニメーションが終了したら次のアニメーションへ
	if (!animator->IsPlay(GetAnimName(m_nowIdx)))
	{
		//複数の攻撃アニメーションをループ
		m_nowIdx++;
		//次の攻撃入力がなければ攻撃終わり
		if (!m_nextAttack)
		{
			Stop();
		}
		else
		{
			//アニメーション数超えてたらループ
			if (m_attackAnimNum <= m_nowIdx)m_nowIdx = 0;
			AnimPlay();
			col->SetActive(true);
		}
	}
}

void PlayerAttack::Attack(const Vec3<float>& FrontVec)
{
	//攻撃の向き
	m_attackVec = FrontVec;

	//既に攻撃中か
	if (m_isActive)
	{
		//次の攻撃の予約入力として受け取る
		if(m_canNextInputFrame[m_nowIdx] < m_attackFrame)m_nextAttack = true;
	}
	//攻撃開始
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
	if (ImGui::DragInt("canNextInputFrame", &m_canNextInputFrame[nowSelectIdx]) && m_canNextInputFrame[nowSelectIdx] < 0)	//マイナス防止
	{
		m_canNextInputFrame[nowSelectIdx] = 0;
	}
	if (ImGui::DragFloat("speed", &m_animSpeed[nowSelectIdx]) && m_animSpeed[nowSelectIdx] < 0)		//マイナス防止
	{
		m_animSpeed[nowSelectIdx] = 0.0f;
	}
}
