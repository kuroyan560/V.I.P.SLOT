#pragma once
#include<string>
#include<vector>
#include<array>
#include<memory>
#include"Vec.h"
class Block;
class SlotBlock;
class CoinBlock;
class SlotMachine;
class LightManager;
class Camera;
class Model;

class StageMgr
{
	//同時間に存在するスロットブロックの最大数
	static const int SLOT_BLOCK_MAX = 4;
	std::array<std::shared_ptr<SlotBlock>, SLOT_BLOCK_MAX>m_slotBlocks;
	std::shared_ptr<Model>m_slotBlockModel;

	//同時間に存在するコインブロックの最大数
	static const int COIN_BLOCK_MAX = 10000;
	std::array<std::shared_ptr<CoinBlock>, COIN_BLOCK_MAX>m_coinBlocks;
	std::shared_ptr<Model>m_coinBlockModel;
	std::shared_ptr<Model>m_emptyCoinBlockModel;

	//縦横ブロック数
	Vec2<int>m_blockNum = { 20,20 };

	//地形を形作るブロック配列（ゲーム内で実際に設置されているブロック）
	std::vector<std::vector<std::shared_ptr<Block>>>m_terrianBlockArray;

public:
	StageMgr(const std::shared_ptr<SlotMachine>& arg_slotMachine);
	void Init(std::string arg_mapFilePath);
	void Update();
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	void ImguiDebug();
};