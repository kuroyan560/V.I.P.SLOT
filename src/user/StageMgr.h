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
	//スロットマシン参照
	std::weak_ptr<SlotMachine>m_slotMachine;

	//同時間に存在するスロットブロックの最大数
	static const int SLOT_BLOCK_MAX = 4;
	//スロットブロックインスタンス
	std::array<std::shared_ptr<SlotBlock>, SLOT_BLOCK_MAX>m_slotBlocks;
	std::shared_ptr<Model>m_slotBlockModel;

	//コインブロックインスタンス
	std::vector<std::shared_ptr<CoinBlock>>m_coinBlocks;
	std::shared_ptr<Model>m_coinBlockModel;
	std::shared_ptr<Model>m_emptyCoinBlockModel;

	//足場インスタンス
	std::vector<std::shared_ptr<Scaffold>>m_scaffolds;

	//コインノルマ
	int m_norma;
	//デバッグ用、ノルマ達成判定を切る
	bool m_isInfinity = true;
	//デバッグ用、地形生成するか
	bool m_generateTerrian = false;

	//ヒットエフェクト
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//縦横ブロック数
	Vec2<int>m_blockNum = { 10,10 };

	//地形を形作るブロック配列（ゲーム内で実際に設置されているブロック）
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

	//ブロック生成の確立
	float m_generateBlockRate = 45.0f;

	//足場配列
	std::vector<std::shared_ptr<Scaffold>>m_scaffoldArray;

	/*--- 地形評価 ---*/
	//地形クリア時間計測
	Timer m_terrianValuationTimer;
	//地形クリア時間ゲージ画像
	std::shared_ptr<TextureBuffer>m_terrianValuationTimerGaugeTex;
	//地形クリア時間ゲージ位置
	Vec2<float>m_terrianValuationTimerGaugePos = { 640.0f,70.0f };
	//地形クリア時間ゲージ拡大率
	Vec2<float>m_terrianValuationTimerGaugeExt = { 1.0f,1.0f };

	//地形評価
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
		//評価文字列画像
		std::shared_ptr<TextureBuffer>m_strTex;
		//色
		Color m_color;
		//数字画像
		std::shared_ptr<TextureBuffer>m_numTex;
	};
	std::array<TerrianEvaluation, TERRIAN_EVALUATION::NUM>m_terrianEvaluationArray;
	
	//地形評価UI演出＆描画
	struct TerrianEvaluationUI
	{
		//地形評価
		const TerrianEvaluation* m_evaluation = nullptr;

		//登場時間
		Timer m_appearTimer;
		//待機時間
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

	//地形評価の「＋」画像
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

	//クリアしたか
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};