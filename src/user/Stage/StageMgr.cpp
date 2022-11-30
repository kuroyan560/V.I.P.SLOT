#include "StageMgr.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Transform.h"
#include"Importer.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TexHitEffect.h"
#include"TimeScale.h"
#include"DrawFunc2D.h"
#include<vector>
#include"SlotMachine.h"
#include"KuroEngine.h"
#include"Player.h"
#include"Sprite.h"

void StageMgr::DisappearBlock(std::shared_ptr<Block>& arg_block, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//アタッチされていたコライダー解除
	arg_collisionMgr.lock()->Remove(arg_block->GetCollider());
}

void StageMgr::GenerateTerrian(std::string arg_stageDataPath, std::weak_ptr<CollisionManager> arg_collisionMgr, int arg_slotBlockNum)
{
	//足場生成
	m_scaffoldArray.clear();

	int scaffoldNum = 4;
	float offsetY = 4.0f;
	for (int i = 0; i < scaffoldNum; ++i)
	{
		m_scaffoldArray.emplace_back(m_scaffolds[i]);
		m_scaffoldArray.back()->Init(0.0f, 5.2f * (i + 1), ConstParameter::Environment::FIELD_FLOOR_SIZE.x);
	}

	if (!m_generateTerrian)return;

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

	//スロットブロック位置
	std::vector<Vec2<int>>slotBlockRandIdx(arg_slotBlockNum);
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
			initTransform.SetPos({ leftX + x * offset.x,topY - y * offset.y,FIELD_FLOOR_POS.z });

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
	m_terrianValuationTimer.Reset(600.0f);
}

void StageMgr::OnImguiItems()
{
	if (ImGui::Checkbox("GenerateTerrian", &m_generateTerrian))
	{
		Finalize();
		Init("", m_collisionMgr);
	}

	bool changeRate = ImGui::DragFloat("BlockGenerateRate", &m_generateBlockRate, 0.5f, 0.0f, 100.0f);

	bool changeX = ImGui::DragInt("BlockNumX", &m_blockNum.x);
	bool changeY = ImGui::DragInt("BlockNumY", &m_blockNum.y);

	if (m_blockNum.x < 1)m_blockNum.x = 1;
	if (m_blockNum.y < 1)m_blockNum.y = 1;

	if (changeRate || changeX || changeY)
	{
		Finalize();
		Init("", m_collisionMgr);
	}
}

StageMgr::StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine) :Debugger("StageMgr")
{
	using namespace ConstParameter::Stage;

	auto app = D3D12App::Instance();

	m_slotMachine = arg_slotMachine;
	
	//モデル読み込み
	m_slotBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	m_coinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
	m_emptyCoinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");

	//ヒットエフェクト生成
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3, false);

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
	originCol->Generate("BlockCollider", { "Block" }, colPrimitiveArray);

	//ブロックのインスタンス生成
	for (int i = 0; i < MAX_BLOCK_NUM; ++i)
	{
		m_coinBlocks.emplace_back(std::make_shared<CoinBlock>(originCol));
	}
	for (auto& b : m_slotBlocks)
	{
		b = std::make_shared<SlotBlock>(originCol, arg_slotMachine);
	}

	//足場のインスタンス生成
	for (int i = 0; i < MAX_SCAFFOLD_NUM; ++i)
	{
		m_scaffolds.emplace_back(std::make_shared<Scaffold>());
	}

	/*--- 地形評価 ---*/
	const std::string terrianValuationTexDir = "resource/user/img/timeGauge/";
	//地形クリア時間ゲージ画像
	m_terrianValuationTimerGaugeTex = app->GenerateTextureBuffer(terrianValuationTexDir + "terrianTimeGauge.png");

	//評価文字列画像名前
	std::array<std::string, TERRIAN_EVALUATION::NUM>terrianEvaluationTexName =
	{
		"failed","bad","good","great","excellent"
	};
	//地形評価ごとの色
	std::array<Color, TERRIAN_EVALUATION::NUM>terrianEvaluationColor =
	{
		Color(93,71,118,255),
		Color(202,96,174,255),
		Color(141,216,148,255),
		Color(255,93,204,255),
		Color(94,253,247,255),
	};
	//数字画像
	std::array<std::shared_ptr<TextureBuffer>, TERRIAN_EVALUATION::NUM>terrianEvaluationNumTex;
	app->GenerateTextureBuffer(terrianEvaluationNumTex.data(), terrianValuationTexDir + "slotBlockNum.png", 5, { 5,1 });
	//各構造体にパラメータセット
	for (int i = 0; i < TERRIAN_EVALUATION::NUM; ++i)
	{
		m_terrianEvaluationArray[i].m_strTex = app->GenerateTextureBuffer(terrianValuationTexDir + terrianEvaluationTexName[i] + ".png");
		m_terrianEvaluationArray[i].m_color = terrianEvaluationColor[i];
		m_terrianEvaluationArray[i].m_numTex = terrianEvaluationNumTex[i];
	}
	//地形評価の「＋」画像
	m_terrianValuationPlusTex = app->GenerateTextureBuffer(terrianValuationTexDir + "plus.png");
}

void StageMgr::Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	GenerateTerrian(arg_stageDataPath, arg_collisionMgr, 4);

	//地形評価UI
	m_terrianEvaluationUI.Init();

	//コリジョンマネージャのポインタ保持
	m_collisionMgr = arg_collisionMgr;
}

void StageMgr::Update(TimeScale& arg_timeScale, std::weak_ptr<Player>arg_player)
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
				DisappearBlock(block, m_collisionMgr);
				block = nullptr;
			}
		}
	}

	//ヒットエフェクト
	m_hitEffect->Update(arg_timeScale.GetTimeScale());

	//地形評価UI
	m_terrianEvaluationUI.Update(arg_timeScale.GetTimeScale());

	//デバッグ用
	bool terrianClearKey = UsersInput::Instance()->KeyOnTrigger(DIK_T);

	//地形クリア時間経過か、全コイン排出で地形変更
	if (m_terrianValuationTimer.UpdateTimer(arg_timeScale.GetTimeScale()) || isAllCoinEmit || terrianClearKey)
	{
		//地形評価
		int evaluation = (int)std::ceil((1.0f - m_terrianValuationTimer.GetTimeRate()) / (1.0f / (TERRIAN_EVALUATION::NUM - 1)));
		m_terrianEvaluationUI.Emit(
			&m_terrianEvaluationArray[evaluation],
			60.0f, 35.0f);

		GenerateTerrian("", m_collisionMgr,evaluation);
	}

	//足場との判定
	for (auto& scaffold : m_scaffoldArray)
	{
		arg_player.lock()->HitCheckWithScaffold(scaffold);
	}
}

#include"BasicDraw.h"
void StageMgr::BlockDraw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//ブロック描画
	std::vector<Matrix>coinBlockTransformArray;

	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//ブロックなし
			if (block == nullptr)continue;

			//スロットブロック
			if (block->GetType() == Block::SLOT)
			{
				BasicDraw::Draw(*arg_lightMgr.lock(), m_slotBlockModel, block->m_transform, *arg_cam.lock());
			}
			//コインブロック
			if (block->GetType() == Block::COIN)
			{
				//コイン既に排出済か
				auto coinBlock = std::dynamic_pointer_cast<CoinBlock>(block);
				coinBlockTransformArray.emplace_back(block->m_transform.GetWorldMat());

				BasicDraw::Draw(*arg_lightMgr.lock(), m_coinBlockModel, block->m_transform, *arg_cam.lock());
			}
			block->Draw(block->m_transform, arg_lightMgr, arg_cam);
		}
	}

	//コインブロックをインスタンシング描画
	//if (!coinBlockTransformArray.empty())
		//DrawFunc3D::DrawNonShadingModel(m_coinBlockModel, coinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
}

void StageMgr::ScaffoldDraw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//足場描画
	for (auto& scaffold : m_scaffoldArray)
	{
		scaffold->Draw(arg_lightMgr, arg_cam);
	}
}

void StageMgr::Finalize()
{
	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//何もないならスルー
			if (block == nullptr)continue;
			DisappearBlock(block, m_collisionMgr);
		}
	}

	//ヒットエフェクト初期化
	m_hitEffect->Init();
}

void StageMgr::Draw2D(std::weak_ptr<Camera> arg_cam)
{
	m_hitEffect->Draw(arg_cam);

	if (m_generateTerrian)
	{
		static float startRadian = Angle::ConvertToRadian(-90);
		DrawFunc2D::DrawRadialWipeGraph2D(
			m_terrianValuationTimerGaugePos,
			m_terrianValuationTimerGaugeExt,
			startRadian,
			startRadian + Angle::ConvertToRadian(-360.0f * m_terrianValuationTimer.GetInverseTimeRate()),
			{ 0.5f,0.5f },
			m_terrianValuationTimerGaugeTex
		);

		//地形評価UI
		m_terrianEvaluationUI.Draw(m_terrianValuationTimerGaugePos, m_terrianValuationPlusTex);
	}
}

void StageMgr::TerrianEvaluationUI::Update(const float& arg_timeScale)
{
	if (m_evaluation == nullptr)return;

	if (m_appearTimer.UpdateTimer(arg_timeScale))
	{
		if (m_waitTimer.UpdateTimer(arg_timeScale))
		{
			m_evaluation = nullptr;
		}
	}
}

#include"DrawFunc2D_Color.h"
void StageMgr::TerrianEvaluationUI::Draw(const Vec2<float>& arg_timeGaugeCenter, std::weak_ptr<TextureBuffer>arg_plusTex)
{
	if (m_evaluation == nullptr)return;

	float rate = m_appearTimer.GetTimeRate();
	Vec2<float>offset = { 0,0 };
	offset.y = KuroMath::Ease(Out, Elastic, rate, 0.0f, 95.0f);
	DrawFunc2D::DrawRotaGraph2D(arg_timeGaugeCenter + offset, { 1,1 }, 0.0f, m_evaluation->m_strTex);

	Vec2<float>plusOffset = { -15.0f,40.0f };
	DrawFunc2D_Color::DrawRotaGraph2D(
		arg_timeGaugeCenter + offset + plusOffset,
		{ 1,1 },
		0.0f,
		arg_plusTex.lock(),
		m_evaluation->m_color);

	Vec2<float>numOffset = Vec2<float>(-plusOffset.x, plusOffset.y);
	DrawFunc2D_Color::DrawRotaGraph2D(
		arg_timeGaugeCenter + offset + numOffset,
		{ 1,1 },
		0.0f,
		m_evaluation->m_numTex,
		m_evaluation->m_color);

}
