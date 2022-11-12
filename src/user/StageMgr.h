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

	//コインノルマ
	int m_norma;
	//デバッグ用、ノルマ達成判定を切る
	bool m_isInfinity = true;
	//デバッグ用、地形生成するか
	bool m_generateTerrian = true;

	//ヒットエフェクト
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//縦横ブロック数
	Vec2<int>m_blockNum = { 10,10 };

	//地形を形作るブロック配列（ゲーム内で実際に設置されているブロック）
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

	//ブロック生成の確立
	float m_generateBlockRate = 45.0f;

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

	//地形評価の「＋」画像
	std::shared_ptr<TextureBuffer>m_terrianValuationPlusTex;

	void DisappearBlock(std::shared_ptr<Block>& arg_block, std::weak_ptr<CollisionManager>arg_collisionMgr);

public:
	StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine);
	void Init(std::string arg_stageDataPath, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);
	void Finalize(std::weak_ptr<CollisionManager>arg_collisionMgr);

	void EffectDraw(std::weak_ptr<Camera>arg_cam);

	void ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr);

	//クリアしたか
	bool IsClear(const int& arg_playersCoinNum)
	{
		return !m_isInfinity && m_norma <= arg_playersCoinNum;
	}
};