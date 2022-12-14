#include "StartWave.h"
#include"GameCamera.h"
#include"Screen.h"
#include"Object.h"
#include"Angle.h"

void StartWave::OnStart()
{
    //登場ステータス
    m_status = APPEAR;
    //描画フラグオンに
    m_referScreen.lock()->SetDrawFlg(true);
    //登場モーションワーク開始
    m_appearScreenWork.Start(true);

    m_referScreen.lock()->m_modelObj->m_transform.SetRotate(Vec3<float>::GetYAxis(), m_ySpinRadianMax);

    //敵を出現させない
    m_enemyEmit = false;
}

void StartWave::OnUpdate()
{
    //プレイヤーの座標取得
    const auto playerPos = InGameMonitor::GetPlayer()->GetCenterPos();
    //スクリーンのトランスフォームへの参照取得
    auto& screenTransform = m_referScreen.lock()->m_modelObj->m_transform;
    //現在のプレイヤーの位置から目標値を算出
    auto targetPos = KuroMath::Lerp(screenTransform.GetPos(), m_defaultPosOffset + playerPos, m_followLerpRate);

    //各モーションワークに目標値を再設定
    m_appearScreenWork.GetMotion(0).m_endPos = targetPos;
    m_disappearScreenWork.GetMotion(0).m_startPos = targetPos;

    //モーションワーク更新
    m_appearScreenWork.Update(1.0f);
    m_disappearScreenWork.Update(1.0f);

    if (m_status == APPEAR)
    {
        //登場モーションワークからスクリーンの座標適用
        screenTransform.SetPos(m_appearScreenWork.GetPos());

        //スクリーンの回転
        float ySpin = KuroMath::Ease(Out, Back, m_appearScreenWork.GetTimeRate(), m_ySpinRadianMax, 0.0f);
        screenTransform.SetRotate(Vec3<float>::GetYAxis(), ySpin);

        //スケール変更
        float scale = KuroMath::Ease(Out, Back, m_appearScreenWork.GetTimeRate(), 0.0f, 1.0f);
        screenTransform.SetScale(scale);

        //登場モーションワーク終了で
        if (m_appearScreenWork.IsEnd())
        {
            //待機状態へ移行
            m_status = WAIT;
            //待機時間セット
            m_waitTimer.Reset(m_waitInterval);
        }
    }
    else if (m_status == WAIT)
    {
        //追従
        screenTransform.SetPos(targetPos);

        //待機時間終了で
        if (m_waitTimer.UpdateTimer(1.0f))
        {
            //退場状態へ移行
            m_status = DISAPPEAR;
            //退場モーションワーク開始
            m_disappearScreenWork.Start(true);
        }
    }
    else if (m_status == DISAPPEAR)
    {
        //退場モーションワークからスクリーンの座標適用
        screenTransform.SetPos(m_disappearScreenWork.GetPos());

        //スクリーンの回転
        float ySpin = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 0.0f, -m_ySpinRadianMax);
        screenTransform.SetRotate(Vec3<float>::GetYAxis(), ySpin);

        //スケール変更
        float scale = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 1.0f, 0.0f);
        screenTransform.SetScale(scale);
    }
}

void StartWave::OnFinish()
{
    //描画フラグオフに
    m_referScreen.lock()->SetDrawFlg(false);
    //敵出現許可
    m_enemyEmit = true;
}

bool StartWave::End()
{
    //退場済
    return m_status == DISAPPEAR && m_disappearScreenWork.IsEnd();
}

std::shared_ptr<Camera> StartWave::GetMainCam()
{
    //ゲームカメラをそのまま流用
    return m_referGameCam.lock()->GetMainCam();
}

std::shared_ptr<Camera> StartWave::GetSubCam()
{
    //ゲームカメラをそのまま流用
    return m_referGameCam.lock()->GetSubCam();
}

void StartWave::OnImguiItems()
{
    if (ImGui::Button("Preview"))
    {
        this->SetNextEvent();
    }

    ImGui::DragFloat3("DefaultPosOffset", (float*)&m_defaultPosOffset, 0.05f);

    float scale = m_referScreen.lock()->m_modelObj->m_transform.GetScale().x;
    if (ImGui::DragFloat("Scale", &scale, 0.05f, 0.0f))
    {
        m_referScreen.lock()->m_modelObj->m_transform.SetScale(scale);
    }
    ImGui::DragFloat("FollowLerpRate", &m_followLerpRate, 0.02f, 0.0f, 1.0f);

    m_appearScreenWork.ImguiDebugItems("AppearWork");
    m_disappearScreenWork.ImguiDebugItems("DisppearWork");
}

StartWave::StartWave() : Debugger("StartWave")
{
    Motion motion;
    motion.m_startPos = { 15.0f,5.0f,0.0f };
    motion.m_startTarget = { 1,0,0 };
    motion.m_endTarget = { 0,0,1 };
    motion.m_interval = 90.0f;
    motion.m_easeParam.m_changeType = Out;
    motion.m_easeParam.m_easeType = Circ;
    m_appearScreenWork.Add(motion);

    motion.m_startTarget = motion.m_endTarget;
    motion.m_endTarget = { -1,0,0 };
    motion.m_endPos = { -15.0f,5.0f,0.0f };
    motion.m_interval = 90.0f;
    motion.m_easeParam.m_changeType = In;
    motion.m_easeParam.m_easeType = Circ;
    m_disappearScreenWork.Add(motion);
}
