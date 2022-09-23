#include "MarkingSystem.h"
#include"ActPoint.h"
#include"KuroFunc.h"
#include"Camera.h"
#include"KuroEngine.h"
#include"DrawFunc2D.h"
#include"AudioApp.h"
#include"Importer.h"

MarkingSystem::MarkingSystem()
{
	D3D12App::Instance()->GenerateTextureBuffer(m_reticleTex.data(), "resource/user/img/markingReticle.png", 2, { 2,1 });
	D3D12App::Instance()->GenerateTextureBuffer(m_markingNumTex.data(), "resource/user/img/markingNumber.png", 10, { 10,1 });
	m_markingSE = AudioApp::Instance()->LoadAudio("resource/user/sound/marking.wav");
	m_markingMaxSE = AudioApp::Instance()->LoadAudio("resource/user/sound/markingMax.wav");
	m_predictionLineModel = Importer::Instance()->LoadModel("resource/user/model/", "PredictionLine_Single.glb");
}

void MarkingSystem::Init()
{
	for (auto& markPt : m_markingPointArray)
	{
		if (markPt.expired())continue;
		markPt.lock()->InitMarkingCount();
	}
	m_markingPointArray.clear();
	m_sumMarkingCount = 0;
}

void MarkingSystem::Update(Camera& Cam, const Vec2<float>& WinSize, bool MarkingInput, Vec3<float>PlayerPos, int MaxMarkingCount)
{
	m_nowTargetIdx = -1;
	auto winCenter = WinSize / 2.0f;

	//�J�����������Ă������
	auto camForward = Cam.GetForward();

	//�}�[�L���O�ΏۂɂȂ肤��|�C���g��z��ɋL�^
	m_canMarkingPointArray.clear();
	for (auto& p : ActPoint::GetActPointArray())
	{
		//�ғ����łȂ����́A�}�[�L���O�ΏۂłȂ����̂�����
		if (p.expired() || !p.lock()->IsActive() || !p.lock()->IsCanMarking())continue;

		//�}�[�L���O�\�ȋ߂��łȂ�
		auto dist = PlayerPos.Distance(p.lock()->GetPosOn3D());
		if (m_canMarkingDist < dist)continue;

		//�J��������ΏۂɌ����Ẵx�N�g��
		auto camToTargetVec = (p.lock()->GetPosOn3D() - Cam.GetPos()).GetNormal();
		//�J�����������Ă�������ɑΏۂ����邩
		if (camForward.Dot(camToTargetVec) < 0)continue;

		m_canMarkingPointArray.emplace_back(p);
	}

	//�ŒZ�����L�^�p
	float nearestDist = std::numeric_limits<float>::max();

	for (int ptIdx = 0; ptIdx < m_canMarkingPointArray.size(); ++ptIdx)
	{
		std::shared_ptr<ActPoint>actPt = m_canMarkingPointArray[ptIdx].lock();

		//�|�C���g�̃X�N���[�����W���Z�o
		auto screenPos = actPt->GetPosOn2D(Cam.GetViewMat(), Cam.GetProjectionMat(), WinSize);

		Vec2<float>addtionalRange = WinSize * m_canMarkingAdditinalRangeRate;
		//�}�[�L���O�o����͈͂ɂ��邩
		if (screenPos.x < -addtionalRange.x)continue;
		else if (WinSize.x + addtionalRange.x < screenPos.x)continue;
		if (screenPos.y < -addtionalRange.y)continue;
		else if (WinSize.y + addtionalRange.y < screenPos.y)continue;

		//��ʒ�������̋���
		auto dist = screenPos.Distance(winCenter);

		//�ŒZ�����Ȃ�}�[�L���O��Ƃ��ċL�^
		if (dist < nearestDist)
		{
			m_nowTargetIdx = ptIdx;
			nearestDist = dist;
		}
	}

	//�L���ȃ}�[�L���O�悪�Ȃ��Ȃ珈���I���
	if (m_nowTargetIdx == -1)return;

	auto nowMarkingTarget = m_canMarkingPointArray[m_nowTargetIdx];

	//�}�[�L���O����
	if (MarkingInput	//����
		&& m_sumMarkingCount < MaxMarkingCount	//�}�[�L���O�����񐔐���
		&& !nowMarkingTarget.lock()->IsMaxMarking())		//��}�[�L���O�񐔐���
	{
		nowMarkingTarget.lock()->OnMarkingTrigger();
		m_sumMarkingCount++;
		if (MaxMarkingCount <= m_sumMarkingCount)
		{
			AudioApp::Instance()->PlayWave(m_markingMaxSE);
		}
		else
		{
			AudioApp::Instance()->PlayWave(m_markingSE);
		}
	}

	//�}�[�L���O�ςŃ|�C���^�z��ɂȂ��Ȃ�ǉ�
	if (nowMarkingTarget.lock()->GetMarkingCount())
	{
		int nowTargetID = nowMarkingTarget.lock()->GetID();
		auto result = std::find_if(m_markingPointArray.begin(), m_markingPointArray.end(), [nowTargetID](std::weak_ptr<ActPoint> p)
			{
				return p.lock()->GetID() == nowTargetID;
			});
		if (result == m_markingPointArray.end())
		{
			//�\�����̐���
			if (!m_markingPointArray.empty())
			{
				//�P�O�̑Ώ�
				auto before = m_markingPointArray.back();
				//���b�V���̗\�����i�z��t���j
				m_predictionLines.emplace_back(before.lock()->GetPosOn3D(), nowMarkingTarget.lock()->GetPosOn3D(), m_predictionLinePosSpan);
			}
			m_markingPointArray.emplace_back(nowMarkingTarget);
		}
	}
}

#include"DrawFunc3D.h"
void MarkingSystem::DrawPredictionLine(Camera& Cam, Vec3<float>PlayerPos)
{
	//�v���C���[�̍��W�I�t�Z�b�g
	PlayerPos.y += 6.0f;
	
	//�v���C���[����Ō�ɒǉ����ꂽ�^�[�Q�b�g�Ɍ����Đ��`��
	if (!m_markingPointArray.empty())
	{
		DrawFunc3D::DrawLine(
			Cam,
			m_markingPointArray.back().lock()->GetPosOn3D(),
			PlayerPos,
			Color(41, 239, 240, 150),
			0.1f);
	}

	for (auto& predictionLine : m_predictionLines)
	{
		predictionLine.Draw(Cam, m_predictionLineModel);
	}
}

void MarkingSystem::DrawHUD(Camera& Cam, const Vec2<float>& WinSize)
{
	//�}�[�L���O��
	for (int ptIdx = 0; ptIdx < m_canMarkingPointArray.size(); ++ptIdx)
	{
		if (m_canMarkingPointArray[ptIdx].expired())continue;

		std::shared_ptr<ActPoint>actPt = m_canMarkingPointArray[ptIdx].lock();

		bool isNowTarget = (ptIdx == m_nowTargetIdx);

		//�摜�ԍ�
		int texIdx = isNowTarget ? static_cast<int>(RETICLE::ON) : static_cast<int>(RETICLE::OFF);

		//��ʊO�ɂ���Ƃ����摜����ʊO�ɂ����Ȃ��悤�ɂ���
		const float expRate = isNowTarget ? 1.0f : 0.7f;
		const Vec2<float> halfGraphSize = m_reticleTex[texIdx]->GetGraphSize().Float() * expRate / 2.0f;

		auto screenPos = actPt->GetPosOn2D(Cam.GetViewMat(), Cam.GetProjectionMat(), WinSize);
		if (screenPos.x - halfGraphSize.x < 0)screenPos.x = halfGraphSize.x;
		else if (WinSize.x < screenPos.x + halfGraphSize.x)screenPos.x = WinSize.x - halfGraphSize.x;
		if (screenPos.y - halfGraphSize.y < 0)screenPos.y = halfGraphSize.y;
		else if (WinSize.y < screenPos.y + halfGraphSize.y)screenPos.y = WinSize.y - halfGraphSize.y;

		DrawFunc2D::DrawRotaGraph2D(screenPos, { expRate,expRate }, 0.0f, m_reticleTex[texIdx]);

		//���݂̃}�[�L���O�Ώۂ̃}�[�L���O�񐔕`��
		if (isNowTarget)
		{
			auto target = m_canMarkingPointArray[ptIdx].lock();
			DrawFunc2D::DrawNumber2D(target->GetMarkingCount(), screenPos, m_markingNumTex, { 1.2f,1.2f }, -10.0f,
				HORIZONTAL_ALIGN::CENTER, VERTICAL_ALIGN::CENTER);
		}
	}
}

void MarkingSystem::PredictionLine::Init(Vec3<float> From, Vec3<float> To, float SpanDist)
{
	//�����v��
	float dist = From.Distance(To);
	//����ݒu���鐔
	int arrowNum = dist / SpanDist;
	//�����擾
	Vec3<float> dir = (To - From).GetNormal();
	//��]�p�x�Ԋu
	Angle angleSpan = Angle::ROUND() / arrowNum;

	//�g�����X�t�H�[������
	for (int arrowIdx = 0; arrowIdx < arrowNum; ++arrowIdx)
	{
		auto pos = From + dir * arrowIdx * SpanDist;
		Transform newTransform;
		newTransform.SetPos(pos);
		newTransform.SetScale(3.0f);
		m_transformArray.push_back(newTransform);
	}
	m_directionPoint = To;

	//��Ԃ����������ꍇ�́A���Ԓn�_�ɖ��ݒu
	if (arrowNum == 0)
	{
		auto pos = From + dir * dist / 2.0f;
		Transform newTransform;
		newTransform.SetPos(pos);
		newTransform.SetScale(3.0f);
		m_transformArray.push_back(newTransform);
	}
}
;

#include"DrawFunc3D.h"
void MarkingSystem::PredictionLine::Draw(Camera& Cam, std::shared_ptr<Model>& Model)
{
	//�ғ����łȂ�
	if (!m_isActive)return;

	//������]������
	m_spinAngle += Angle(1);

	//��]��̏�x�N�g���Z�o
	Vec3<float>up = KuroMath::TransformVec3({ 0,1,0 }, KuroMath::RotateMat({ 0,0,1 }, m_spinAngle));

	for (auto& transform : m_transformArray)
	{
		//�ڕW�n�_����������Ԃŉ�]
		transform.SetLookAtRotate(m_directionPoint, up);
		//�`��
		DrawFunc3D::DrawNonShadingModel(Model, transform, Cam);
	}
}
