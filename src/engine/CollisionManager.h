#pragma once
#include<map>
#include<string>
#include<list>
#include<memory>
#include<limits>
class Collider;
class Camera;
class CollisionCallBack;

class CollisionManager
{
	static std::string AlmightyAttributeName() { return "Almighty"; }
	static const unsigned char ALMIGHTY_ATTRIBUTE = UCHAR_MAX;

	static std::string NoneAttributeName() { return "None"; }
	static const unsigned char NONE_ATTRIBUTE = 0;

	struct RegisterInfo
	{
		//コライダーのポインタ
		std::shared_ptr<Collider>m_collider;
		//自身の振る舞い
		unsigned char m_myAttribute = 0;
	};

	//初期化済か
	bool m_invalid = true;
	//登録されたコライダーの振る舞いリスト
	std::map<std::string, unsigned char>m_attributeList;

	//登録されたコライダーリスト
	std::list<RegisterInfo>m_registerList;

	void CheckInvalid();

public:
	CollisionManager() {}
	void Generate(const std::map<std::string, unsigned char>& arg_attributeList);

	void Update();
	void DebugDraw(Camera& Cam);

	const unsigned char& GetAttribute(std::string arg_key)const;
};

