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

	//返却コインの放出パワー下限
	static const float RETURN_COIN_EMIT_POWER_MIN = 2.0f;
	//返却コインの放出パワー上限
	static const float RETURN_COIN_EMIT_POWER_MAX = 6.0f;
	//返還の放出パワーXZ平面方向の強さレート下限
	static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MIN = 0.05f;
	//返還の放出パワーXZ平面方向の強さレート上限
	static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MAX = 0.2f;
	//返還の放出パワーY方向の強さレート下限
	static const float RETURN_COIN_EMIT_Y_POWER_RATE_MIN = 0.3f;
	//返還の放出パワーY方向の強さレート上限
	static const float RETURN_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;

	//返却コイン配列
	if (!m_returnCoins.empty())
	{
		//放出タイミング
		if (m_emitTimer.IsTimeUp())
		{
			//１度に放出する数
			const int emitCoinCount = KuroFunc::GetRand(EMIT_COIN_COUNT_MIN, EMIT_COIN_COUNT_MAX);

			//放出した数
			int count = 0;

			while (count < emitCoinCount && !m_returnCoins.empty())
			{
				//返却コインの情報取得
				const auto& returnCoin = m_returnCoins.front();

				//放出パワー
				const float power = KuroFunc::GetRand(RETURN_COIN_EMIT_POWER_MIN, RETURN_COIN_EMIT_POWER_MAX);

				//放出ベクトル
				float vec_y = KuroFunc::GetRand(RETURN_COIN_EMIT_Y_POWER_RATE_MIN, RETURN_COIN_EMIT_Y_POWER_RATE_MAX);
				float vec_x = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				float vec_z = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
				vec_x *= KuroFunc::GetRandPlusMinus();
				vec_z *= KuroFunc::GetRandPlusMinus();

				//放出移動量
				const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

				//返却コイン描画
				m_returnCoinObjManager.Add(returnCoin.m_perCoinNum,
					returnCoin.m_initTransform, RETURN_COIN_LIFE_TIME, new ReturnCoinPerform(initMove));

				//放出した数カウント
				count++;
			}

			//SE再生
			AudioApp::Instance()->PlayWaveDelay(m_coinReturnSE);

			//放出済のコイン情報ポップ
			m_returnCoins.pop_front();

			//放出タイマーリセット
			m_emitTimer.Reset(ConstParameter::Slot::RETURN_COIN_EMIT_SPAN);
		}

		//放出タイマーカウント
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

	//総額
	int slotCoinNum = arg_coinNumSum;

	//描画する返却コインの追加
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

	//XZ平面方向の移動量は空気抵抗で減衰
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);
	m_move.z = KuroMath::Lerp(m_move.z, 0.0f, 0.01f);

	arg_coin.m_transform.SetPos(pos);
}
