#include "StageMgr.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Transform.h"
#include"Importer.h"
#include"DrawFunc3D.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TexHitEffect.h"
#include"TimeScale.h"
#include"DrawFunc2D.h"
#include<vector>
#include"SlotMachine.h"

void StageMgr::DisappearBlock(std::shared_ptr<Block>& arg_block, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//アタッチされていたコライダー解除
	arg_collisionMgr.lock()->Remove(arg_block->GetCollider());
}

StageMgr::StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine)
{
	using namespace ConstParameter::Stage;

	m_slotMachine = arg_slotMachine;
	
	m_slotBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	m_coinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
	m_emptyCoinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");

	//ヒットエフェクト生成
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3);

	//ブロックのコライダー用プリミティブ配列
	Vec3<ValueMinMax>val;
	val.x.m_min = -BLOCK_LEN_HALF;
	val.x.m_max = BLOCK_LEN_HALF;
	val.y.m_min = -BLOCK_LEN_HALF;
	val.y.m_max = BLOCK_LEN_HALF;
	val.z.m_min = -BLOCK_LEN_HALF;
	val.z.m_max = BLOCK_LEN_HALF;

	//ブロックのコライダー生成
	const float BOX_SIZE = 1.0f;
	Vec3<ValueMinMax>box;
	box.x.m_min = -BOX_SIZE;
	box.y.m_min = -BOX_SIZE;
	box.z.m_min = -BOX_SIZE;
	box.x.m_max = BOX_SIZE;
	box.y.m_max = BOX_SIZE;
	box.z.m_max = BOX_SIZE;

	std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
	colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(1.0f, Vec3<float>(0, 0, 0)));
	auto originCol = std::make_shared<Collider>();
	originCol->Generate("BlockCollider", "Block", colPrimitiveArray);

	for (int i = 0; i < MAX_BLOCK_NUM; ++i)
	{
		m_coinBlocks.emplace_back(std::make_shared<CoinBlock>(originCol));
	}
	for (auto& b : m_slotBlocks)
	{
		b = std::make_shared<SlotBlock>(originCol, arg_slotMachine);
	}

	m_terrianClearTimerGauge = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/terrianTimeGauge.png");
}

void StageMgr::Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Environment;
	using namespace ConstParameter::Stage;

	//ブロック数上限肥えないようにする
	if (MAX_BLOCK_NUM_AXIS.x < m_blockNum.x)m_blockNum.x = MAX_BLOCK_NUM_AXIS.x;
	if (MAX_BLOCK_NUM_AXIS.y < m_blockNum.y)m_blockNum.y = MAX_BLOCK_NUM_AXIS.y;

	//地形クリア
	for (auto& terrianArray : m_terrianBlockArray)
		for (auto& t : terrianArray)
		{
			//ブロックなし
			if (t == nullptr)continue;

			DisappearBlock(t, arg_collisionMgr);
		}
	m_terrianBlockArray.clear();

	//地形構成
	Vec2<float>scale = { 1.0f,1.0f };
	scale.y = (FIELD_HEIGHT_MAX - FIELD_HEIGHT_MIN) / (BLOCK_LEN * m_blockNum.y);
	scale.x = FIELD_WIDTH / (BLOCK_LEN * m_blockNum.x);

	//スケール適用後のブロックの一辺の長さ
	Vec2<float>sideFixed = { BLOCK_LEN * scale.x ,BLOCK_LEN * scale.y };
	//スケール適用後のブロックの一辺の長さ半分
	Vec2<float> sideFixedHalf = sideFixed / 2.0f;
	//ブロック同士の座標オフセット
	Vec2<float>offset = sideFixed;

	const float leftX = -((floor(m_blockNum.x / 2.0f) - 1) * sideFixed.x) + (-sideFixedHalf.x * (m_blockNum.x % 2 == 0 ? 1 : 2));
	const float topY = FIELD_HEIGHT_MAX - sideFixedHalf.y;

	Transform initTransform;
	initTransform.SetScale({ scale.x,scale.y,1.0f });

//※以下、本来ならファイルから読み込み
	//スロットリール交換
	//m_slotMachine.lock()->ReelSet(SlotMachine::LEFT, );
	//m_slotMachine.lock()->ReelSet(SlotMachine::CENTER, );
	//m_slotMachine.lock()->ReelSet(SlotMachine::RIGHT, );

	//コインノルマ
	m_norma = 10;

	//スロットポップ数
	int slotBlockNum = 4;

	//スロットブロック位置
	std::vector<Vec2<int>>slotBlockRandIdx(slotBlockNum);
	for (auto& idx : slotBlockRandIdx)
	{
		idx.x = KuroFunc::GetRand(m_blockNum.x - 1);
		idx.y = KuroFunc::GetRand(m_blockNum.y - 1);
	}

	int slotBlockIdx = 0;
	int coinBlockIdx = 0;

	m_terrianBlockArray.resize(m_blockNum.y);
	for (int y = 0; y < m_blockNum.y; ++y)
	{
		m_terrianBlockArray[y].resize(m_blockNum.x);
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			if (!KuroFunc::Probability(m_generateBlockRate))continue;

			auto& block = m_terrianBlockArray[y][x];
			initTransform.SetPos({ leftX + x * offset.x,topY - y * offset.y,FIELD_DEPTH_FIXED });

			auto itr = std::find(slotBlockRandIdx.begin(), slotBlockRandIdx.end(), Vec2<int>(x, y));
			if (itr == slotBlockRandIdx.end())
			{
				block = std::dynamic_pointer_cast<Block>(m_coinBlocks[coinBlockIdx++]);
			}
			else
			{
				block = std::dynamic_pointer_cast<Block>(m_slotBlocks[slotBlockIdx++]);
			}

			//ブロック初期化、コライダーアタッチ
			arg_collisionMgr.lock()->Register(block->GetCollider());
			block->Init(initTransform, m_hitEffect);
		}
	}

	//地形クリア時間設定
	m_terrianClearTimer.Reset(600.0f);
}

void StageMgr::Update(TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//すべてのコイン排出済か
	bool isAllCoinEmit = true;

	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//ブロックなし
			if (block == nullptr)continue;

			block->Update(arg_timeScale);

			if (block->GetType() == Block::COIN)
			{
				isAllCoinEmit = false;
			}

			if (block->IsDisappear())
			{
				DisappearBlock(block, arg_collisionMgr);
				block = nullptr;
			}
		}
	}

	m_hitEffect->Update(arg_timeScale.GetTimeScale());


	//地形クリア時間経過か、全コイン排出で地形変更
	if (m_terrianClearTimer.UpdateTimer(arg_timeScale.GetTimeScale()) || isAllCoinEmit)Init("", arg_collisionMgr);
}

void StageMgr::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	std::vector<Matrix>coinBlockTransformArray;
	std::vector<Matrix>emptyCoinBlockTransformArray;

	for (int y = 0; y < m_blockNum.y; ++y)
	{
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			auto& block = m_terrianBlockArray[y][x];

			//ブロックなし
			if (block == nullptr)continue;

			//スロットブロック
			if (block->GetType() == Block::SLOT)
			{
				DrawFunc3D::DrawNonShadingModel(m_slotBlockModel, block->m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
			}
			//コインブロック
			if (block->GetType() == Block::COIN)
			{
				//コイン既に排出済か
				auto coinBlock = std::dynamic_pointer_cast<CoinBlock>(block);
				coinBlockTransformArray.emplace_back(block->m_transform.GetMat());
			}
			block->Draw(block->m_transform, arg_lightMgr, arg_cam);
		}
	}

	//コインブロックをインスタンシング描画
	if (!coinBlockTransformArray.empty())
		DrawFunc3D::DrawNonShadingModel(m_coinBlockModel, coinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
	if (!emptyCoinBlockTransformArray.empty())
		DrawFunc3D::DrawNonShadingModel(m_emptyCoinBlockModel, emptyCoinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
}

void StageMgr::Finalize(std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//何もないならスルー
			if (block == nullptr)continue;
			DisappearBlock(block, arg_collisionMgr);
		}
	}

	//ヒットエフェクト初期化
	m_hitEffect->Init();
}

void StageMgr::EffectDraw(std::weak_ptr<Camera> arg_cam)
{
	m_hitEffect->Draw(arg_cam);

	static float startRadian = Angle::ConvertToRadian(-90);
	DrawFunc2D::DrawRadialWipeGraph2D(
		m_terrianClearTimerGaugePos,
		m_terrianClearTimerGaugeExt,
		startRadian,
		startRadian + Angle::ConvertToRadian(-360.0f * m_terrianClearTimer.GetInverseTimeRate()),
		{ 0.5f,0.5f },
		m_terrianClearTimerGauge
	);
}

#include"imguiApp.h"
void StageMgr::ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	ImGui::Begin("StageMgr");

	ImGui::Text("Norma : { %d }", m_norma);

	ImGui::Separator();

	ImGui::Checkbox("InfinityMode", &m_isInfinity);

	bool changeRate = ImGui::DragFloat("BlockGenerateRate", &m_generateBlockRate, 0.5f, 0.0f, 100.0f);

	bool changeX = ImGui::DragInt("BlockNumX", &m_blockNum.x);
	bool changeY = ImGui::DragInt("BlockNumY", &m_blockNum.y);

	if (m_blockNum.x < 1)m_blockNum.x = 1;
	if (m_blockNum.y < 1)m_blockNum.y = 1;

	if (changeRate || changeX || changeY)Init("", arg_collisionMgr);

	ImGui::End();
}
