#include "StageMgr.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Transform.h"
#include"Importer.h"
#include"DrawFunc3D.h"
#include"Collider.h"
#include"CollisionManager.h"

StageMgr::StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine)
{
	using namespace ConstParameter::Stage;

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

	//�u���b�N�̃R���C�_�[�p�v���~�e�B�u�z��
	Vec3<ValueMinMax>val;
	val.x.m_min = -BLOCK_LEN_HALF;
	val.x.m_max = BLOCK_LEN_HALF;
	val.y.m_min = -BLOCK_LEN_HALF;
	val.y.m_max = BLOCK_LEN_HALF;
	val.z.m_min = -BLOCK_LEN_HALF;
	val.z.m_max = BLOCK_LEN_HALF;
	for (int i = 0; i < MAX_BLOCK_NUM; ++i)
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		//colPrimitiveArray.emplace_back(std::make_shared<CollisionAABB>(val));
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0, 0, 0)));

		m_coinBlocks.emplace_back(std::make_shared<CoinBlock>());
		m_colliders.emplace_back(std::make_shared<Collider>());
		m_colliders.back()->Generate("BlockCollider", "Block", colPrimitiveArray);
	}
}

void StageMgr::Init(std::string arg_mapFilePath, std::weak_ptr<CollisionManager>arg_collisionMgr)
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

			//�A�^�b�`����Ă����R���C�_�[����
			arg_collisionMgr.lock()->Remove(t->GetCollider());
			t->GetCollider()->SetCallBack("Player",nullptr);
			t->GetCollider()->SetParentTransform(nullptr);
		}
	m_terrianBlockArray.clear();

	//�n�`�\��
	Vec2<float>scale = { 1.0f,1.0f };
	scale.y = (FIELD_HEIGHT - FLOOR_HEIGHT) / (BLOCK_LEN * m_blockNum.y);
	scale.x = FIELD_WIDTH / (BLOCK_LEN * m_blockNum.x);

	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒���
	Vec2<float>sideFixed = { BLOCK_LEN * scale.x ,BLOCK_LEN * scale.y };
	//�X�P�[���K�p��̃u���b�N�̈�ӂ̒�������
	Vec2<float> sideFixedHalf = sideFixed / 2.0f;
	//�u���b�N���m�̍��W�I�t�Z�b�g
	Vec2<float>offset = sideFixed;

	const float leftX = -((floor(m_blockNum.x / 2.0f) - 1) * sideFixed.x) + (-sideFixedHalf.x * (m_blockNum.x % 2 == 0 ? 1 : 2));
	const float topY = FIELD_HEIGHT - sideFixedHalf.y;

	Transform initTransform;
	initTransform.SetScale({ scale.x,scale.y,1.0f });

	//���{���Ȃ�t�@�C������n�`�ǂݍ���

	int coinBlockIdx = 0;
	int slotBlockIdx = 0;
	int colliderIdx = 0;

	m_terrianBlockArray.resize(m_blockNum.y);
	for (int y = 0; y < m_blockNum.y; ++y)
	{
		m_terrianBlockArray[y].resize(m_blockNum.x);
		for (int x = 0; x < m_blockNum.x; ++x)
		{
			if (!KuroFunc::Probability(45.0f))continue;

			auto& block = m_terrianBlockArray[y][x];
			initTransform.SetPos({ leftX + x * offset.x,topY - y * offset.y,FIELD_DEPTH_FIXED });
			block = std::dynamic_pointer_cast<Block>(m_coinBlocks[coinBlockIdx++]);

			//�u���b�N�������A�R���C�_�[�A�^�b�`
			arg_collisionMgr.lock()->Register(m_colliders[colliderIdx]);
			block->Init(initTransform, m_colliders[colliderIdx++]);
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
				if (coinBlock->IsEmpty())emptyCoinBlockTransformArray.emplace_back(block->m_transform.GetMat());
				else coinBlockTransformArray.emplace_back(block->m_transform.GetMat());
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

#include"imguiApp.h"
void StageMgr::ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	ImGui::Begin("StageMgr");

	bool changeX = ImGui::DragInt("BlockNumX", &m_blockNum.x);
	bool changeY = ImGui::DragInt("BlockNumY", &m_blockNum.y);

	if (m_blockNum.x < 1)m_blockNum.x = 1;
	if (m_blockNum.y < 1)m_blockNum.y = 1;

	if (changeX || changeY)Init("", arg_collisionMgr);

	ImGui::End();
}
