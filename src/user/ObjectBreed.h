#pragma once
#include<memory>
#include<vector>
#include"Collider.h"
class Model;
class ObjectController;

class ObjectBreed
{
public:
	//��ʔԍ��i��ނ��Ɓj
	const int m_typeID;
	//���f��
	const std::shared_ptr<Model>m_model;
	//�ő�HP
	const int m_maxHp;
	//�����R�C���i�|�����瓾����R�C���̐��j
	const int m_initCoinNum;
	//��������
	const std::unique_ptr<ObjectController>m_controller;
	//�R���C�_�[�i�N���[�����j
	const std::vector<std::unique_ptr<Collider>>m_originCollider;

	/// <summary>
	/// �G�̌����R���X�g���N�^
	/// </summary>
	/// <param name="arg_typeID">��ʔԍ��i�G�̎�ނ��Ɓj</param>
	/// <param name="arg_model">���f���f�[�^</param>
	/// <param name="arg_maxHp">�ő�HP</param>
	/// <param name="arg_initCoinNum">�������̏����R�C����</param>
	/// <param name="arg_controller">��������</param>
	/// <param name="arg_originCollider">�N���[�����R���C�_�[</param>
	ObjectBreed(
		int arg_typeID,
		std::shared_ptr<Model>arg_model,
		int arg_maxHp,
		int arg_initCoinNum,
		std::unique_ptr<ObjectController>arg_controller, 
		std::vector<std::unique_ptr<Collider>>& arg_originCollider)
		:m_typeID(arg_typeID),
		m_model(arg_model),
		m_maxHp(arg_maxHp),
		m_initCoinNum(arg_initCoinNum),
		m_controller(std::move(arg_controller)),
		m_originCollider(std::move(arg_originCollider)) {}
};