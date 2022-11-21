#pragma once
#include<string>
#include<vector>
#include<array>
#include<memory>
#include"Vec.h"
#include"ConstParameters.h"
#include"Block.h"
#include"Timer.h"
#include<forward_list>
#include"Color.h"
#include"Scaffold.h"
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
class Scaffold;
class Player;

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

	//����C���X�^���X
	std::vector<std::shared_ptr<Scaffold>>m_scaffolds;

	//�R�C���m���}
	int m_norma;
	//�f�o�b�O�p�A�m���}�B�������؂�
	bool m_isInfinity = true;
	//�f�o�b�O�p�A�n�`�������邩
	bool m_generateTerrian = false;

	//�q�b�g�G�t�F�N�g
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//�c���u���b�N��
	Vec2<int>m_blockNum = { 10,10 };

	//�n�`���`���u���b�N�z��i�Q�[�����Ŏ��ۂɐݒu����Ă���u���b�N�j
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

	//�u���b�N�����̊m��
	float m_generateBlockRate = 45.0f;

	//����z��
	std::vector<std::shared_ptr<Scaffold>>m_scaffoldArray;

	/*--- �n�`�]�� ---*/
	//�n�`�N���A���Ԍv��
	Timer m_terrianValuationTimer;
	//�n�`�N���A���ԃQ�[�W�摜
	std::shared_ptr<TextureBuffer>m_terrianValuationTimerGaugeTex;
	//�n�`�N���A���ԃQ�[�W�ʒu
	Vec2<float>m_terrianValuationTimerGaugePos = { 640.0f,70.0f };
	//�n�`�N���A���ԃQ�[�W�g�嗦
	Vec2<float>m_terrianValuationTimerGaugeExt = { 1.0f,1.0f };

	//�n�`�]��
	enum TERRIAN_EVALUATION
	{
		FAILED,
		BAD,
		GOOD,
		GREAT,
		EXCELLENT,
		NUM
	};
	struct TerrianEvaluation
	{
		//�]��������摜
		std::shared_ptr<TextureBuffer>m_strTex;
		//�F
		Color m_color;
		//�����摜
		std::shared_ptr<TextureBuffer>m_numTex;
	};
	std::array<TerrianEvaluation, TERRIAN_EVALUATION::NUM>m_terrianEvaluationArray;
	
	//�n�`�]��UI���o���`��
	struct TerrianEvaluationUI
	{
		//�n�`�]��
		const TerrianEvaluation* m_evaluation = nullptr;

		//�o�ꎞ��
		Timer m_appearTimer;
		//�ҋ@����
		Timer m_waitTimer;

		void Init()
		{
			m_evaluation = nullptr;
		}
		void Emit(TerrianEvaluation* arg_evaluation, float arg_appearTime,float arg_waitTime)
		{
			m_evaluation = arg_evaluation;
			m_appearTimer.Reset(arg_appearTime);
			m_waitTimer.Reset(arg_waitTime);
		}
		void Update(const float& arg_timeScale);
		void Draw(const Vec2<float>& arg_timeGaugeCenter, std::weak_ptr<TextureBuffer>arg_plusTex);
	}m_terrianEvaluationUI;

	//�n�`�]���́u�{�v�摜
	std::shared_ptr<TextureBuffer>m_terrianValuationPlusTex;

	void DisappearBlock(std::shared_ptr<Block>& arg_block, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void GenerateTerrian(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr, int arg_slotBlockNum);
public:
	StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine);
	void Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);
	void Finalize(std::weak_ptr<CollisionManager>arg_collisionMgr);

	void Draw2D(std::weak_ptr<Camera>arg_cam);

	void ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr);

	//�N���A������
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};