#include "StageMgr.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Transform.h"

StageMgr::StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine)
{
	for (auto& b : m_slotBlocks)
	{
		b = std::make_shared<SlotBlock>(arg_slotMachine);
	}
	
	for (auto& b : m_coinBlocks)
	{
		b = std::make_shared<CoinBlock>();
	}
}

void StageMgr::Init(std::string arg_mapFilePath)
{
	//�n�`�N���A
	m_terrianBlockArray.clear();

	//�n�`�\��
	//���{���Ȃ�t�@�C������n�`�ǂݍ���
	int coinBlockIdx = 0;
	int slotBlockIdx = 0;

	m_terrianBlockArray.resize(m_blockNum.y);
	for (int y = 0; y < m_blockNum.y; ++y)
	{
		m_terrianBlockArray[y].resize(m_blockNum.x);
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			auto& block = m_terrianBlockArray[y][x];
			block = std::dynamic_pointer_cast<Block>(m_coinBlocks[coinBlockIdx++]);
			block->Init();
		}
	}
}

void StageMgr::Update()
{
	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//�u���b�N�Ȃ�
			if (block == nullptr)continue;

			block->Update();
		}
	}
}

void StageMgr::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	using namespace ConstParameter::Environment;

	//�u���b�N�̈�Ӓ���
	const float SIDE = 2.0f;

	Vec2<float>scale = { 1.0f,1.0f };
	scale.y = (FIELD_HEIGHT - FLOOR_HEIGHT) / (SIDE * m_blockNum.y);
	scale.x = FIELD_WIDTH / (SIDE * m_blockNum.x);

	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒���
	Vec2<float>sideFixed = { SIDE * scale.x ,SIDE * scale.y };
	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒�������
	Vec2<float> sideFixedHalf = sideFixed / 2.0f;
	//�u���b�N���m�̍��W�I�t�Z�b�g
	Vec2<float>offset = sideFixed;

	const float leftX = -((floor(m_blockNum.x / 2.0f) - 1) * sideFixed.x) + (-sideFixedHalf.x * (m_blockNum.x % 2 == 0 ? 1 : 2));
	const float topY = FIELD_HEIGHT - sideFixedHalf.y;

	Transform transform;
	transform.SetScale({ scale.x,scale.y,1.0f });

	for (int y = 0; y < m_blockNum.y; ++y)
	{
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			auto& block = m_terrianBlockArray[y][x];

			//�u���b�N�Ȃ�
			if (block == nullptr)continue;

			transform.SetPos({ leftX + x * offset.x,topY - y * offset.y,FIELD_DEPTH_FIXED });
			block->Draw(transform, arg_lightMgr, arg_cam);
		}
	}
}

#include"imguiApp.h"
void StageMgr::ImguiDebug()
{
	ImGui::Begin("StageMgr");

	bool changeX = ImGui::DragInt("BlockNumX", &m_blockNum.x);
	bool changeY = ImGui::DragInt("BlockNumY", &m_blockNum.y);

	if (m_blockNum.x < 1)m_blockNum.x = 1;
	if (m_blockNum.y < 1)m_blockNum.y = 1;

	if (changeX || changeY)Init("");

	ImGui::End();
}
