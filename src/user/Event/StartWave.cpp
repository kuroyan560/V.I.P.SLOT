#include "StartWave.h"
#include"GameCamera.h"
#include"Screen.h"
#include"Object.h"
#include"Angle.h"

void StartWave::OnStart()
{
    //�o��X�e�[�^�X
    m_status = APPEAR;
    //�`��t���O�I����
    m_referScreen.lock()->SetDrawFlg(true);
    //�o�ꃂ�[�V�������[�N�J�n
    m_appearScreenWork.Start(true);

    m_referScreen.lock()->m_modelObj->m_transform.SetRotate(Vec3<float>::GetYAxis(), m_ySpinRadianMax);

    //�G���o�������Ȃ�
    m_enemyEmit = false;
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

        //�X�N���[���̉�]
        float ySpin = KuroMath::Ease(Out, Back, m_appearScreenWork.GetTimeRate(), m_ySpinRadianMax, 0.0f);
        screenTransform.SetRotate(Vec3<float>::GetYAxis(), ySpin);

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

        //�X�N���[���̉�]
        float ySpin = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 0.0f, -m_ySpinRadianMax);
        screenTransform.SetRotate(Vec3<float>::GetYAxis(), ySpin);

        //�X�P�[���ύX
        float scale = KuroMath::Ease(In, Back, m_disappearScreenWork.GetTimeRate(), 1.0f, 0.0f);
        screenTransform.SetScale(scale);
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
