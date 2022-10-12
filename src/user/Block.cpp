#include "Block.h"
#include"SlotMachine.h"

void Block::Init()
{
	m_hitCount = 0;
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

void Block::HitTrigger()
{
	m_hitCount++;
	OnHitTrigger();
}

void CoinBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
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
	m_slotMachinePtr.lock()->Lever();
}

SlotBlock::SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine)
	:m_slotMachinePtr(arg_slotMachine)
{
}

bool SlotBlock::IsDead()
{
	//‰½‰ñ‚Å”j‰ó‚³‚ê‚é‚©
	const int HP = 3;
	return HP <= m_hitCount;
}