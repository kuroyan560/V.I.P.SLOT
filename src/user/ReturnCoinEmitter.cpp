#include "ReturnCoinEmitter.h"
#include"AudioApp.h"
#include"ConstParameters.h"

ReturnCoinEmitter::ReturnCoinEmitter()
{
	m_coinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav", 0.15f);
}

void ReturnCoinEmitter::Init()
{
	m_returnCoinObjManager.Init();
	m_returnCoins.clear();
	m_emitTimer.Reset(ConstParameter::Slot::RETURN_COIN_EMIT_SPAN);
}

int ReturnCoinEmitter::Update(float arg_timeScale)
{
	using namespace ConstParameter::Slot;

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

	//�ԋp�R�C���z��
	if (!m_returnCoins.empty())
	{
		//���o�^�C�~���O
		if (m_emitTimer.IsTimeUp())
		{
			//�P�x�ɕ��o���鐔
			const int emitCoinCount = KuroFunc::GetRand(EMIT_COIN_COUNT_MIN, EMIT_COIN_COUNT_MAX);

			//���o������
			int count = 0;

			while (count < emitCoinCount && !m_returnCoins.empty())
			{
				//�ԋp�R�C���̏��擾
				const auto& returnCoin = m_returnCoins.front();

				//���o�p���[
				const float power = KuroFunc::GetRand(RETURN_COIN_EMIT_POWER_MIN, RETURN_COIN_EMIT_POWER_MAX);

				//���o�x�N�g��
				float vec_y = KuroFunc::GetRand(RETURN_COIN_EMIT_Y_POWER_RATE_MIN, RETURN_COIN_EMIT_Y_POWER_RATE_MAX);
				float vec_x = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				float vec_z = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				vec_x *= KuroFunc::GetRandPlusMinus();
				vec_z *= KuroFunc::GetRandPlusMinus();

				//���o�ړ���
				const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

				//�ԋp�R�C���`��
				m_returnCoinObjManager.Add(returnCoin.m_perCoinNum,
					returnCoin.m_initTransform, RETURN_COIN_LIFE_TIME, new ReturnCoinPerform(initMove));

				//���o�������J�E���g
				count++;
			}

			//SE�Đ�
			AudioApp::Instance()->PlayWaveDelay(m_coinReturnSE);

			//���o�ς̃R�C�����|�b�v
			m_returnCoins.pop_front();

			//���o�^�C�}�[���Z�b�g
			m_emitTimer.Reset(ConstParameter::Slot::RETURN_COIN_EMIT_SPAN);
		}

		//���o�^�C�}�[�J�E���g
		m_emitTimer.UpdateTimer(arg_timeScale);
	}

	return m_returnCoinObjManager.Update(arg_timeScale);
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

	m_emitTimer.Reset(0);
}

void ReturnCoinEmitter::ReturnCoinPerform::OnUpdate(Coins& arg_coin, float arg_timeScale)
{
	m_move.y += m_fallAccel;
	m_fallAccel += ConstParameter::Environment::COIN_GRAVITY * arg_timeScale;

	auto pos = arg_coin.m_transform.GetPos();
	pos += m_move * arg_timeScale;

	//XZ���ʕ����̈ړ��ʂ͋�C��R�Ō���
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);
	m_move.z = KuroMath::Lerp(m_move.z, 0.0f, 0.01f);

	arg_coin.m_transform.SetPos(pos);
}
