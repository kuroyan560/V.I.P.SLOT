#pragma once
#include<memory>
#include<forward_list>
#include"Coins.h"
class Model;
class CoinPerform;
class LightManager;
class GameCamera;

//�R�C�����o�E����
class CoinObjectManager
{
private:
	//�R�C���z��
	std::forward_list<Coins>m_coins;

	//�R�C�����f��
	std::shared_ptr<Model>m_coinModel;

public:
	//�������A�^�b�`
	CoinObjectManager();
	//������
	void Init();
	/// <summary>
	/// �X�V
	/// </summary>
	/// <returns>���o�I�������R�C���̑����i�O�Ȃ�ǂ̃R�C�����I�����Ă��Ȃ��j</returns>
	int Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam);

	//�R�C���ǉ�
	void Add(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime, CoinPerform* arg_perform);
};