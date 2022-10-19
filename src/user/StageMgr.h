#pragma once
#include<string>
#include<vector>
#include<array>
#include<memory>
#include"Vec.h"
#include"ConstParameters.h"
#include"Block.h"
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

class StageMgr
{
	//同時間に存在するスロットブロックの最大数
	static const int SLOT_BLOCK_MAX = 4;
	//スロットブロックインスタンス
	std::array<std::shared_ptr<SlotBlock>, SLOT_BLOCK_MAX>m_slotBlocks;
	std::shared_ptr<Model>m_slotBlockModel;

	//コインブロックインスタンス
	std::vector<std::shared_ptr<CoinBlock>>m_coinBlocks;
	std::shared_ptr<Model>m_coinBlockModel;
	std::shared_ptr<Model>m_emptyCoinBlockModel;

	//ヒットエフェクト
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//コライダーインスタンス
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//縦横ブロック数
	Vec2<int>m_blockNum = { 20,20 };

	//地形を形作るブロック配列（ゲーム内で実際に設置されているブロック）
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

	//ブロック生成の確立
	float m_generateBlockRate = 45.0f;

public:
	StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine);
	void Init(std::string arg_mapFilePath, std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(TimeScale& arg_timeScale);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);
	void EffectDraw(std::weak_ptr<Camera>arg_cam);

	void ImguiDebug(std::weak_ptr<CollisionManager>arg_collisionMgr);
};