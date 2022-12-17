#include "StartWave.h"
#include"GameCamera.h"
#include"Screen.h"
#include"Object.h"
#include"Angle.h"
#include"WaveMgr.h"
#include"DrawFunc2D.h"

void StartWave::OnStart()
{
    //登場ステータス
    m_status = APPEAR;
    //描画フラグオンに
    m_referScreen.lock()->SetDrawFlg(true);
    //登場モーションワーク開始
    m_appearScreenWork.Start(true);

    //敵を出現させない
    m_enemyEmit = false;

    //ウェーブ表示のスクロール量リセット
    m_wavePosScrollX = 0.0f;

    //スクリーンの画面クリア
    m_referScreen.lock()->ClearTarget();
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
            //ウェーブ表示のモーションワーク開始
            m_screenWaveDraw.Start(true);
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

        //スケール変更
        float scale = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 1.0f, 0.0f);
        screenTransform.SetScale(scale);
    }

    if (m_status != APPEAR)
    {
        m_screenWaveDraw.Update(1.0f);
        float waveExpY = 1.0f;

        /*--- 大型スクリーンにに描画 ---*/
        //大型スクリーンのレンダーターゲットセット
        m_referScreen.lock()->ClearTarget();
        m_referScreen.lock()->SetTarget();

        //ウェーブ表示スクロール
        m_wavePosScrollX -= 2.0f;

        //ウェーブ表示位置
        Vec2<float>drawPos = m_screenWaveDraw.GetPos() + Vec2<float>(m_wavePosScrollX, 0.0f);
        DrawFunc2D::DrawRotaGraph2D(
            drawPos,
            { 1.0f,waveExpY },
            0.0f,
            m_waveStrTex
        );

        //ウェーブ数字描画位置のオフセット
        Vec2<float>numPosOffset = { m_waveStrTex->GetGraphSize().Float().x * 0.7f,0.0f };
        DrawFunc2D::DrawNumber2D(
            m_referWaveMgr.lock()->GetNowWaveIdx() + 1,
            drawPos + numPosOffset,
            m_waveNumTex.data(),
            { 1.0f,waveExpY },
            8.0f,
            HORIZONTAL_ALIGN::LEFT,
            VERTICAL_ALIGN::CENTER);
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
    m_screenWaveDraw.ImguiDebugItems("ScreenWaveDraw");
}

StartWave::StartWave() : Debugger("StartWave")
{
    //テクスチャ読み込み
    std::string dir = "resource/user/img/ui/screen/";
    m_goodLuckTex = D3D12App::Instance()->GenerateTextureBuffer(dir + "wave_good_luck.png");
    D3D12App::Instance()->GenerateTextureBuffer(
        m_waveNumTex.data(),
        dir + "wave_num.png",
        10,
        { 10,1 });
    m_waveStrTex = D3D12App::Instance()->GenerateTextureBuffer(dir + "wave_str.png");

    //モーションワーク設定
    Motion3D motion;
    motion.m_startPos = { 15.0f,5.0f,0.0f };
    motion.m_startTarget = { 1,0,0 };
    motion.m_endTarget = { 0,0,1 };
    motion.m_interval = 40.0f;
    motion.m_easeParam.m_changeType = Out;
    motion.m_easeParam.m_easeType = Circ;
    m_appearScreenWork.Add(motion);

    motion.m_startTarget = motion.m_endTarget;
    motion.m_endTarget = { -1,0,0 };
    motion.m_endPos = { -15.0f,5.0f,0.0f };
    motion.m_interval = 40.0f;
    motion.m_easeParam.m_changeType = In;
    motion.m_easeParam.m_easeType = Circ;
    m_disappearScreenWork.Add(motion);

    Motion2D motion2D;
    motion2D.m_startPos = { 1280.0f,680.0f };
    motion2D.m_endPos = { 940.0f,650.0f };
    motion2D.m_interval = 60.0f;
    motion2D.m_easeParam.m_changeType = Out;
    motion2D.m_easeParam.m_easeType = Circ;
    m_screenWaveDraw.Add(motion2D);

    motion2D.m_startPos = motion2D.m_endPos;
    motion2D.m_endPos.x = 880.0f;
    motion2D.m_interval = 120.0f;
    motion2D.m_easeParam.m_changeType = In;
    motion2D.m_easeParam.m_easeType = Liner;
    m_screenWaveDraw.Add(motion2D);

    motion2D.m_startPos = motion2D.m_endPos;
    motion2D.m_endPos = { 0.0f,680.0f };
    motion2D.m_interval = 30.0f;
    motion2D.m_easeParam.m_changeType = In;
    motion2D.m_easeParam.m_easeType = Circ;
    m_screenWaveDraw.Add(motion2D);
    
    m_waitInterval = m_screenWaveDraw.GetFullInterval() - 10.0f;
}
