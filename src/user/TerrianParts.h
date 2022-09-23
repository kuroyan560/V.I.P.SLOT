#pragma once
#include<memory>
#include<vector>
#include"Transform.h"
class Model;
class ActPoint;
class Collider;
class TerrianParts;
class TerrianPartsType;

//地形パーツの挙動
class TerrianPartsControl
{
	friend class TerrianPartsType;
	virtual TerrianPartsControl* Clone() = 0;
public:
	virtual void Init(TerrianParts& Parts) {}
	virtual void Update(TerrianParts& Parts) {}
};

//地形パーツ型オブジェクト
class TerrianPartsType
{
	friend class TerrianManager;

	//モデル
	std::shared_ptr<Model>m_model;
	//コライダークローン元
	std::vector<std::shared_ptr<Collider>>m_collidersOrigin;
	//地形パーツの挙動
	std::weak_ptr<TerrianPartsControl>m_controlOrigin;
	//ActPointクローン元
	std::vector<std::shared_ptr<ActPoint>>m_actPointOrigin;

	TerrianPartsType(
		std::shared_ptr<Model>Model,
		const std::vector<std::shared_ptr<Collider>>& CollidersOrigin,
		std::shared_ptr<TerrianPartsControl> ControlOrigin = nullptr,
		std::vector<std::shared_ptr<ActPoint>>ActPointOrigin = {});
public:

	//ステージパーツインスタンス生成
	std::shared_ptr<TerrianParts>GenerateParts(const Transform& InitTransform);
};

//地形パーツ
class TerrianParts
{
	friend class TerrianPartsControl;
	friend class TerrianPartsType;
	friend class TerrianManager;
	
	const TerrianPartsType* m_typeObj;

	//トランスフォーム
	Transform m_transform;

	//コライダー
	std::vector<std::shared_ptr<Collider>>m_colliders;
	//挙動
	std::shared_ptr<TerrianPartsControl>m_control;
	//地形ActPoint
	std::vector<std::shared_ptr<ActPoint>>m_actPoints;

	//型オブジェクトからしか生成できない
	TerrianParts(const TerrianPartsType* Type) : m_typeObj(Type) {}

	void Init()
	{
		if (m_control)m_control->Init(*this);
	}
	void Update()
	{
		if (m_control)m_control->Update(*this);
	}
};