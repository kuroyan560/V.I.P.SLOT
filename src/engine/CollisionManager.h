#pragma once
#include<map>
#include<string>
#include<list>
#include<memory>
#include<limits>
#include<vector>
#include"Vec.h"
class Collider;
class Camera;

class CollisionManager
{
	struct RegisterInfo
	{
		//コライダーのポインタ
		std::shared_ptr<Collider>m_collider;
		//自身の振る舞い
		unsigned char m_myAttribute = 0;
	};

	//登録されたコライダーの振る舞いリスト
	std::map<std::string, unsigned char>m_attributeList;

	//登録されたコライダーリスト
	std::list<RegisterInfo>m_registerList;

	//衝突していた場合の処理
	void OnHit(const RegisterInfo& arg_myInfo, const RegisterInfo& arg_otherInfo, const Vec3<float>& arg_inter);

public:
	CollisionManager() {}
	void AddAttribute(std::string arg_attributeKey, unsigned char arg_attribute);

	void Update();
	void DebugDraw(Camera& Cam);

	//コライダーの登録
	void Register(std::string arg_attributeKey, const std::shared_ptr<Collider>& arg_collider);
	void Register(std::string arg_attributeKey, const std::vector<std::shared_ptr<Collider>>& arg_colliderArray)
	{
		for (auto& col : arg_colliderArray)Register(arg_attributeKey, col);
	}
	//コライダーの登録解除
	void Remove(const std::shared_ptr<Collider>& arg_collider);

	//コライダー振る舞いをキーから取得
	const unsigned char& GetAttribute(std::string arg_key)const;
};

