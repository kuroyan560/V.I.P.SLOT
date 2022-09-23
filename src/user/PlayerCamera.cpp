#include "PlayerCamera.h"
#include"Camera.h"
#include"Transform.h"
#include"ActPoint.h"
#include"DrawFuncBillBoard.h"
#include"KuroEngine.h"

//カメラ位置高さ制限
static float HEIGHT_MIN = 3.63f;
static float HEIGHT_DEFAULT = 11.397f;
static float HEIGHT_MAX = 17.495f;

//プレイヤーとの距離制限
static float DISTANCE_MIN = 4.040f;
static float DISTANCE_MAX = 15.791f;
static float DISTANCE_DEFAULT = DISTANCE_MAX;

//実際のプレイヤーの位置とロックオンする位置のオフセット
static float TARGET_DIST_OFFSET = 7.363f;
static float TARGET_HEIGHT_OFFSET = 10.549f;

//ロックオン可能なカメラ座標との距離の上限（３D）
static float CAN_ROCK_ON_DIST_3D = 80.0f;
//ロックオン可能な画面中央との距離の上限（２D）
static float CAN_ROCK_ON_DIST_2D = 400.0f;
//ロックオン許容角度
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

	//プレイヤーとカメラの距離
	auto result = Player.GetPos() + angleVec * m_dist;
	result.y += m_height;

	float rate = (m_dist - DISTANCE_MIN) / (DISTANCE_MAX - DISTANCE_MIN);
	float lerpRate = 0.0f;
	lerpRate = KuroMath::Lerp(LERP_RATE_MAX, LERP_RATE_MIN, rate);
	result = KuroMath::Lerp(m_cam->GetPos(), result, lerpRate);

	m_cam->SetPos(result);

	//プレイヤーをロックオン
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
	//ロックオン対象が非アクティブになったらロックオン解除して終了
	if (m_rockOnPoint.expired() || !m_rockOnPoint.lock()->IsActive())
	{
		SetRockOnTarget(nullptr);
		return;
	}

	//ロックオン対象座標
	const auto rockPos = m_rockOnPoint.lock()->GetPosOn3D();
	//自身（カメラ）の座標
	const auto camPos = m_cam->GetPos();

	//ロックオン対象に合わせてカメラを動かす
	Angle toAngle = KuroMath::GetAngle({ rockPos.x,rockPos.z }, { PlayerPos.x,PlayerPos.z });

	//0 ~ 360の範囲に収める
	toAngle.Normalize();
	m_posAngle.Normalize();

	//目標角度までの遠回り防止
	if (toAngle < m_posAngle && Angle::PI() < m_posAngle - toAngle)
	{
		m_posAngle -= Angle::ROUND();
	}
	else if (m_posAngle < toAngle && Angle::PI() < toAngle - m_posAngle)
	{
		m_posAngle += Angle::ROUND();
	}

	//許容角度より目標値との角度の差が大きければ
	if (ROCK_ON_ANGLE_RANGE < Angle(abs(m_posAngle - toAngle)))
	{
		//目標値に近づく
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

	//ロックオン先までの線分のコライダー
	m_rockOnColLine = std::make_shared<CollisionLine>(Vec3<float>(0, 0, 0), Vec3<float>(0, 1, 0), 1.0f, nullptr);
	m_rockOnCollider = Collider::Generate(
		"Player Camera's Rock On Ray Collider",
		m_rockOnColLine,this, 
		COLLIDER_ATTRIBUTE::COLLIDER_NONE, COLLIDER_ATTRIBUTE::ENEMY | COLLIDER_ATTRIBUTE::TERRIAN);
}

void PlayerCamera::Init(const Transform& Player)
{
	m_posAngle = Angle(-90);	//プレイヤー後方
	m_height = HEIGHT_DEFAULT;
	m_dist = DISTANCE_DEFAULT;

	CalculatePos(Player);

	//何もロックオンしていない
	SetRockOnTarget(nullptr);
}

void PlayerCamera::Update(const Transform& Player, Vec2<float> InputVec)
{
	//角度の変化量
	const Angle angleAmount = Angle(2 * (m_mirrorX ? 1 : -1));

	//上下入力の変化量
	const float verticalAmount = 0.2f * (m_mirrorY ? 1 : -1);

	//右スティック入力
	Vec2<float> inputVec = InputVec;

	//ロックオン対象
	auto rockOnPoint = m_rockOnPoint.lock();

	//ロックオン中
	if (rockOnPoint)
	{
		RockOnTargeting(Player.GetPos());
	}
	//右スティック入力あり
	else if (!inputVec.IsZero())
	{
		//左右首振り
		m_posAngle += angleAmount * inputVec.x;

		//右スティック上下
		//高さが下限にあるとき上下スティック入力が距離入力になる
		if (m_dist < DISTANCE_MAX)
		{
			m_dist += verticalAmount * inputVec.y;
		}
		//高さの入力になる(縦首振り)
		else
		{
			m_height += verticalAmount * inputVec.y;
		}

		//超過した分の高さ入力を距離入力に変換
		if (m_height < HEIGHT_MIN)
		{
			m_dist += m_height - HEIGHT_MIN;
		}
		//超過した分の距離入力を高さ入力に変換
		if (DISTANCE_MAX < m_dist)
		{
			m_height += m_dist - DISTANCE_MAX;
		}
	}

	//クランプ
	m_dist = std::clamp(m_dist, DISTANCE_MIN, DISTANCE_MAX);
	m_height = std::clamp(m_height, HEIGHT_MIN, HEIGHT_MAX);
	m_posAngle.Normalize();

	//カメラ位置計算
	CalculatePos(Player);

	//計算後、当たり判定用の線分も更新
	if (rockOnPoint)
	{
		auto camPos = m_cam->GetPos();
		//ロックオン当たり判定線分の始点はカメラ座標
		m_rockOnColLine->m_start = camPos;;
		//ロックオン先に向かう
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
	//稼働中でないもの、ロックオン対象でないものを除く
	std::vector<std::weak_ptr<ActPoint>>actPointArray;
	for (auto& p : ActPoint::GetActPointArray())
	{
		if (p.expired() || !p.lock()->IsActive() || !p.lock()->IsCanRockOn())continue;
		actPointArray.emplace_back(p);
	}

	//算出したロックオン対象のポインタ格納先
	std::shared_ptr<ActPoint>newRockOnPoint = nullptr;
	float nearestDist2D = 100000.0f;

	//２番目に適しているロックオン対象のポインタ格納先
	std::shared_ptr<ActPoint>secondRockOnPoint = nullptr;

	for (auto& p : actPointArray)
	{
		auto worldPos = p.lock()->GetPosOn3D();
		//カメラ座標と離れすぎている
		if (CAN_ROCK_ON_DIST_3D < m_cam->GetPos().Distance(worldPos))continue;

		auto screenPos = p.lock()->GetPosOn2D(m_cam->GetViewMat(), m_cam->GetProjectionMat(), WinApp::Instance()->GetExpandWinSize());
		float dist2D = WinApp::Instance()->GetExpandWinCenter().Distance(screenPos);
		//画面中央と離れすぎている
		if (CAN_ROCK_ON_DIST_2D < dist2D)continue;

		//既に格納されているものより画面中央に近いなら更新
		if (dist2D < nearestDist2D)
		{
			secondRockOnPoint = newRockOnPoint;
			newRockOnPoint = p.lock();
			nearestDist2D = dist2D;
		}
	}

	//適した相手がいない
	if (!newRockOnPoint)
	{
		//既にロックオン済なら解除して終わり
		if (m_rockOnPoint.lock())
		{
			SetRockOnTarget(nullptr);
			return;
		}
		//ロックオンしていないなら正面を向く
		else
		{
			LookAtPlayersFront(Player);
			return;
		}
	}

	//元々ロックオン対象がいないか、ロックオン対象がいた場合違う相手なら設定して終わり
	if (!m_rockOnPoint.lock() || m_rockOnPoint.lock() != newRockOnPoint)
	{
		SetRockOnTarget(newRockOnPoint);
		return;
	}

	//既にロックオンしている対象と同じ相手なら、２番目のロックオン対象を採用
	if (secondRockOnPoint)
	{
		SetRockOnTarget(secondRockOnPoint);
		return;
	}

	//２番目の対象がいないならロックオン解除
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
