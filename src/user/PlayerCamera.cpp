#include "PlayerCamera.h"
#include"Camera.h"
#include"Transform.h"
#include"ActPoint.h"
#include"DrawFuncBillBoard.h"
#include"KuroEngine.h"

//�J�����ʒu��������
static float HEIGHT_MIN = 3.63f;
static float HEIGHT_DEFAULT = 11.397f;
static float HEIGHT_MAX = 17.495f;

//�v���C���[�Ƃ̋�������
static float DISTANCE_MIN = 4.040f;
static float DISTANCE_MAX = 15.791f;
static float DISTANCE_DEFAULT = DISTANCE_MAX;

//���ۂ̃v���C���[�̈ʒu�ƃ��b�N�I������ʒu�̃I�t�Z�b�g
static float TARGET_DIST_OFFSET = 7.363f;
static float TARGET_HEIGHT_OFFSET = 10.549f;

//���b�N�I���\�ȃJ�������W�Ƃ̋����̏���i�RD�j
static float CAN_ROCK_ON_DIST_3D = 80.0f;
//���b�N�I���\�ȉ�ʒ����Ƃ̋����̏���i�QD�j
static float CAN_ROCK_ON_DIST_2D = 400.0f;
//���b�N�I�����e�p�x
static Angle ROCK_ON_ANGLE_RANGE = Angle(5);

void PlayerCamera::SetRockOnTarget(std::shared_ptr<ActPoint> Target)
{
	m_rockOnPoint = Target;
	m_rockOnCollider->SetActive(Target != nullptr);
}

void PlayerCamera::CalculatePos(const Transform& Player)
{
	static const float LERP_RATE_MIN = 0.12f;
	static const float LERP_RATE_MAX = 0.19f;

	Vec3<float>angleVec = { cos(m_posAngle),0.0f,sin(m_posAngle) };

	//�v���C���[�ƃJ�����̋���
	auto result = Player.GetPos() + angleVec * m_dist;
	result.y += m_height;

	float rate = (m_dist - DISTANCE_MIN) / (DISTANCE_MAX - DISTANCE_MIN);
	float lerpRate = 0.0f;
	lerpRate = KuroMath::Lerp(LERP_RATE_MAX, LERP_RATE_MIN, rate);
	result = KuroMath::Lerp(m_cam->GetPos(), result, lerpRate);

	m_cam->SetPos(result);

	//�v���C���[�����b�N�I��
	auto rockOnPlayerPos = Player.GetPos();
	auto forward = m_cam->GetForward();
	rockOnPlayerPos += forward * TARGET_DIST_OFFSET;
	rockOnPlayerPos.y += TARGET_HEIGHT_OFFSET;
	//m_cam->SetTarget(rockOnPlayerPos);
	auto nowTargetPos = m_cam->GetTarget();
	m_cam->SetTarget(KuroMath::Lerp(nowTargetPos, rockOnPlayerPos, 0.6f));
}

void PlayerCamera::LookAtPlayersFront(const Transform& Player)
{
	m_height = HEIGHT_DEFAULT;
	m_dist = DISTANCE_DEFAULT;
	const auto playerPos = Player.GetPos();
	const auto playerBackVec = playerPos - Player.GetFront();
	m_posAngle = KuroMath::GetAngle({ playerPos.x,playerPos.z }, { playerBackVec.x,playerBackVec.z });
}

void PlayerCamera::RockOnTargeting(Vec3<float> PlayerPos)
{
	//���b�N�I���Ώۂ���A�N�e�B�u�ɂȂ����烍�b�N�I���������ďI��
	if (m_rockOnPoint.expired() || !m_rockOnPoint.lock()->IsActive())
	{
		SetRockOnTarget(nullptr);
		return;
	}

	//���b�N�I���Ώۍ��W
	const auto rockPos = m_rockOnPoint.lock()->GetPosOn3D();
	//���g�i�J�����j�̍��W
	const auto camPos = m_cam->GetPos();

	//���b�N�I���Ώۂɍ��킹�ăJ�����𓮂���
	Angle toAngle = KuroMath::GetAngle({ rockPos.x,rockPos.z }, { PlayerPos.x,PlayerPos.z });

	//0 ~ 360�͈̔͂Ɏ��߂�
	toAngle.Normalize();
	m_posAngle.Normalize();

	//�ڕW�p�x�܂ł̉����h�~
	if (toAngle < m_posAngle && Angle::PI() < m_posAngle - toAngle)
	{
		m_posAngle -= Angle::ROUND();
	}
	else if (m_posAngle < toAngle && Angle::PI() < toAngle - m_posAngle)
	{
		m_posAngle += Angle::ROUND();
	}

	//���e�p�x���ڕW�l�Ƃ̊p�x�̍����傫�����
	if (ROCK_ON_ANGLE_RANGE < Angle(abs(m_posAngle - toAngle)))
	{
		//�ڕW�l�ɋ߂Â�
		m_posAngle = KuroMath::Lerp(m_posAngle, toAngle, 0.15f);
	}
}

PlayerCamera::PlayerCamera()
{
	m_cam = std::make_shared<Camera>("PlayerCamera");
	m_canRockOnDist3D = CAN_ROCK_ON_DIST_3D;
	m_canRockOnDist2D = CAN_ROCK_ON_DIST_2D;
	m_rockOnAngleRange = ROCK_ON_ANGLE_RANGE;
	m_reticleTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/reticle.png");

	//���b�N�I����܂ł̐����̃R���C�_�[
	m_rockOnColLine = std::make_shared<CollisionLine>(Vec3<float>(0, 0, 0), Vec3<float>(0, 1, 0), 1.0f, nullptr);
	m_rockOnCollider = Collider::Generate(
		"Player Camera's Rock On Ray Collider",
		m_rockOnColLine,this, 
		COLLIDER_ATTRIBUTE::COLLIDER_NONE, COLLIDER_ATTRIBUTE::ENEMY | COLLIDER_ATTRIBUTE::TERRIAN);
}

void PlayerCamera::Init(const Transform& Player)
{
	m_posAngle = Angle(-90);	//�v���C���[���
	m_height = HEIGHT_DEFAULT;
	m_dist = DISTANCE_DEFAULT;

	CalculatePos(Player);

	//�������b�N�I�����Ă��Ȃ�
	SetRockOnTarget(nullptr);
}

void PlayerCamera::Update(const Transform& Player, Vec2<float> InputVec)
{
	//�p�x�̕ω���
	const Angle angleAmount = Angle(2 * (m_mirrorX ? 1 : -1));

	//�㉺���͂̕ω���
	const float verticalAmount = 0.2f * (m_mirrorY ? 1 : -1);

	//�E�X�e�B�b�N����
	Vec2<float> inputVec = InputVec;

	//���b�N�I���Ώ�
	auto rockOnPoint = m_rockOnPoint.lock();

	//���b�N�I����
	if (rockOnPoint)
	{
		RockOnTargeting(Player.GetPos());
	}
	//�E�X�e�B�b�N���͂���
	else if (!inputVec.IsZero())
	{
		//���E��U��
		m_posAngle += angleAmount * inputVec.x;

		//�E�X�e�B�b�N�㉺
		//�����������ɂ���Ƃ��㉺�X�e�B�b�N���͂��������͂ɂȂ�
		if (m_dist < DISTANCE_MAX)
		{
			m_dist += verticalAmount * inputVec.y;
		}
		//�����̓��͂ɂȂ�(�c��U��)
		else
		{
			m_height += verticalAmount * inputVec.y;
		}

		//���߂������̍������͂��������͂ɕϊ�
		if (m_height < HEIGHT_MIN)
		{
			m_dist += m_height - HEIGHT_MIN;
		}
		//���߂������̋������͂��������͂ɕϊ�
		if (DISTANCE_MAX < m_dist)
		{
			m_height += m_dist - DISTANCE_MAX;
		}
	}

	//�N�����v
	m_dist = std::clamp(m_dist, DISTANCE_MIN, DISTANCE_MAX);
	m_height = std::clamp(m_height, HEIGHT_MIN, HEIGHT_MAX);
	m_posAngle.Normalize();

	//�J�����ʒu�v�Z
	CalculatePos(Player);

	//�v�Z��A�����蔻��p�̐������X�V
	if (rockOnPoint)
	{
		auto camPos = m_cam->GetPos();
		//���b�N�I�������蔻������̎n�_�̓J�������W
		m_rockOnColLine->m_start = camPos;;
		//���b�N�I����Ɍ�����
		auto differ = rockOnPoint->GetPosOn3D() - camPos;
		m_rockOnColLine->m_dir = differ.GetNormal();
		m_rockOnColLine->m_len = differ.Length() - rockOnPoint->GetRayOffsetWhenRockOn();
	}
}

#include"D3D12App.h"
void PlayerCamera::Draw(Camera& NowCam)
{
	if(m_rockOnPoint.lock())
	{
		DrawFuncBillBoard::Graph(NowCam, m_rockOnPoint.lock()->GetPosOn3D(), Vec2<float>(3.0f, 3.0f), m_reticleTex);
	}
}

void PlayerCamera::RockOn(const Transform& Player)
{
	//�ғ����łȂ����́A���b�N�I���ΏۂłȂ����̂�����
	std::vector<std::weak_ptr<ActPoint>>actPointArray;
	for (auto& p : ActPoint::GetActPointArray())
	{
		if (p.expired() || !p.lock()->IsActive() || !p.lock()->IsCanRockOn())continue;
		actPointArray.emplace_back(p);
	}

	//�Z�o�������b�N�I���Ώۂ̃|�C���^�i�[��
	std::shared_ptr<ActPoint>newRockOnPoint = nullptr;
	float nearestDist2D = 100000.0f;

	//�Q�ԖڂɓK���Ă��郍�b�N�I���Ώۂ̃|�C���^�i�[��
	std::shared_ptr<ActPoint>secondRockOnPoint = nullptr;

	for (auto& p : actPointArray)
	{
		auto worldPos = p.lock()->GetPosOn3D();
		//�J�������W�Ɨ��ꂷ���Ă���
		if (CAN_ROCK_ON_DIST_3D < m_cam->GetPos().Distance(worldPos))continue;

		auto screenPos = p.lock()->GetPosOn2D(m_cam->GetViewMat(), m_cam->GetProjectionMat(), WinApp::Instance()->GetExpandWinSize());
		float dist2D = WinApp::Instance()->GetExpandWinCenter().Distance(screenPos);
		//��ʒ����Ɨ��ꂷ���Ă���
		if (CAN_ROCK_ON_DIST_2D < dist2D)continue;

		//���Ɋi�[����Ă�����̂���ʒ����ɋ߂��Ȃ�X�V
		if (dist2D < nearestDist2D)
		{
			secondRockOnPoint = newRockOnPoint;
			newRockOnPoint = p.lock();
			nearestDist2D = dist2D;
		}
	}

	//�K�������肪���Ȃ�
	if (!newRockOnPoint)
	{
		//���Ƀ��b�N�I���ςȂ�������ďI���
		if (m_rockOnPoint.lock())
		{
			SetRockOnTarget(nullptr);
			return;
		}
		//���b�N�I�����Ă��Ȃ��Ȃ琳�ʂ�����
		else
		{
			LookAtPlayersFront(Player);
			return;
		}
	}

	//���X���b�N�I���Ώۂ����Ȃ����A���b�N�I���Ώۂ������ꍇ�Ⴄ����Ȃ�ݒ肵�ďI���
	if (!m_rockOnPoint.lock() || m_rockOnPoint.lock() != newRockOnPoint)
	{
		SetRockOnTarget(newRockOnPoint);
		return;
	}

	//���Ƀ��b�N�I�����Ă���ΏۂƓ�������Ȃ�A�Q�Ԗڂ̃��b�N�I���Ώۂ��̗p
	if (secondRockOnPoint)
	{
		SetRockOnTarget(secondRockOnPoint);
		return;
	}

	//�Q�Ԗڂ̑Ώۂ����Ȃ��Ȃ烍�b�N�I������
	SetRockOnTarget(nullptr);
}

#include"imguiApp.h"
void PlayerCamera::OnImguiDebug()
{
	return;
	ImGui::Begin("PlayerCamera");

	ImGui::Text("Height");
	ImGui::SliderFloat("h_min", &HEIGHT_MIN, 0.1f, HEIGHT_DEFAULT);
	ImGui::SliderFloat("h_default", &HEIGHT_DEFAULT, HEIGHT_MIN, HEIGHT_MAX);
	ImGui::SliderFloat("h_max", &HEIGHT_MAX, HEIGHT_DEFAULT, 20.0f);
	ImGui::Separator();

	ImGui::Text("Distance");
	ImGui::SliderFloat("d_min", &DISTANCE_MIN, 0.1f, DISTANCE_MAX);
	if (ImGui::SliderFloat("d_max", &DISTANCE_MAX, DISTANCE_MIN, 20.0f))DISTANCE_DEFAULT = DISTANCE_MAX;
	ImGui::Separator();

	ImGui::Text("TargetOffset");
	ImGui::SliderFloat("t_distance", &TARGET_DIST_OFFSET, 0.0f, 20.0f);
	ImGui::SliderFloat("t_height", &TARGET_HEIGHT_OFFSET, 0.0f, 20.0f);

	ImGui::End();
}
