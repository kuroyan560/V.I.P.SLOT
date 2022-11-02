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
	//�A�^�b�`����Ă����R���C�_�[����
	arg_collisionMgr.lock()->Remove(arg_block->GetCollider());
}

StageMgr::StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine)
{
	using namespace ConstParameter::Stage;

	m_slotMachine = arg_slotMachine;
	
	m_slotBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "slotBlock.glb");
	m_coinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock.glb");
	m_emptyCoinBlockModel = Importer::Instance()->LoadModel("resource/user/model/", "coinBlock_empty.glb");

	//�q�b�g�G�t�F�N�g����
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3);

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

	//�R�C���m���}
	m_norma = 10;

	//�X���b�g�|�b�v��
	int slotBlockNum = 4;

	//�X���b�g�u���b�N�ʒu
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

			//�u���b�N�������A�R���C�_�[�A�^�b�`
			arg_collisionMgr.lock()->Register(block->GetCollider());
			block->Init(initTransform, m_hitEffect);
		}
	}

	//�n�`�N���A���Ԑݒ�
	m_terrianClearTimer.Reset(600.0f);
}

void StageMgr::Update(TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
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
				DisappearBlock(block, arg_collisionMgr);
				block = nullptr;
			}
		}
	}

	m_hitEffect->Update(arg_timeScale.GetTimeScale());


	//�n�`�N���A���Ԍo�߂��A�S�R�C���r�o�Œn�`�ύX
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

			//�u���b�N�Ȃ�
			if (block == nullptr)continue;

			//�X���b�g�u���b�N
			if (block->GetType() == Block::SLOT)
			{
				DrawFunc3D::DrawNonShadingModel(m_slotBlockModel, block->m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
			}
			//�R�C���u���b�N
			if (block->GetType() == Block::COIN)
			{
				//�R�C�����ɔr�o�ς�
				auto coinBlock = std::dynamic_pointer_cast<CoinBlock>(block);
				coinBlockTransformArray.emplace_back(block->m_transform.GetMat());
			}
			block->Draw(block->m_transform, arg_lightMgr, arg_cam);
		}
	}

	//�R�C���u���b�N���C���X�^���V���O�`��
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
			//�����Ȃ��Ȃ�X���[
			if (block == nullptr)continue;
			DisappearBlock(block, arg_collisionMgr);
		}
	}

	//�q�b�g�G�t�F�N�g������
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
