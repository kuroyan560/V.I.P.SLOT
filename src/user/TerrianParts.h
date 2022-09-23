#pragma once
#include<memory>
#include<vector>
#include"Transform.h"
class Model;
class ActPoint;
class Collider;
class TerrianParts;
class TerrianPartsType;

//�n�`�p�[�c�̋���
class TerrianPartsControl
{
	friend class TerrianPartsType;
	virtual TerrianPartsControl* Clone() = 0;
public:
	virtual void Init(TerrianParts& Parts) {}
	virtual void Update(TerrianParts& Parts) {}
};

//�n�`�p�[�c�^�I�u�W�F�N�g
class TerrianPartsType
{
	friend class TerrianManager;

	//���f��
	std::shared_ptr<Model>m_model;
	//�R���C�_�[�N���[����
	std::vector<std::shared_ptr<Collider>>m_collidersOrigin;
	//�n�`�p�[�c�̋���
	std::weak_ptr<TerrianPartsControl>m_controlOrigin;
	//ActPoint�N���[����
	std::vector<std::shared_ptr<ActPoint>>m_actPointOrigin;

	TerrianPartsType(
		std::shared_ptr<Model>Model,
		const std::vector<std::shared_ptr<Collider>>& CollidersOrigin,
		std::shared_ptr<TerrianPartsControl> ControlOrigin = nullptr,
		std::vector<std::shared_ptr<ActPoint>>ActPointOrigin = {});
public:

	//�X�e�[�W�p�[�c�C���X�^���X����
	std::shared_ptr<TerrianParts>GenerateParts(const Transform& InitTransform);
};

//�n�`�p�[�c
class TerrianParts
{
	friend class TerrianPartsControl;
	friend class TerrianPartsType;
	friend class TerrianManager;
	
	const TerrianPartsType* m_typeObj;

	//�g�����X�t�H�[��
	Transform m_transform;

	//�R���C�_�[
	std::vector<std::shared_ptr<Collider>>m_colliders;
	//����
	std::shared_ptr<TerrianPartsControl>m_control;
	//�n�`ActPoint
	std::vector<std::shared_ptr<ActPoint>>m_actPoints;

	//�^�I�u�W�F�N�g���炵�������ł��Ȃ�
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