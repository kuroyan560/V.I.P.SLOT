#pragma once
#include<memory>
class Model;
class EnemyController;

class EnemyBreed
{
public:
	//��ʔԍ��i��ނ��Ɓj
	int m_typeID;
	//���f��
	std::shared_ptr<Model>m_model;
	//�ő�HP
	int m_maxHp;
	//�����R�C���i�|�����瓾����R�C���̐��j
	int m_initCoinNum;
	//��������
	std::unique_ptr<EnemyController>m_controller;
};