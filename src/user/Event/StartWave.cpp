#include "StartWave.h"
#include"GameCamera.h"
#include"Screen.h"
#include"Object.h"
#include"Angle.h"
#include"WaveMgr.h"
#include"DrawFunc2D.h"

void StartWave::OnStart()
{
    //�o��X�e�[�^�X
    m_status = APPEAR;
    //�`��t���O�I����
    m_referScreen.lock()->SetDrawFlg(true);
    //�o�ꃂ�[�V�������[�N�J�n
    m_appearScreenWork.Start(true);

    //�G���o�������Ȃ�
    m_enemyEmit = false;

    //�E�F�[�u�\���̃X�N���[���ʃ��Z�b�g
    m_wavePosScrollX = 0.0f;

    //�X�N���[���̉�ʃN���A
    m_referScreen.lock()->ClearTarget();
}

void StartWave::OnUpdate()
{
    //�v���C���[�̍��W�擾
    const auto playerPos = InGameMonitor::GetPlayer()->GetCenterPos();
    //�X�N���[���̃g�����X�t�H�[���ւ̎Q�Ǝ擾
    auto& screenTransform = m_referScreen.lock()->m_modelObj->m_transform;
    //���݂̃v���C���[�̈ʒu����ڕW�l���Z�o
    auto targetPos = KuroMath::Lerp(screenTransform.GetPos(), m_defaultPosOffset + playerPos, m_followLerpRate);

    //�e���[�V�������[�N�ɖڕW�l���Đݒ�
    m_appearScreenWork.GetMotion(0).m_endPos = targetPos;
    m_disappearScreenWork.GetMotion(0).m_startPos = targetPos;

    //���[�V�������[�N�X�V
    m_appearScreenWork.Update(1.0f);
    m_disappearScreenWork.Update(1.0f);

    if (m_status == APPEAR)
    {
        //�o�ꃂ�[�V�������[�N����X�N���[���̍��W�K�p
        screenTransform.SetPos(m_appearScreenWork.GetPos());

        //�X�P�[���ύX
        float scale = KuroMath::Ease(Out, Back, m_appearScreenWork.GetTimeRate(), 0.0f, 1.0f);
        screenTransform.SetScale(scale);

        //�o�ꃂ�[�V�������[�N�I����
        if (m_appearScreenWork.IsEnd())
        {
            //�ҋ@��Ԃֈڍs
            m_status = WAIT;
            //�ҋ@���ԃZ�b�g
            m_waitTimer.Reset(m_waitInterval);
            //�E�F�[�u�\���̃��[�V�������[�N�J�n
            m_screenWaveDraw.Start(true);
        }
    }
    else if (m_status == WAIT)
    {
        //�Ǐ]
        screenTransform.SetPos(targetPos);

        //�ҋ@���ԏI����
        if (m_waitTimer.UpdateTimer(1.0f))
        {
            //�ޏ��Ԃֈڍs
            m_status = DISAPPEAR;
            //�ޏꃂ�[�V�������[�N�J�n
            m_disappearScreenWork.Start(true);
        }
    }
    else if (m_status == DISAPPEAR)
    {
        //�ޏꃂ�[�V�������[�N����X�N���[���̍��W�K�p
        screenTransform.SetPos(m_disappearScreenWork.GetPos());

        //�X�P�[���ύX
        float scale = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 1.0f, 0.0f);
        screenTransform.SetScale(scale);
    }

    if (m_status != APPEAR)
    {
        m_screenWaveDraw.Update(1.0f);
        float waveExpY = 1.0f;

        /*--- ��^�X�N���[���ɂɕ`�� ---*/
        //��^�X�N���[���̃����_�[�^�[�Q�b�g�Z�b�g
        m_referScreen.lock()->ClearTarget();
        m_referScreen.lock()->SetTarget();

        //�E�F�[�u�\���X�N���[��
        m_wavePosScrollX -= 2.0f;

        //�E�F�[�u�\���ʒu
        Vec2<float>drawPos = m_screenWaveDraw.GetPos() + Vec2<float>(m_wavePosScrollX, 0.0f);
        DrawFunc2D::DrawRotaGraph2D(
            drawPos,
            { 1.0f,waveExpY },
            0.0f,
            m_waveStrTex
        );

        //�E�F�[�u�����`��ʒu�̃I�t�Z�b�g
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
    //�`��t���O�I�t��
    m_referScreen.lock()->SetDrawFlg(false);
    //�G�o������
    m_enemyEmit = true;
}

bool StartWave::End()
{
    //�ޏ��
    return m_status == DISAPPEAR && m_disappearScreenWork.IsEnd();
}

std::shared_ptr<Camera> StartWave::GetMainCam()
{
    //�Q�[���J���������̂܂ܗ��p
    return m_referGameCam.lock()->GetMainCam();
}

std::shared_ptr<Camera> StartWave::GetSubCam()
{
    //�Q�[���J���������̂܂ܗ��p
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
    //�e�N�X�`���ǂݍ���
    std::string dir = "resource/user/img/ui/screen/";
    m_goodLuckTex = D3D12App::Instance()->GenerateTextureBuffer(dir + "wave_good_luck.png");
    D3D12App::Instance()->GenerateTextureBuffer(
        m_waveNumTex.data(),
        dir + "wave_num.png",
        10,
        { 10,1 });
    m_waveStrTex = D3D12App::Instance()->GenerateTextureBuffer(dir + "wave_str.png");

    //���[�V�������[�N�ݒ�
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
