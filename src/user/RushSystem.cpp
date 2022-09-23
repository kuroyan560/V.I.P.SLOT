#include "RushSystem.h"
#include"KuroMath.h"
#include"ActPoint.h"
#include"MarkingSystem.h"
#include"Player.h"
#include"LevelParameters.h"

const std::string RushSystem::s_cameraKey = "RushCamera";
std::shared_ptr<Camera>RushSystem::s_cam;

void RushSystem::Init()
{
	m_markingPointArray.clear();
}

void RushSystem::Update(Transform& PlayerTransform, const float& Gravity)
{
	if (!IsActive())return;

	//�v���C���[�̍��W�擾
	auto playerPos = PlayerTransform.GetPos();
	//�z��Ō���̃}�[�L���O�Ώ�
	auto target = m_markingPointArray[m_rushTargetPointIdx].lock();
	//�}�[�L���O�Ώۂ̍��W�擾
	auto targetPos = target->GetPosOn3D();
	//�}�[�L���O�Ώۂւ̃x�N�g���擾
	auto vecToTarget = (targetPos - playerPos).GetNormal();

	//�����b�V���̃J�������[�N
	if (m_strongRushFlg)
	{
		//���b�V���S�̂̌��݂̎��Ԋ���
		float timeRate = static_cast<float>(m_totalRushTimer) / m_totalRushTime;
		m_strongRushCamInfo.Update(timeRate);
	}
	if (m_totalRushTimer < m_totalRushTime) m_totalRushTimer++;

	//�P���b�V���ŏ��̃t���[��
	if (m_timer++ <= 0)
	{
		//�ʏ탉�b�V���̃J�����i��_�Ȃ̂ōŏ��̃t���[���̂ݏ����j
		if (!m_strongRushFlg)
		{
			//�J�������W�v�Z
			Vec3<float>camPos = targetPos;
			auto camPosVec = vecToTarget;
			camPosVec.y = 0.0f;

			Angle vecSpinAngle = Angle(KuroFunc::GetRand(m_camToTargetVecSpinDegreeMax) * KuroFunc::GetRandPlusMinus());
			camPosVec = KuroMath::TransformVec3(camPosVec, Vec3<float>(0, 1, 0), vecSpinAngle);
			camPos += camPosVec * m_camDistToTarget;	//��ʑ̂Ƌ��������
			camPos.y += m_camHeightOffset;					//�������グ��
			s_cam->SetPos(camPos);
			//�����_�ݒ�
			auto camTargetPos = targetPos + Vec3<float>(0, m_camTargetHeightOffset, 0);
			s_cam->SetTarget(camTargetPos);
		}

		//�v���C���[�̌����ύX
		PlayerTransform.SetLookAtRotate(playerPos + Vec3<float>(vecToTarget.x, 0.0f, vecToTarget.z));
		//�C�[�W���O�̏I�����W�v�Z
		m_easingEndPos = targetPos - vecToTarget * m_distToTarget;
	}
	
	//�C�[�W���OOut��ԂŃv���C���[�ړ�
	Vec3<float>newPlayerPos = KuroMath::Ease(
		EASE_CHANGE_TYPE::Out,
		EASING_TYPE::Exp,
		static_cast<float>(m_timer),
		static_cast<float>(m_oneRushTime),
		m_easingStartPos,
		m_easingEndPos);
	//�v���C���[�̍��W�K�p
	PlayerTransform.SetPos(newPlayerPos);

	//���b�V���ɂ�鐁����΂�
	float oneRushTimeRate = static_cast<float>(m_timer) / m_oneRushTime;
	if (!m_blow && m_blowTiming < oneRushTimeRate)
	{
		//ActPoint�ɂӂ��Ƃ΂��ʒm
		m_markingPointArray[m_rushTargetPointIdx].lock()->OnRushBlowTrigger();
		//�t���O�I��
		m_blow = true;
	}

	m_blowRate = (oneRushTimeRate - m_blowTiming) / (1.0f - m_blowTiming);

	//�P���b�V���I��
	if (m_oneRushTime <= m_timer)
	{
		//�C�[�W���O�X�^�[�g�l�ύX
		m_easingStartPos = m_easingEndPos;
		//���b�V���ΏەύX
		m_rushTargetPointIdx--;
		
		//�܂��Ώۂ��Q�ȏ゠��Ƃ�
		if (0 < m_rushTargetPointIdx)
		{
			m_involvedObjsTargetPos = m_markingPointArray[m_rushTargetPointIdx - 1].lock()->GetPosOn3D();
		}
		//�Ō�̃��b�V��
		else if (m_rushTargetPointIdx == 0)
		{
			//�eActPoint�ɒʒm
			for (auto& pt : m_markingPointArray)pt.lock()->OnLastRushTrigger();
		}
		//�Ώۂ̑SActPoint�ւ̃��b�V������������
		else
		{
			//�z��N���A
			m_markingPointArray.clear();
		}
		//�^�C�}�[���Z�b�g
		m_timer = 0;
		//�ӂ��Ƃ΂��t���O���Z�b�g
		m_blow = false;
		//�ӂ���΂��̐������Z�b�g
		m_blowRate = 0.0f;
	}
}

void RushSystem::Excute(MarkingSystem& MarkingSys, const Vec3<float>& PlayerPos, bool& IsOneShot, const float& OneShotChargeRate)
{
	//�P�����b�V��
	if (IsOneShot)
	{
		//���ݏ����ɍ����Ώۂ����݂��Ȃ��Ȃ�X���[
		if (MarkingSys.m_nowTargetIdx == -1)return;

		//�P�����b�V���̑Ώۂ̂݊i�[
		m_markingPointArray = { MarkingSys.m_canMarkingPointArray[MarkingSys.m_nowTargetIdx] };
	}
	else
	{
		//�}�[�L���O�����Ώۂ̔z����R�s�[
		m_markingPointArray = MarkingSys.m_markingPointArray;
	}

	//�}�[�L���O�Ώۂ̐�
	int targetNum = static_cast<int>(m_markingPointArray.size());

	//�P�̂Ȃ�P�����b�V���t���O��ON�ɕύX
	if (targetNum == 1)IsOneShot = true;

	//�}�[�L���O�Ώۂ̐��̕ψʊ
	int markObjNumIndicator = targetNum;

	//�����A����𒴂��Ȃ��悤�ɂ���
	if (markObjNumIndicator < m_markCountIndicatorMin)markObjNumIndicator = m_markCountIndicatorMin;
	if (m_markCountIndicatorMax < markObjNumIndicator)markObjNumIndicator = m_markCountIndicatorMax;

	//�}�[�L���O�Ώې��̔䗦
	float markingCountRate = KuroMath::Lerp(0.0f, 1.0f,
		static_cast<float>((markObjNumIndicator - m_markCountIndicatorMin)) 
		/ static_cast<float>((m_markCountIndicatorMax - m_markCountIndicatorMin)));

	//�䗦�����ȏォ�P�����b�V������Ȃ��Ƃ������b�V�����o�J�����t���O�I��
	m_strongRushFlg = (m_strongRushThreshold <= markingCountRate) && !IsOneShot;
	m_strongRushCamInfo.Init(m_markingPointArray, PlayerPos);

	//�P���b�V���ɂ����鎞�Ԃ��v���i�}�[�L���O�Ώۂ������Ƒ��x�㏸�j
	m_oneRushTime = static_cast<int>(KuroMath::Lerp(
		static_cast<float>(m_oneRushTimeMin), 
		static_cast<float>(m_oneRushTimeMax), 
		markingCountRate));
	//���b�V���S�̂ɂ����鎞��
	m_totalRushTime = m_oneRushTime * targetNum;

	//�P���b�V���̉Η͂��v���i�}�[�L���O�Ώۂ������ƉΗ͏㏸�j
	if (IsOneShot)
	{
		m_oneRushDamage = static_cast<int>(KuroMath::Lerp(
			static_cast<float>(LevelParameters::GetPlayer().m_singleRushDamageMin),
			static_cast<float>(LevelParameters::GetPlayer().m_singleRushDamageMax),
			OneShotChargeRate
		));
	}
	else
	{
		m_oneRushDamage = static_cast<int>(KuroMath::Lerp(
			static_cast<float>(LevelParameters::GetPlayer().m_rushDamageMin),
			static_cast<float>(LevelParameters::GetPlayer().m_rushDamageMax),
			markingCountRate));
	}

	//�^�C�}�[�̃��Z�b�g
	m_timer = 0;
	m_totalRushTimer = 0;

	//���b�V���J�n���̃v���C���[�̍��W��ۑ����Ă���
	m_easingStartPos = PlayerPos;

	//�Ō�����ŏ��̃��b�V���Ώ�
	m_rushTargetPointIdx = static_cast<int>(m_markingPointArray.size()) - 1;

	//�Q�̈ȏ�̃��b�V���Ώۂ�����Ƃ�
	if (0 < m_rushTargetPointIdx)
	{
		m_involvedObjsTargetPos = m_markingPointArray[m_rushTargetPointIdx - 1].lock()->GetPosOn3D();
	}

	//�ӂ���΂��t���O���Z�b�g
	m_blow = false;

	//�ӂ���΂��̐������Z�b�g
	m_blowRate = 0.0f;
}

#include"imguiApp.h"
void RushSystem::ImguiDebug()
{
	static ImVec4 RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::Text("- RushParameters -");
	ImGui::TextColored(IsActive() ? RED : WHITE, "timer : { %d }", m_timer);
	ImGui::Text("oneRushTime : { %d }", m_oneRushTime);

	if (ImGui::DragInt("OneRushTime_Min", &m_oneRushTimeMin))
	{
		if (m_oneRushTimeMin < 0)m_oneRushTimeMin = 0;	//�}�C�i�X�h�~
		if (m_oneRushTimeMax < m_oneRushTimeMin)m_oneRushTimeMin = m_oneRushTimeMax;	//����𒴂��Ȃ�
	}
	if (ImGui::DragInt("OneRushTime_Max", &m_oneRushTimeMax))
	{
		if (m_oneRushTimeMax < m_oneRushTimeMin)m_oneRushTimeMax = m_oneRushTimeMin;	//�����𒴂��Ȃ�
	}

	//���b�V���̂ӂ���΂���
	if (ImGui::DragFloat("BlowPower", &m_blowPowerMax, 0.05f) && m_blowPowerMax < 0.0f)	//�}�C�i�X�h�~
	{
		m_blowPowerMax = 0.0f;
	}

	//���b�V���Ő�����΂��^�C�~���O
	ImGui::DragFloat("BlowTiming", &m_blowTiming, 0.05f, 0.0f, 1.0f);

	//�ӂ���΂��ꂽ�Ƃ��̍��W�̍����I�t�Z�b�g�ő�
	ImGui::DragFloat("BlowPowerY", &m_blowPowerY, 0.05f);

	//�ӂ���΂��ꂽ�Ƃ��̏d�͂̓x����
	ImGui::DragFloat("BlowGravityRate", &m_blowGravityRate, 0.05f);


	if (ImGui::DragInt("MarkCountIndicator_Min", &m_markCountIndicatorMin))
	{
		if (m_markCountIndicatorMin < 0)m_markCountIndicatorMin = 0;	//�}�C�i�X�h�~
		if (m_markCountIndicatorMax < m_markCountIndicatorMin)m_markCountIndicatorMin = m_markCountIndicatorMax;	//����𒴂��Ȃ�
	}
	if (ImGui::DragInt("MarkCountIndicator_Max", &m_markCountIndicatorMax))
	{
		if (m_markCountIndicatorMax < m_markCountIndicatorMin)m_markCountIndicatorMax = m_markCountIndicatorMin;	//�����𒴂��Ȃ�
	}

	if (ImGui::DragFloat("DistToTarget", &m_distToTarget) && m_distToTarget < 0)	//�}�C�i�X�h�~
	{
		m_distToTarget = 0;
	}

	//���b�V���J�������o�֘A
	ImGui::Separator();
	ImGui::Text("- NormalRushCamera -");
	if (ImGui::DragFloat("CamDistToTarget", &m_camDistToTarget) && m_camDistToTarget < 0)	//�}�C�i�X�h�~
	{
		m_camDistToTarget = 0;
	}
	if (ImGui::DragFloat("CamHeightOffset", &m_camHeightOffset) && m_camHeightOffset < 0)	//�}�C�i�X�h�~
	{
		m_camHeightOffset = 0;
	}
	if (ImGui::DragFloat("CamTargetHeightOffset", &m_camTargetHeightOffset) && m_camTargetHeightOffset < 0)	//�}�C�i�X�h�~
	{
		m_camTargetHeightOffset = 0;
	}
	if (ImGui::DragInt("CamToTargetVecSpinDegree", &m_camToTargetVecSpinDegreeMax))
	{
		if (m_camToTargetVecSpinDegreeMax < 0)m_camToTargetVecSpinDegreeMax = 0;	//�}�C�i�X�h�~
		if (360 < m_camToTargetVecSpinDegreeMax)m_camToTargetVecSpinDegreeMax = 360;	//������ߖh�~
	}

	//�����b�V���J�������o�֘A
	ImGui::Separator();
	ImGui::Text("- StrongRushCamera -");
	ImGui::Text("StrongRush : { %s }", m_strongRushFlg ? "true" : "false");
	ImGui::DragFloat("StrongRush_Threshold", &m_strongRushThreshold, 0.05f, 0.0f, 1.0f);
	ImGui::DragFloat("DistMotionRate", &m_strongRushCamInfo.m_distMotionRate, 0.05f, 0.0f, 1.0f);

}

float RushSystem::GetNowBlowPower() const
{
	//�Ō�̃��b�V���̃m�b�N�o�b�N�ł̂ݎg�p����
	assert(IsLastRush());
	return KuroMath::Ease(Out, Circ, m_blowRate, m_blowPowerMax, 0.0f);
}

void RushSystem::StrongRushCamInfo::Init(const std::vector<std::weak_ptr<ActPoint>>& TargetArray, Vec3<float>PlayerPos)
{
	m_ySpinAngle = -m_ySpinAngleOffset;
	m_distToCenter = 0.0f;

	m_spaceMin = TargetArray[0].lock()->GetPosOn3D();
	m_spaceMax = TargetArray[0].lock()->GetPosOn3D();

	//�Ώۂ̍��W�̔z��
	std::vector<Vec3<float>>targetPositions;
	for (auto& target : TargetArray)
	{
		auto pos = target.lock()->GetPosOn3D();
		targetPositions.emplace_back(pos);

		//�ŏ��l�̕ۑ�
		if (pos.x < m_spaceMin.x)m_spaceMin.x = pos.x;
		if (pos.y < m_spaceMin.y)m_spaceMin.y = pos.y;
		if (pos.z < m_spaceMin.z)m_spaceMin.z = pos.z;

		//�ő�l�̕ۑ�
		if (m_spaceMax.x < pos.x)m_spaceMax.x = pos.x;
		if (m_spaceMax.y < pos.y)m_spaceMax.y = pos.y;
		if (m_spaceMax.z < pos.z)m_spaceMax.z = pos.z;
	}

	//�K��̃I�t�Z�b�g�̌��E�𒴂��Ȃ�
	if (-m_distToCenterOffsetMin.x < m_spaceMin.x)m_spaceMin.x = -m_distToCenterOffsetMin.x;
	if (-m_distToCenterOffsetMin.y < m_spaceMin.y)m_spaceMin.y = -m_distToCenterOffsetMin.y;
	if (-m_distToCenterOffsetMin.z < m_spaceMin.z)m_spaceMin.z = -m_distToCenterOffsetMin.z;
	if (m_spaceMax.x < m_distToCenterOffsetMin.x)m_spaceMax.x = m_distToCenterOffsetMin.x;
	if (m_spaceMax.y < m_distToCenterOffsetMin.y)m_spaceMax.y = m_distToCenterOffsetMin.y;
	if (m_spaceMax.z < m_distToCenterOffsetMin.z)m_spaceMax.z = m_distToCenterOffsetMin.z;

	//�ǉ���
	m_spaceMin -= {m_distToCenterAdditional, m_distToCenterAdditional, m_distToCenterAdditional};
	m_spaceMax += {m_distToCenterAdditional, m_distToCenterAdditional, m_distToCenterAdditional};

	//�J���������̒��S���v�Z
	m_center = (m_spaceMax - m_spaceMin) / 2.0f + m_spaceMin;

	//�J���������̊�ƂȂ�x�N�g��
	m_criteriaVec = (m_center - PlayerPos).GetNormal();
	m_criteriaVec.y = 0.0f;
}

void RushSystem::StrongRushCamInfo::Update(const float& Rate)
{
	//�����̕ω���
	float distChangeRate = Rate;
	if (m_distMotionRate < distChangeRate)distChangeRate = m_distMotionRate;

	//����
	m_distToCenter = KuroMath::Ease(
		m_distEaseChangeType,
		m_distEaseType,
		distChangeRate,
		m_distMotionRate,
		0.0f,
		m_spaceMax.Distance(m_center));

	//���W��Y����]
	m_ySpinAngle = KuroMath::Ease(
		m_spinEaseChangeType,
		m_spinEaseType,
		Rate,
		-m_ySpinAngleOffset,
		m_ySpinAngleOffset);

	//�J�����̍��W���v�Z���ċ��߂�
	Vec3<float>camPos = m_center + KuroMath::TransformVec3(m_criteriaVec, { 0,1,0 }, m_ySpinAngle) * m_distToCenter;
	camPos.y += m_heightOffset;
	s_cam->SetPos(camPos);
	s_cam->SetTarget(m_center);
}
