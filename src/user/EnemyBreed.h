#pragma once
#include<memory>
class Model;
class EnemyController;

class EnemyBreed
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
	const std::unique_ptr<EnemyController>m_controller;

	EnemyBreed(
		int arg_typeID,
		std::shared_ptr<Model>arg_model,
		int arg_maxHp,
		int arg_initCoinNum,
		std::unique_ptr<EnemyController>arg_controller)
		:m_typeID(arg_typeID),
		m_model(arg_model),
		m_maxHp(arg_maxHp),
		m_initCoinNum(arg_initCoinNum),
		m_controller(std::move(arg_controller)) {}
};