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
		//�R���C�_�[�̃|�C���^
		std::shared_ptr<Collider>m_collider;
		//���g�̐U�镑��
		unsigned char m_myAttribute = 0;
	};

	//�������ς�
	bool m_invalid = true;
	//�o�^���ꂽ�R���C�_�[�̐U�镑�����X�g
	std::map<std::string, unsigned char>m_attributeList;

	//�o�^���ꂽ�R���C�_�[���X�g
	std::list<RegisterInfo>m_registerList;

	void CheckInvalid();

public:
	CollisionManager() {}
	void Generate(const std::map<std::string, unsigned char>& arg_attributeList);

	void Update();
	void DebugDraw(Camera& Cam);

	const unsigned char& GetAttribute(std::string arg_key)const;
};

