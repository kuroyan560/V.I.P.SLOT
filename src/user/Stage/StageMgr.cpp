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
	//�A�^�b�`����Ă����R���C�_�[����
	arg_collisionMgr.lock()->Remove(arg_block->GetCollider());
}

void StageMgr::GenerateTerrian(std::string arg_stageDataPath, std::weak_ptr<CollisionManager> arg_collisionMgr, int arg_slotBlockNum)
{
	//���ꐶ��
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

	//�u���b�N������삦�Ȃ��悤�ɂ���
	if (MAX_BLOCK_NUM_AXIS.x < m_blockNum.x)m_blockNum.x = MAX_BLOCK_NUM_AXIS.x;
	if (MAX_BLOCK_NUM_AXIS.y < m_blockNum.y)m_blockNum.y = MAX_BLOCK_NUM_AXIS.y;

	//�n�`�N���A
	for (auto& terrianArray : m_terrianBlockArray)
		for (auto& t : terrianArray)
		{
			//�u���b�N�Ȃ�
			if (t == nullptr)continue;

			DisappearBlock(t, arg_collisionMgr);
		}
	m_terrianBlockArray.clear();

	//�n�`�\��
	Vec2<float>scale = { 1.0f,1.0f };
	scale.y = (FIELD_HEIGHT_MAX - FIELD_HEIGHT_MIN) / (BLOCK_LEN * m_blockNum.y);
	scale.x = FIELD_WIDTH / (BLOCK_LEN * m_blockNum.x);

	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒���
	Vec2<float>sideFixed = { BLOCK_LEN * scale.x ,BLOCK_LEN * scale.y };
	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒�������
	Vec2<float> sideFixedHalf = sideFixed / 2.0f;
	//�u���b�N���m�̍��W�I�t�Z�b�g
	Vec2<float>offset = sideFixed;

	const float leftX = -((floor(m_blockNum.x / 2.0f) - 1) * sideFixed.x) + (-sideFixedHalf.x * (m_blockNum.x % 2 == 0 ? 1 : 2));
	const float topY = FIELD_HEIGHT_MAX - sideFixedHalf.y;

	Transform initTransform;
	initTransform.SetScale({ scale.x,scale.y,1.0f });

	//���ȉ��A�{���Ȃ�t�@�C������ǂݍ���
		//�X���b�g���[������
		//m_slotMachine.lock()->ReelSet(SlotMachine::LEFT, );
		//m_slotMachine.lock()->ReelSet(SlotMachine::CENTER, );
		//m_slotMachine.lock()->ReelSet(SlotMachine::RIGHT, );

	//�X���b�g�u���b�N�ʒu
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

			//�u���b�N�������A�R���C�_�[�A�^�b�`
			arg_collisionMgr.lock()->Register(block->GetCollider());
			block->Init(initTransform, m_hitEffect);
		}
	}

	//�n�`�N���A���Ԑݒ�
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
	
	//���f���ǂݍ���
	m_slotBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	m_coinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
	m_emptyCoinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");

	//�q�b�g�G�t�F�N�g����
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3, false);

	//�u���b�N�̃R���C�_�[�p�v���~�e�B�u�z��
	Vec3<ValueMinMax>val;
	val.x.m_min = -BLOCK_LEN_HALF;
	val.x.m_max = BLOCK_LEN_HALF;
	val.y.m_min = -BLOCK_LEN_HALF;
	val.y.m_max = BLOCK_LEN_HALF;
	val.z.m_min = -BLOCK_LEN_HALF;
	val.z.m_max = BLOCK_LEN_HALF;

	//�u���b�N�̃R���C�_�[����
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

	//�u���b�N�̃C���X�^���X����
	for (int i = 0; i < MAX_BLOCK_NUM; ++i)
	{
		m_coinBlocks.emplace_back(std::make_shared<CoinBlock>(originCol));
	}
	for (auto& b : m_slotBlocks)
	{
		b = std::make_shared<SlotBlock>(originCol, arg_slotMachine);
	}

	//����̃C���X�^���X����
	for (int i = 0; i < MAX_SCAFFOLD_NUM; ++i)
	{
		m_scaffolds.emplace_back(std::make_shared<Scaffold>());
	}

	/*--- �n�`�]�� ---*/
	const std::string terrianValuationTexDir = "resource/user/img/timeGauge/";
	//�n�`�N���A���ԃQ�[�W�摜
	m_terrianValuationTimerGaugeTex = app->GenerateTextureBuffer(terrianValuationTexDir + "terrianTimeGauge.png");

	//�]��������摜���O
	std::array<std::string, TERRIAN_EVALUATION::NUM>terrianEvaluationTexName =
	{
		"failed","bad","good","great","excellent"
	};
	//�n�`�]�����Ƃ̐F
	std::array<Color, TERRIAN_EVALUATION::NUM>terrianEvaluationColor =
	{
		Color(93,71,118,255),
		Color(202,96,174,255),
		Color(141,216,148,255),
		Color(255,93,204,255),
		Color(94,253,247,255),
	};
	//�����摜
	std::array<std::shared_ptr<TextureBuffer>, TERRIAN_EVALUATION::NUM>terrianEvaluationNumTex;
	app->GenerateTextureBuffer(terrianEvaluationNumTex.data(), terrianValuationTexDir + "slotBlockNum.png", 5, { 5,1 });
	//�e�\���̂Ƀp�����[�^�Z�b�g
	for (int i = 0; i < TERRIAN_EVALUATION::NUM; ++i)
	{
		m_terrianEvaluationArray[i].m_strTex = app->GenerateTextureBuffer(terrianValuationTexDir + terrianEvaluationTexName[i] + ".png");
		m_terrianEvaluationArray[i].m_color = terrianEvaluationColor[i];
		m_terrianEvaluationArray[i].m_numTex = terrianEvaluationNumTex[i];
	}
	//�n�`�]���́u�{�v�摜
	m_terrianValuationPlusTex = app->GenerateTextureBuffer(terrianValuationTexDir + "plus.png");
}

void StageMgr::Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	GenerateTerrian(arg_stageDataPath, arg_collisionMgr, 4);

	//�n�`�]��UI
	m_terrianEvaluationUI.Init();

	//�R���W�����}�l�[�W���̃|�C���^�ێ�
	m_collisionMgr = arg_collisionMgr;
}

void StageMgr::Update(TimeScale& arg_timeScale, std::weak_ptr<Player>arg_player)
{
	//���ׂẴR�C���r�o�ς�
	bool isAllCoinEmit = true;

	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//�u���b�N�Ȃ�
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

	//�q�b�g�G�t�F�N�g
	m_hitEffect->Update(arg_timeScale.GetTimeScale());

	//�n�`�]��UI
	m_terrianEvaluationUI.Update(arg_timeScale.GetTimeScale());

	//�f�o�b�O�p
	bool terrianClearKey = UsersInput::Instance()->KeyOnTrigger(DIK_T);

	//�n�`�N���A���Ԍo�߂��A�S�R�C���r�o�Œn�`�ύX
	if (m_terrianValuationTimer.UpdateTimer(arg_timeScale.GetTimeScale()) || isAllCoinEmit || terrianClearKey)
	{
		//�n�`�]��
		int evaluation = (int)std::ceil((1.0f - m_terrianValuationTimer.GetTimeRate()) / (1.0f / (TERRIAN_EVALUATION::NUM - 1)));
		m_terrianEvaluationUI.Emit(
			&m_terrianEvaluationArray[evaluation],
			60.0f, 35.0f);

		GenerateTerrian("", m_collisionMgr,evaluation);
	}

	//����Ƃ̔���
	for (auto& scaffold : m_scaffoldArray)
	{
		arg_player.lock()->HitCheckWithScaffold(scaffold);
	}
}

#include"BasicDraw.h"
void StageMgr::BlockDraw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//�u���b�N�`��
	std::vector<Matrix>coinBlockTransformArray;

	for (auto& blockArray : m_terrianBlockArray)
	{
		for (auto& block : blockArray)
		{
			//�u���b�N�Ȃ�
			if (block == nullptr)continue;

			//�X���b�g�u���b�N
			if (block->GetType() == Block::SLOT)
			{
				BasicDraw::Draw(*arg_lightMgr.lock(), m_slotBlockModel, block->m_transform, *arg_cam.lock());
			}
			//�R�C���u���b�N
			if (block->GetType() == Block::COIN)
			{
				//�R�C�����ɔr�o�ς�
				auto coinBlock = std::dynamic_pointer_cast<CoinBlock>(block);
				coinBlockTransformArray.emplace_back(block->m_transform.GetWorldMat());

				BasicDraw::Draw(*arg_lightMgr.lock(), m_coinBlockModel, block->m_transform, *arg_cam.lock());
			}
			block->Draw(block->m_transform, arg_lightMgr, arg_cam);
		}
	}

	//�R�C���u���b�N���C���X�^���V���O�`��
	//if (!coinBlockTransformArray.empty())
		//DrawFunc3D::DrawNonShadingModel(m_coinBlockModel, coinBlockTransformArray, *arg_cam.lock(), AlphaBlendMode_None);
}

void StageMgr::ScaffoldDraw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//����`��
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
			//�����Ȃ��Ȃ�X���[
			if (block == nullptr)continue;
			DisappearBlock(block, m_collisionMgr);
		}
	}

	//�q�b�g�G�t�F�N�g������
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

		//�n�`�]��UI
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
