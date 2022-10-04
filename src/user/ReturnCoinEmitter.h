#pragma once
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"Transform.h"
#include<forward_list>
#include"Timer.h"
class Camera;

class ReturnCoinEmitter
{
	//�ԋp����R�C���̋����i�X���b�g�㕔������o�j
	CoinObjectManager m_returnCoinObjManager;
	class ReturnCoinPerform : public CoinPerform
	{
		//���������x
		float m_fallAccel = 0.0f;
		//�ړ���
		Vec3<float>m_move;

	public:
		ReturnCoinPerform(Vec3<float>arg_initMove) : m_move(arg_initMove) {	}
		void OnUpdate(Coins& arg_coin, float arg_timeScale)override;
		void OnEmit(Coins& arg_coin)override {};
		bool IsDead(Coins& arg_coin)override;
	};

	struct ReturnCoin
	{
		int m_perCoinNum;
		Transform m_initTransform;
		ReturnCoin(const int& arg_perCoinNum, const Transform& arg_initTransform)
			:m_perCoinNum(arg_perCoinNum), m_initTransform(arg_initTransform) {}
	};
	std::forward_list<ReturnCoin>m_returnCoins;

	//���o�^�C�~���O�v��
	Timer m_emitTimer;

	//�R�C���ԋpSE
	int m_coinReturnSE;

public:
	ReturnCoinEmitter();

	//������
	void Init();
	/// <summary>
	/// �X�V
	/// </summary>
	/// <returns>���o���I�����ԋp����R�C���̐�</returns>
	int Update(float arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//���o
	void Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform);
};

