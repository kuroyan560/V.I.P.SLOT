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
		//�R���C�_�[�̃|�C���^
		std::shared_ptr<Collider>m_collider;
		//���g�̐U�镑��
		unsigned char m_myAttribute = 0;
	};

	//�o�^���ꂽ�R���C�_�[�̐U�镑�����X�g
	std::map<std::string, unsigned char>m_attributeList;

	//�o�^���ꂽ�R���C�_�[���X�g
	std::list<RegisterInfo>m_registerList;

	//�Փ˂��Ă����ꍇ�̏���
	void OnHit(const RegisterInfo& arg_myInfo, const RegisterInfo& arg_otherInfo, const Vec3<float>& arg_inter);

public:
	CollisionManager() {}
	void AddAttribute(std::string arg_attributeKey, unsigned char arg_attribute);

	void Update();
	void DebugDraw(Camera& Cam);

	//�R���C�_�[�̓o�^
	void Register(std::string arg_attributeKey, const std::shared_ptr<Collider>& arg_collider);
	void Register(std::string arg_attributeKey, const std::vector<std::shared_ptr<Collider>>& arg_colliderArray)
	{
		for (auto& col : arg_colliderArray)Register(arg_attributeKey, col);
	}
	//�R���C�_�[�̓o�^����
	void Remove(const std::shared_ptr<Collider>& arg_collider);

	//�R���C�_�[�U�镑�����L�[����擾
	const unsigned char& GetAttribute(std::string arg_key)const;
};

