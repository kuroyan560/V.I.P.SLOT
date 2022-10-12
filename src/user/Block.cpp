#include "Block.h"
#include"SlotMachine.h"
#include"DrawFunc3D.h"
#include"Importer.h"

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

std::shared_ptr<Model>CoinBlock::COIN_MODEL;
std::shared_ptr<Model>CoinBlock::EMPTY_COIN_MODEL;

void CoinBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(COIN_MODEL, arg_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
}

CoinBlock::CoinBlock()
{
	if (!COIN_MODEL)
	{
		COIN_MODEL = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
		EMPTY_COIN_MODEL = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");
	}
}

std::shared_ptr<Model>SlotBlock::MODEL;

void SlotBlock::OnInit()
{
}

void SlotBlock::OnUpdate()
{
}

void SlotBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(MODEL, arg_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
}

void SlotBlock::OnHitTrigger()
{
	m_slotMachinePtr.lock()->Lever();
}

SlotBlock::SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine)
	:m_slotMachinePtr(arg_slotMachine)
{
	if (!MODEL)
	{
		MODEL = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	}
}

bool SlotBlock::IsDead()
{
	//‰½‰ñ‚Å”j‰ó‚³‚ê‚é‚©
	const int HP = 3;
	return HP <= m_hitCount;
}