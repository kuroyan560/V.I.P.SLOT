#pragma once
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"Transform.h"
class Camera;

class ReturnCoinEmitter
{
	//�ԋp����R�C���̋����i�X���b�g�㕔������o�j
	CoinObjectManager m_returnCoinObjManager;
	class ReturnCoinPerform : public CoinPerform
	{
		float m_fallAccel = 0.0f;
		Vec3<float>m_move;
	public:
		ReturnCoinPerform(Vec3<float>arg_initMove) : m_move(arg_initMove) {	}
		void OnUpdate(Coins& arg_coin)override;
		void OnEmit(Coins& arg_coin)override {};
	};

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
	int Update();
	//�`��
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//���o
	void Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform);
};

