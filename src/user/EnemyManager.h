#pragma once
#include<vector>
#include<array>
#include<memory>
#include"Singleton.h"
#include"EnemyBreed.h"
#include"Transform.h"
#include"Collider.h"
class Enemy;
class GraphicsPipeline;
class StructuredBuffer;
class Camera;
class CubeMap;
class Player;

//エネミー種別
const enum ENEMY_TYPE { SANDBAG, ENEMY_TYPE_NUM };

class EnemyManager : public Singleton<EnemyManager>
{
private:
	friend class Singleton<EnemyManager>;
	EnemyManager();
	typedef std::vector<std::shared_ptr<Enemy>> EnemyArray;

private:
	//各種別ごとに生成できる最大数
	static const int s_maxNum = 100;
	//各モデルのボーン最大数
	static const int s_maxBoneNum = 32;
	//描画パイプライン（インスタンシング描画）
	std::shared_ptr<GraphicsPipeline>m_pipeline;

	//系統（型オブジェクト）
	std::array<std::unique_ptr<EnemyBreed>, ENEMY_TYPE_NUM>m_breeds;
	//生成済エネミー配列
	std::array<EnemyArray, ENEMY_TYPE_NUM>m_enemys;
	//ワールド行列配列用構造化バッファ
	std::array<std::shared_ptr<StructuredBuffer>, ENEMY_TYPE_NUM>m_worldMatriciesBuff;
	//ボーン行列配列用構造化バッファ
	std::array<std::shared_ptr<StructuredBuffer>, ENEMY_TYPE_NUM>m_boneMatriciesBuff;

public:
	//生成済エネミークリア
	void Clear();
	//初期化
	void Init();
	//更新
	void Update(const Player& Player, const float& Gravity);
	//描画
	void Draw(Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap);

	//生成
	void Spawn(const ENEMY_TYPE& Type, const Transform& InitTransform);

	//モデル取得
	std::shared_ptr<Model>GetModel(const ENEMY_TYPE& Type)
	{
		return m_breeds[Type]->GetModel();
	}
};