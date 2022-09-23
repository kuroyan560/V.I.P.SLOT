#pragma once
#include"Transform.h"
#include"Singleton.h"
#include<memory>
#include<array>
#include<forward_list>
#include"TerrianParts.h"

const enum TERRIAN_PARTS { SINGLE_ACT_POINT, TERRIAN_PARTS_NUM };
class LightManager;
class Camera;
class CubeMap;

class TerrianManager : public Singleton<TerrianManager>
{
private:
	friend class Singleton<TerrianManager>;
	TerrianManager();

private:
	//地形パーツの型オブジェクト
	std::array<std::unique_ptr<TerrianPartsType>, TERRIAN_PARTS_NUM>m_stagePartsTypeArray;
	//生成済地形配列
	std::forward_list<std::shared_ptr<TerrianParts>>m_terrians;

public:
	//パーツを設置
	void Spawn(const TERRIAN_PARTS& PartsType, const Transform& InitTransform);
	//生成済地形をクリア
	void Clear();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(LightManager& LigManager, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap = nullptr);
};