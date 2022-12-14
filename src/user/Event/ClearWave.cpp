#include "ClearWave.h"
#include"TimeScale.h"
#include"KuroMath.h"
#include"KuroEngine.h"
#include"Camera.h"
#include"GameCamera.h"
#include"Player.h"
#include"WaveMgr.h"
#include"HUDInterface.h"
#include"EnemyEmitter.h"
#include"ObjectManager.h"

void ClearWave::OnStart()
{
	m_status = START_WAIT;

	m_waitTimer.Reset(m_startStopWaitInterval);
	*m_cam = *m_referGameCam.lock()->GetMainCam();

	m_referTimeScale->Set(0.0f);

	//HUD非表示
	HUDInterface::s_draw = false;

	//プレイヤーの被ダメージコライダーオフ
	m_referPlayer.lock()->SetDamageColldierActive(false);

	m_enemyKill = false;
}

void ClearWave::EmitHealKit(int arg_num)
{
	const float EMIT_POWER_X = 0.15f;
	const float EMIT_POWER_Y_MIN = 0.7f;
	const float EMIT_POWER_Y_MAX = 1.0f;

	for (int i = 0; i < arg_num; ++i)
	{
		Vec3<float>initVel;
		initVel.x = KuroFunc::GetRand(-EMIT_POWER_X, EMIT_POWER_X);
		initVel.y = KuroFunc::GetRand(EMIT_POWER_Y_MIN, EMIT_POWER_Y_MAX);
		m_referObjMgr.lock()->AppearHealKit(
			m_referCollisionMgr,
			Vec3<float>(0.0f, 6.0f, ConstParameter::Environment::FIELD_FLOOR_POS.z),
			initVel);
	}
}

void ClearWave::OnUpdate()
{
	if (m_debug && !m_preview)return;

	//演出開始直後しばらく待機
	if (m_status == START_WAIT)
	{
		if (m_waitTimer.UpdateTimer())
		{
			m_status = CAM_WORK;
			//カメラワークスタート
			m_camWork.Start(false);
		}
	}
	else if (m_status == WAIT)
	{
		if (m_waitTimer.UpdateTimer())
		{
			//次のカメラワークに進行
			m_camWork.Proceed();
			m_status = CAM_WORK;
		}
	}
	else if (m_status == CAM_WORK)
	{
		//カメラワーク更新
		bool nowCamWorkEnd = m_camWork.Update(1.0f);

		//プレイヤーの座標取得
		auto playerPos = m_referPlayer.lock()->GetCenterPos();

		//座標
		m_cam->SetPos(playerPos + m_camWork.GetPos());
		//注視点
		m_cam->SetTarget(playerPos + m_camWork.GetTarget());

		//最後のカメラワークでスローモーションに
		if (m_camWork.IsLastMotion())
		{
			float timeScale = m_slowEaseParam.Calculate(m_camWork.GetTimeRate(), 0.0f, m_slowTimeScale);
			m_referTimeScale->Set(timeScale);

			if (!m_enemyKill)
			{
				m_enemyKill = true;
				//敵全員死亡
				m_referEnemyEmitter.lock()->KillAllEnemys();
			}
		}

		//カメラワーク切り替え時
		if (nowCamWorkEnd)
		{
			//終了
			if (m_camWork.IsEnd())
			{
				EmitHealKit(m_referWaveMgr.lock()->GetHealKitNum());
				m_status = END;
				m_referWaveMgr.lock()->ProceedWave();
				*m_referGameCam.lock()->GetMainCam() = *this->m_cam;
				m_referTimeScale->Set(1.0f);
			}
			//カメラワーク間の待機状態へ
			else
			{
				m_status = WAIT;
				m_waitTimer.Reset(m_camWorkWaitInterval);
			}
		}
	}

	//デバッグモード
	if (m_debug && End())
	{
		m_preview = false;
		OnStart();
	}
}
void ClearWave::OnFinish()
{
	//HUD表示
	HUDInterface::s_draw = true;

	//プレイヤーの被ダメージコライダーオン
	m_referPlayer.lock()->SetDamageColldierActive(true);
}

std::shared_ptr<Camera> ClearWave::GetSubCam()
{
	return m_referGameCam.lock()->GetSubCam();
}

void ClearWave::OnImguiItems()
{
	if (ImGui::Checkbox("Debug", &m_debug))
	{
		if (m_debug)this->Start();
		else
		{
			m_referTimeScale->Set(1.0f);
		}
	}

	if (ImGui::Button("Preview"))
	{
		m_preview = true;
		this->Start();
	}

	if (!m_debug)return;

	ImGui::Separator();
	ImGui::DragFloat("StartStopWaitInterval", &m_startStopWaitInterval, 0.5f, 1.0f);
	ImGui::DragFloat("CamWorkWaitInterval", &m_camWorkWaitInterval, 0.5f, 1.0f);
	ImGui::DragFloat("SlowWaitInterval", &m_slowWaitInterval, 0.5f, 1.0f);

	m_slowEaseParam.ImguiDebug("Slow");
	ImGui::DragFloat("SlowTimeScale", &m_slowTimeScale, 0.0f, 0.0f);

	const Vec3<float>* changePos = nullptr;
	const Vec3<float>* changeTarget = nullptr;
	m_camWork.ImguiDebugItems("CamWork", changePos, changeTarget);
	if (changePos != nullptr)m_cam->SetPos(*changePos);
	if (changeTarget != nullptr)m_cam->SetTarget(*changeTarget);
}

ClearWave::ClearWave() :Debugger("ClearWave")
{
	m_cam = std::make_shared<Camera>("ClearWave - Event - Camera");
	
	Motion motion;
	motion.m_startPos = { -8.62f, 4.76f, -19.96f };
	motion.m_endPos = { -8.62f, 4.76f, -19.96f };
	motion.m_startTarget = { 0,0,0 };
	motion.m_endTarget = { 0,0,0 };
	motion.m_interval = 25.0f;
	m_camWork.Add(motion);

	motion.m_startPos = { 13.4f, 4.0f, -12.94f };
	motion.m_endPos = { -3.52f, 2.28f, -5.82f };
	motion.m_startTarget = { 0,0,0 };
	motion.m_endTarget = { 0,0,0 };
	motion.m_interval = 25.0f;
	m_camWork.Add(motion);

	motion.m_startPos = { 0.0f, 0.62f, -3.0f };
	motion.m_endPos = { 0.0f, 9.7f, -32.66f };
	motion.m_startTarget = { 0.0f, 0.36f, 0.0f };
	motion.m_endTarget = { 0.0f, 9.3f, 0.0f };
	motion.m_easeParam.m_easeType = Quint;
	motion.m_interval = m_slowWaitInterval;
	m_camWork.Add(motion);
}
