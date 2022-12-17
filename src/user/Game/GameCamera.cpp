#include "GameCamera.h"
#include"KuroFunc.h"
#include"KuroMath.h"
#include<math.h>

void GameCamera::SetPosAndTarget(Vec3<float>arg_lerpOffset, float arg_timeScale, bool arg_interpolation)
{
	Vec3<float>offset = arg_lerpOffset;

	//床下はあまり見えないように高さ補正
	const float CORRECT_HEIGHT = 4.5f;
	offset.y += (1.0f - std::min(arg_lerpOffset.y / 10.0f, 1.0f)) * CORRECT_HEIGHT;

	//前景カメラ
	m_posLerpOffset = KuroMath::Lerp(m_posLerpOffset, offset,
		arg_interpolation ? 0.05f * arg_timeScale : 1.0f);

	auto newPos = KuroMath::Lerp(m_cam[MAIN]->GetPos() - m_oldShakeOffset, m_defaultPos[MAIN] + m_posLerpOffset, 
		arg_interpolation ? 0.1f * arg_timeScale : 1.0f);
	m_cam[MAIN]->SetPos(newPos + m_shake.GetOffset());

	m_targetLerpOffset = KuroMath::Lerp(m_targetLerpOffset, offset, 0.08f * arg_timeScale);
	auto newTarget = KuroMath::Lerp(m_cam[MAIN]->GetTarget() - m_oldShakeOffset, m_targetPos[MAIN] + m_targetLerpOffset,
		arg_interpolation ? 0.1f * arg_timeScale : 1.0f);
	m_cam[MAIN]->SetTarget(newTarget + m_shake.GetOffset());

	//背景カメラ
	m_cam[SUB]->SetPos(m_defaultPos[SUB] + m_shake.GetOffset());
	m_cam[SUB]->SetTarget(m_targetPos[SUB] + m_shake.GetOffset());
}


GameCamera::GameCamera() : m_shake({ 1.0f,1.0f,0.0f })
{
	m_cam[SUB] = std::make_shared<Camera>("GameCamera - Back");
	m_cam[MAIN] = std::make_shared<Camera>("GameCamera - Front");
}

void GameCamera::Init()
{
	for (int i = 0; i < NUM; ++i)
	{
		m_cam[i]->SetAngleOfView(Angle(m_defaultCapeView[i]));
	}

	m_defaultPos[MAIN] = Vec3<float>(0.0f, 3.4f, -65.0f);
	m_targetPos[MAIN] = Vec3<float>(0.0f, m_defaultPos[MAIN].y + 2.0f, 0.0f);
	m_posLerpOffset = { 0,0,0 };
	m_targetLerpOffset = { 0.0f,-1.0f,120.0f };
	m_shake.Init();
	SetPosAndTarget({ 0,0,100 }, 1.0f, false);
}

void GameCamera::Update(float arg_timeScale, Vec3<float>arg_playersDisplacement)
{
	//1フレーム前の振動オフセットを記録
	m_oldShakeOffset = m_shake.GetOffset();

	m_shake.Update(arg_timeScale);

	SetPosAndTarget(arg_playersDisplacement, 1.0f, true);
}