#include "StageMgr.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Transform.h"
#include"Importer.h"
#include"DrawFunc3D.h"

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

	m_slotBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	m_coinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
	m_emptyCoinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");
}

void StageMgr::Init(std::string arg_mapFilePath)
{
	//地形クリア
	m_terrianBlockArray.clear();

	//地形構成
	//※本来ならファイルから地形読み込み

	const Vec2<int>MAX_BLOCK_NUM = { 20,15 };
	if (MAX_BLOCK_NUM.x < m_blockNum.x)m_blockNum.x = MAX_BLOCK_NUM.x;
	if (MAX_BLOCK_NUM.y < m_blockNum.y)m_blockNum.y = MAX_BLOCK_NUM.y;

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
			//ブロックなし
			if (block == nullptr)continue;

			block->Update();
		}
	}
}

void StageMgr::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	using namespace ConstParameter::Environment;

	//ブロックの一辺長さ
	const float SIDE = 2.0f;

	Vec2<float>scale = { 1.0f,1.0f };
	scale.y = (FIELD_HEIGHT - FLOOR_HEIGHT) / (SIDE * m_blockNum.y);
	scale.x = FIELD_WIDTH / (SIDE * m_blockNum.x);

	//スケール適用後のブロックの一辺の長さ
	Vec2<float>sideFixed = { SIDE * scale.x ,SIDE * scale.y };
	//スケール適用後のブロックの一辺の長さ半分
	Vec2<float> sideFixedHalf = sideFixed / 2.0f;
	//ブロック同士の座標オフセット
	Vec2<float>offset = sideFixed;

	const float leftX = -((floor(m_blockNum.x / 2.0f) - 1) * sideFixed.x) + (-sideFixedHalf.x * (m_blockNum.x % 2 == 0 ? 1 : 2));
	const float topY = FIELD_HEIGHT - sideFixedHalf.y;

	Transform transform;
	transform.SetScale({ scale.x,scale.y,1.0f });

	std::vector<Matrix>coinBlockTransformArray;
	std::vector<Matrix>emptyCoinBlockTransformArray;

	for (int y = 0; y < m_blockNum.y; ++y)
	{
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			auto& block = m_terrianBlockArray[y][x];

			//ブロックなし
			if (block == nullptr)continue;

			transform.SetPos({ leftX + x * offset.x,topY - y * offset.y,FIELD_DEPTH_FIXED });

			//スロットブロック
			if (block->GetType() == Block::SLOT)
			{
				DrawFunc3D::DrawNonShadingModel(m_slotBlockModel, transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
			}
			//コインブロック
			if (block->GetType() == Block::COIN)
			{
				//コイン既に排出済か
				auto coinBlock = std::dynamic_pointer_cast<CoinBlock>(block);
				if (coinBlock->IsEmpty())emptyCoinBlockTransformArray.emplace_back(transform.GetMat());
				else coinBlockTransformArray.emplace_back(transform.GetMat());
			}
			block->Draw(transform, arg_lightMgr, arg_cam);
		}
	}

	//コインブロックをインスタンシング描画
	if (!coinBlockTransformArray.empty())
		DrawFunc3D::DrawNonShadingModel(m_coinBlockModel, coinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
	if (!emptyCoinBlockTransformArray.empty())
		DrawFunc3D::DrawNonShadingModel(m_emptyCoinBlockModel, emptyCoinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
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
