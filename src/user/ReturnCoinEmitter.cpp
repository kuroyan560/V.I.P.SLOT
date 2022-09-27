#include "ReturnCoinEmitter.h"
#include"AudioApp.h"

ReturnCoinEmitter::ReturnCoinEmitter()
{
	m_coinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav", 0.15f);
}

void ReturnCoinEmitter::Init()
{
	m_returnCoinObjManager.Init();
}

int ReturnCoinEmitter::Update()
{
	return m_returnCoinObjManager.Update();
}

void ReturnCoinEmitter::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	m_returnCoinObjManager.Draw(arg_lightMgr, arg_cam);
}

void ReturnCoinEmitter::Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform)
{
	//総額
	int slotCoinNum = arg_coinNumSum;

	//返却コインの寿命
	static const int RETURN_COIN_LIFE_TIME = 200;
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

	//描画する返却コインの追加
	for (; arg_perCoinNum <= slotCoinNum; slotCoinNum -= arg_perCoinNum)
	{
		const float power = KuroFunc::GetRand(RETURN_COIN_EMIT_POWER_MIN, RETURN_COIN_EMIT_POWER_MAX);
		float vec_x = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
		vec_x *= KuroFunc::GetRandPlusMinus();
		float vec_y = KuroFunc::GetRand(RETURN_COIN_EMIT_Y_POWER_RATE_MIN, RETURN_COIN_EMIT_Y_POWER_RATE_MAX);
		float vec_z = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
		vec_z *= KuroFunc::GetRandPlusMinus();

		const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

		m_returnCoinObjManager.Add(arg_perCoinNum,
			arg_initTransform, RETURN_COIN_LIFE_TIME, new ReturnCoinPerform(initMove));
	}
}

void ReturnCoinEmitter::ReturnCoinPerform::OnUpdate(Coins& arg_coin)
{
	const float GRAVITY = -0.002f;
	m_move.y += m_fallAccel;
	m_fallAccel += GRAVITY;

	auto pos = arg_coin.m_transform.GetPos();
	pos += m_move;

	//XZ平面方向の移動量は空気抵抗で減衰
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);
	m_move.z = KuroMath::Lerp(m_move.z, 0.0f, 0.01f);

	arg_coin.m_transform.SetPos(pos);
}
