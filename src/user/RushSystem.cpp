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

	//プレイヤーの座標取得
	auto playerPos = PlayerTransform.GetPos();
	//配列最後尾のマーキング対象
	auto target = m_markingPointArray[m_rushTargetPointIdx].lock();
	//マーキング対象の座標取得
	auto targetPos = target->GetPosOn3D();
	//マーキング対象へのベクトル取得
	auto vecToTarget = (targetPos - playerPos).GetNormal();

	//強ラッシュのカメラワーク
	if (m_strongRushFlg)
	{
		//ラッシュ全体の現在の時間割合
		float timeRate = static_cast<float>(m_totalRushTimer) / m_totalRushTime;
		m_strongRushCamInfo.Update(timeRate);
	}
	if (m_totalRushTimer < m_totalRushTime) m_totalRushTimer++;

	//１ラッシュ最初のフレーム
	if (m_timer++ <= 0)
	{
		//通常ラッシュのカメラ（定点なので最初のフレームのみ処理）
		if (!m_strongRushFlg)
		{
			//カメラ座標計算
			Vec3<float>camPos = targetPos;
			auto camPosVec = vecToTarget;
			camPosVec.y = 0.0f;

			Angle vecSpinAngle = Angle(KuroFunc::GetRand(m_camToTargetVecSpinDegreeMax) * KuroFunc::GetRandPlusMinus());
			camPosVec = KuroMath::TransformVec3(camPosVec, Vec3<float>(0, 1, 0), vecSpinAngle);
			camPos += camPosVec * m_camDistToTarget;	//被写体と距離を取る
			camPos.y += m_camHeightOffset;					//高さを上げる
			s_cam->SetPos(camPos);
			//注視点設定
			auto camTargetPos = targetPos + Vec3<float>(0, m_camTargetHeightOffset, 0);
			s_cam->SetTarget(camTargetPos);
		}

		//プレイヤーの向き変更
		PlayerTransform.SetLookAtRotate(playerPos + Vec3<float>(vecToTarget.x, 0.0f, vecToTarget.z));
		//イージングの終了座標計算
		m_easingEndPos = targetPos - vecToTarget * m_distToTarget;
	}
	
	//イージングOut補間でプレイヤー移動
	Vec3<float>newPlayerPos = KuroMath::Ease(
		EASE_CHANGE_TYPE::Out,
		EASING_TYPE::Exp,
		static_cast<float>(m_timer),
		static_cast<float>(m_oneRushTime),
		m_easingStartPos,
		m_easingEndPos);
	//プレイヤーの座標適用
	PlayerTransform.SetPos(newPlayerPos);

	//ラッシュによる吹き飛ばし
	float oneRushTimeRate = static_cast<float>(m_timer) / m_oneRushTime;
	if (!m_blow && m_blowTiming < oneRushTimeRate)
	{
		//ActPointにふっとばし通知
		m_markingPointArray[m_rushTargetPointIdx].lock()->OnRushBlowTrigger();
		//フラグオン
		m_blow = true;
	}

	m_blowRate = (oneRushTimeRate - m_blowTiming) / (1.0f - m_blowTiming);

	//１ラッシュ終了
	if (m_oneRushTime <= m_timer)
	{
		//イージングスタート値変更
		m_easingStartPos = m_easingEndPos;
		//ラッシュ対象変更
		m_rushTargetPointIdx--;
		
		//まだ対象が２つ以上あるとき
		if (0 < m_rushTargetPointIdx)
		{
			m_involvedObjsTargetPos = m_markingPointArray[m_rushTargetPointIdx - 1].lock()->GetPosOn3D();
		}
		//最後のラッシュ
		else if (m_rushTargetPointIdx == 0)
		{
			//各ActPointに通知
			for (auto& pt : m_markingPointArray)pt.lock()->OnLastRushTrigger();
		}
		//対象の全ActPointへのラッシュが完了した
		else
		{
			//配列クリア
			m_markingPointArray.clear();
		}
		//タイマーリセット
		m_timer = 0;
		//ふっとばしフラグリセット
		m_blow = false;
		//ふっ飛ばしの勢いリセット
		m_blowRate = 0.0f;
	}
}

void RushSystem::Excute(MarkingSystem& MarkingSys, const Vec3<float>& PlayerPos, bool& IsOneShot, const float& OneShotChargeRate)
{
	//単発ラッシュ
	if (IsOneShot)
	{
		//現在条件に合う対象が存在しないならスルー
		if (MarkingSys.m_nowTargetIdx == -1)return;

		//単発ラッシュの対象のみ格納
		m_markingPointArray = { MarkingSys.m_canMarkingPointArray[MarkingSys.m_nowTargetIdx] };
	}
	else
	{
		//マーキングした対象の配列をコピー
		m_markingPointArray = MarkingSys.m_markingPointArray;
	}

	//マーキング対象の数
	int targetNum = static_cast<int>(m_markingPointArray.size());

	//１体なら単発ラッシュフラグをONに変更
	if (targetNum == 1)IsOneShot = true;

	//マーキング対象の数の変位基準
	int markObjNumIndicator = targetNum;

	//下限、上限を超えないようにする
	if (markObjNumIndicator < m_markCountIndicatorMin)markObjNumIndicator = m_markCountIndicatorMin;
	if (m_markCountIndicatorMax < markObjNumIndicator)markObjNumIndicator = m_markCountIndicatorMax;

	//マーキング対象数の比率
	float markingCountRate = KuroMath::Lerp(0.0f, 1.0f,
		static_cast<float>((markObjNumIndicator - m_markCountIndicatorMin)) 
		/ static_cast<float>((m_markCountIndicatorMax - m_markCountIndicatorMin)));

	//比率が一定以上かつ単発ラッシュじゃないとき強ラッシュ演出カメラフラグオン
	m_strongRushFlg = (m_strongRushThreshold <= markingCountRate) && !IsOneShot;
	m_strongRushCamInfo.Init(m_markingPointArray, PlayerPos);

	//１ラッシュにかかる時間を計測（マーキング対象が多いと速度上昇）
	m_oneRushTime = static_cast<int>(KuroMath::Lerp(
		static_cast<float>(m_oneRushTimeMin), 
		static_cast<float>(m_oneRushTimeMax), 
		markingCountRate));
	//ラッシュ全体にかかる時間
	m_totalRushTime = m_oneRushTime * targetNum;

	//１ラッシュの火力を計測（マーキング対象が多いと火力上昇）
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

	//タイマーのリセット
	m_timer = 0;
	m_totalRushTimer = 0;

	//ラッシュ開始時のプレイヤーの座標を保存しておく
	m_easingStartPos = PlayerPos;

	//最後尾が最初のラッシュ対象
	m_rushTargetPointIdx = static_cast<int>(m_markingPointArray.size()) - 1;

	//２体以上のラッシュ対象がいるとき
	if (0 < m_rushTargetPointIdx)
	{
		m_involvedObjsTargetPos = m_markingPointArray[m_rushTargetPointIdx - 1].lock()->GetPosOn3D();
	}

	//ふっ飛ばしフラグリセット
	m_blow = false;

	//ふっ飛ばしの勢いリセット
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
		if (m_oneRushTimeMin < 0)m_oneRushTimeMin = 0;	//マイナス防止
		if (m_oneRushTimeMax < m_oneRushTimeMin)m_oneRushTimeMin = m_oneRushTimeMax;	//上限を超えない
	}
	if (ImGui::DragInt("OneRushTime_Max", &m_oneRushTimeMax))
	{
		if (m_oneRushTimeMax < m_oneRushTimeMin)m_oneRushTimeMax = m_oneRushTimeMin;	//下限を超えない
	}

	//ラッシュのふっ飛ばし力
	if (ImGui::DragFloat("BlowPower", &m_blowPowerMax, 0.05f) && m_blowPowerMax < 0.0f)	//マイナス防止
	{
		m_blowPowerMax = 0.0f;
	}

	//ラッシュで吹き飛ばすタイミング
	ImGui::DragFloat("BlowTiming", &m_blowTiming, 0.05f, 0.0f, 1.0f);

	//ふっ飛ばされたときの座標の高さオフセット最大
	ImGui::DragFloat("BlowPowerY", &m_blowPowerY, 0.05f);

	//ふっ飛ばされたときの重力の度合い
	ImGui::DragFloat("BlowGravityRate", &m_blowGravityRate, 0.05f);


	if (ImGui::DragInt("MarkCountIndicator_Min", &m_markCountIndicatorMin))
	{
		if (m_markCountIndicatorMin < 0)m_markCountIndicatorMin = 0;	//マイナス防止
		if (m_markCountIndicatorMax < m_markCountIndicatorMin)m_markCountIndicatorMin = m_markCountIndicatorMax;	//上限を超えない
	}
	if (ImGui::DragInt("MarkCountIndicator_Max", &m_markCountIndicatorMax))
	{
		if (m_markCountIndicatorMax < m_markCountIndicatorMin)m_markCountIndicatorMax = m_markCountIndicatorMin;	//下限を超えない
	}

	if (ImGui::DragFloat("DistToTarget", &m_distToTarget) && m_distToTarget < 0)	//マイナス防止
	{
		m_distToTarget = 0;
	}

	//ラッシュカメラ演出関連
	ImGui::Separator();
	ImGui::Text("- NormalRushCamera -");
	if (ImGui::DragFloat("CamDistToTarget", &m_camDistToTarget) && m_camDistToTarget < 0)	//マイナス防止
	{
		m_camDistToTarget = 0;
	}
	if (ImGui::DragFloat("CamHeightOffset", &m_camHeightOffset) && m_camHeightOffset < 0)	//マイナス防止
	{
		m_camHeightOffset = 0;
	}
	if (ImGui::DragFloat("CamTargetHeightOffset", &m_camTargetHeightOffset) && m_camTargetHeightOffset < 0)	//マイナス防止
	{
		m_camTargetHeightOffset = 0;
	}
	if (ImGui::DragInt("CamToTargetVecSpinDegree", &m_camToTargetVecSpinDegreeMax))
	{
		if (m_camToTargetVecSpinDegreeMax < 0)m_camToTargetVecSpinDegreeMax = 0;	//マイナス防止
		if (360 < m_camToTargetVecSpinDegreeMax)m_camToTargetVecSpinDegreeMax = 360;	//一周超過防止
	}

	//強ラッシュカメラ演出関連
	ImGui::Separator();
	ImGui::Text("- StrongRushCamera -");
	ImGui::Text("StrongRush : { %s }", m_strongRushFlg ? "true" : "false");
	ImGui::DragFloat("StrongRush_Threshold", &m_strongRushThreshold, 0.05f, 0.0f, 1.0f);
	ImGui::DragFloat("DistMotionRate", &m_strongRushCamInfo.m_distMotionRate, 0.05f, 0.0f, 1.0f);

}

float RushSystem::GetNowBlowPower() const
{
	//最後のラッシュのノックバックでのみ使用する
	assert(IsLastRush());
	return KuroMath::Ease(Out, Circ, m_blowRate, m_blowPowerMax, 0.0f);
}

void RushSystem::StrongRushCamInfo::Init(const std::vector<std::weak_ptr<ActPoint>>& TargetArray, Vec3<float>PlayerPos)
{
	m_ySpinAngle = -m_ySpinAngleOffset;
	m_distToCenter = 0.0f;

	m_spaceMin = TargetArray[0].lock()->GetPosOn3D();
	m_spaceMax = TargetArray[0].lock()->GetPosOn3D();

	//対象の座標の配列
	std::vector<Vec3<float>>targetPositions;
	for (auto& target : TargetArray)
	{
		auto pos = target.lock()->GetPosOn3D();
		targetPositions.emplace_back(pos);

		//最小値の保存
		if (pos.x < m_spaceMin.x)m_spaceMin.x = pos.x;
		if (pos.y < m_spaceMin.y)m_spaceMin.y = pos.y;
		if (pos.z < m_spaceMin.z)m_spaceMin.z = pos.z;

		//最大値の保存
		if (m_spaceMax.x < pos.x)m_spaceMax.x = pos.x;
		if (m_spaceMax.y < pos.y)m_spaceMax.y = pos.y;
		if (m_spaceMax.z < pos.z)m_spaceMax.z = pos.z;
	}

	//規定のオフセットの限界を超えない
	if (-m_distToCenterOffsetMin.x < m_spaceMin.x)m_spaceMin.x = -m_distToCenterOffsetMin.x;
	if (-m_distToCenterOffsetMin.y < m_spaceMin.y)m_spaceMin.y = -m_distToCenterOffsetMin.y;
	if (-m_distToCenterOffsetMin.z < m_spaceMin.z)m_spaceMin.z = -m_distToCenterOffsetMin.z;
	if (m_spaceMax.x < m_distToCenterOffsetMin.x)m_spaceMax.x = m_distToCenterOffsetMin.x;
	if (m_spaceMax.y < m_distToCenterOffsetMin.y)m_spaceMax.y = m_distToCenterOffsetMin.y;
	if (m_spaceMax.z < m_distToCenterOffsetMin.z)m_spaceMax.z = m_distToCenterOffsetMin.z;

	//追加分
	m_spaceMin -= {m_distToCenterAdditional, m_distToCenterAdditional, m_distToCenterAdditional};
	m_spaceMax += {m_distToCenterAdditional, m_distToCenterAdditional, m_distToCenterAdditional};

	//カメラ挙動の中心を計算
	m_center = (m_spaceMax - m_spaceMin) / 2.0f + m_spaceMin;

	//カメラ挙動の基準となるベクトル
	m_criteriaVec = (m_center - PlayerPos).GetNormal();
	m_criteriaVec.y = 0.0f;
}

void RushSystem::StrongRushCamInfo::Update(const float& Rate)
{
	//距離の変化率
	float distChangeRate = Rate;
	if (m_distMotionRate < distChangeRate)distChangeRate = m_distMotionRate;

	//距離
	m_distToCenter = KuroMath::Ease(
		m_distEaseChangeType,
		m_distEaseType,
		distChangeRate,
		m_distMotionRate,
		0.0f,
		m_spaceMax.Distance(m_center));

	//座標のY軸回転
	m_ySpinAngle = KuroMath::Ease(
		m_spinEaseChangeType,
		m_spinEaseType,
		Rate,
		-m_ySpinAngleOffset,
		m_ySpinAngleOffset);

	//カメラの座標を計算して求める
	Vec3<float>camPos = m_center + KuroMath::TransformVec3(m_criteriaVec, { 0,1,0 }, m_ySpinAngle) * m_distToCenter;
	camPos.y += m_heightOffset;
	s_cam->SetPos(camPos);
	s_cam->SetTarget(m_center);
}
