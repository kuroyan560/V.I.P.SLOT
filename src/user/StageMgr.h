#pragma once
#include<string>
#include<vector>
#include<array>
#include<memory>
#include"Vec.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Timer.h"
class SlotBlock;
class CoinBlock;
class SlotMachine;
class LightManager;
class Camera;
class Model;
class Collider;
class CollisionManager;
class TexHitEffect;
class TimeScale;
class TextureBuffer;

class StageMgr
{
	//�X���b�g�}�V���Q��
	std::weak_ptr<SlotMachine>m_slotMachine;

	//�����Ԃɑ��݂���X���b�g�u���b�N�̍ő吔
	static const int SLOT_BLOCK_MAX = 4;
	//�X���b�g�u���b�N�C���X�^���X
	std::array<std::shared_ptr<SlotBlock>, SLOT_BLOCK_MAX>m_slotBlocks;
	std::shared_ptr<Model>m_slotBlockModel;

	//�R�C���u���b�N�C���X�^���X
	std::vector<std::shared_ptr<CoinBlock>>m_coinBlocks;
	std::shared_ptr<Model>m_coinBlockModel;
	std::shared_ptr<Model>m_emptyCoinBlockModel;

	//�R�C���m���}
	int m_norma;

	//�q�b�g�G�t�F�N�g
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//�R���C�_�[�C���X�^���X
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//�c���u���b�N��
	Vec2<int>m_blockNum = { 20,20 };

	//�n�`���`���u���b�N�z��i�Q�[�����Ŏ��ۂɐݒu����Ă���u���b�N�j
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

	//�u���b�N�����̊m��
	float m_generateBlockRate = 45.0f;

	//�n�`�N���A���Ԍv��
	Timer m_terrianClearTimer;
	//�n�`�N���A���ԃQ�[�W�摜
	std::shared_ptr<TextureBuffer>m_terrianClearTimerGauge;
	//�n�`�N���A���ԃQ�[�W�ʒu
	Vec2<float>m_terrianClearTimerGaugePos = { 640.0f,70.0f };
	//�n�`�N���A���ԃQ�[�W�g�嗦
	Vec2<float>m_terrianClearTimerGaugeExt = { 1.0f,1.0f };

	void DisappearBlock(std::shared_ptr<Block>& arg_block, std::weak_ptr<CollisionManager>arg_collisionMgr);

public:
	StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine);
	void Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);
	void Finalize();

	void EffectDraw(std::weak_ptr<Camera>arg_cam);

	void ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr);

	//�N���A������
	bool IsClear(const int& arg_playersCoinNum)
	{
		return m_norma <= arg_playersCoinNum;
	}
};