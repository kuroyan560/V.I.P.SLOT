#include "ReturnCoinEmitter.h"
#include"AudioApp.h"

ReturnCoinEmitter::ReturnCoinEmitter()
{
	m_coinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav", 0.15f);
}

void ReturnCoinEmitter::Init()
{
	m_returnCoinObjManager.Init();
	m_returnCoins.clear();
	m_emitTimer = -1;
}

int ReturnCoinEmitter::Update()
{
	const int EMIT_SPAN = 5;

	//�ԋp�R�C���̎���
	static const int RETURN_COIN_LIFE_TIME = 200;
	//�ԋp�R�C���̕��o�p���[����
	static const float RETURN_COIN_EMIT_POWER_MIN = 2.0f;
	//�ԋp�R�C���̕��o�p���[���
	static const float RETURN_COIN_EMIT_POWER_MAX = 6.0f;
	//�Ԋ҂̕��o�p���[XZ���ʕ����̋������[�g����
	static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MIN = 0.05f;
	//�Ԋ҂̕��o�p���[XZ���ʕ����̋������[�g���
	static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MAX = 0.2f;
	//�Ԋ҂̕��o�p���[Y�����̋������[�g����
	static const float RETURN_COIN_EMIT_Y_POWER_RATE_MIN = 0.3f;
	//�Ԋ҂̕��o�p���[Y�����̋������[�g���
	static const float RETURN_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;


	if (!m_returnCoins.empty())
	{
		if (m_emitTimer % EMIT_SPAN == 0)
		{
			const int EMIT_COIN_COUNT_MIN = 1;
			const int EMIT_COIN_COUNT_MAX = 10;
			const int emitCoinCount = KuroFunc::GetRand(EMIT_COIN_COUNT_MIN, EMIT_COIN_COUNT_MAX);

			int count = 0;

			while (count < emitCoinCount && !m_returnCoins.empty())
			{
				const auto& returnCoin = m_returnCoins.front();

				const float power = KuroFunc::GetRand(RETURN_COIN_EMIT_POWER_MIN, RETURN_COIN_EMIT_POWER_MAX);
				float vec_x = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				vec_x *= KuroFunc::GetRandPlusMinus();
				float vec_y = KuroFunc::GetRand(RETURN_COIN_EMIT_Y_POWER_RATE_MIN, RETURN_COIN_EMIT_Y_POWER_RATE_MAX);
				float vec_z = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				vec_z *= KuroFunc::GetRandPlusMinus();

				const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;
				m_returnCoinObjManager.Add(returnCoin.m_perCoinNum,
					returnCoin.m_initTransform, RETURN_COIN_LIFE_TIME, new ReturnCoinPerform(initMove));

				count++;
			}

			AudioApp::Instance()->PlayWaveDelay(m_coinReturnSE);

			m_returnCoins.pop_front();

			m_emitTimer = 0;
		}

		m_emitTimer++;
	}
	else m_emitTimer = -1;

	return m_returnCoinObjManager.Update();
}

void ReturnCoinEmitter::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	m_returnCoinObjManager.Draw(arg_lightMgr, arg_cam);
}

void ReturnCoinEmitter::Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform)
{
	m_returnCoins.clear();

	//���z
	int slotCoinNum = arg_coinNumSum;

	//�`�悷��ԋp�R�C���̒ǉ�
	for (; arg_perCoinNum <= slotCoinNum; slotCoinNum -= arg_perCoinNum)
	{
		m_returnCoins.emplace_front(arg_perCoinNum, arg_initTransform);
	}

	m_emitTimer = 0;
}

void ReturnCoinEmitter::ReturnCoinPerform::OnUpdate(Coins& arg_coin)
{
	const float GRAVITY = -0.002f;
	m_move.y += m_fallAccel;
	m_fallAccel += GRAVITY;

	auto pos = arg_coin.m_transform.GetPos();
	pos += m_move;

	//XZ���ʕ����̈ړ��ʂ͋�C��R�Ō���
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);
	m_move.z = KuroMath::Lerp(m_move.z, 0.0f, 0.01f);

	arg_coin.m_transform.SetPos(pos);
}
