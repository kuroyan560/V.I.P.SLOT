#include "Block.h"
#include"SlotMachine.h"
#include"Collision.h"
#include"ConstParameters.h"
#include"Collider.h"

Block::Block()
{
}

void Block::Init(Transform& arg_initTransform, std::shared_ptr<Collider>& arg_attachCollider, unsigned char arg_playerColAttaribute)
{
	//初期化トランスフォームの記録と適用
	m_initTransform = arg_initTransform;
	m_initTransform.GetMat();
	m_transform = m_initTransform;

	//叩かれた回数リセット
	m_hitCount = 0;

	//アタッチされたコライダーを記録
	m_attachCollider = arg_attachCollider;
	arg_attachCollider->SetParentTransform(&m_transform);
	arg_attachCollider->SetCallBack(this, arg_playerColAttaribute);
	arg_attachCollider->SetActive(true);

	OnInit();
}

void Block::Update()
{
	OnUpdate();
}

void Block::Draw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	OnDraw(arg_transform, arg_lightMgr, arg_cam);
}

void CoinBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
}

void CoinBlock::OnHitTrigger()
{
	m_attachCollider.lock()->SetActive(false);
}

CoinBlock::CoinBlock()
{

}

void SlotBlock::OnInit()
{
}

void SlotBlock::OnUpdate()
{
}

void SlotBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
}

void SlotBlock::OnHitTrigger()
{
	if (IsDead())
	{
		m_slotMachinePtr.lock()->Lever();
		m_attachCollider.lock()->SetActive(false);
	}
}

SlotBlock::SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine)
	:m_slotMachinePtr(arg_slotMachine)
{
}

bool SlotBlock::IsDead()
{
	//何回で破壊されるか
	const int HP = 3;
	return HP <= m_hitCount;
}