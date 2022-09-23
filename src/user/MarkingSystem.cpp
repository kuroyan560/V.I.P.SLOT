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

	//カメラが向いている方向
	auto camForward = Cam.GetForward();

	//マーキング対象になりうるポイントを配列に記録
	m_canMarkingPointArray.clear();
	for (auto& p : ActPoint::GetActPointArray())
	{
		//稼働中でないもの、マーキング対象でないものを除く
		if (p.expired() || !p.lock()->IsActive() || !p.lock()->IsCanMarking())continue;

		//マーキング可能な近さでない
		auto dist = PlayerPos.Distance(p.lock()->GetPosOn3D());
		if (m_canMarkingDist < dist)continue;

		//カメラから対象に向けてのベクトル
		auto camToTargetVec = (p.lock()->GetPosOn3D() - Cam.GetPos()).GetNormal();
		//カメラが向いている方向に対象がいるか
		if (camForward.Dot(camToTargetVec) < 0)continue;

		m_canMarkingPointArray.emplace_back(p);
	}

	//最短距離記録用
	float nearestDist = std::numeric_limits<float>::max();

	for (int ptIdx = 0; ptIdx < m_canMarkingPointArray.size(); ++ptIdx)
	{
		std::shared_ptr<ActPoint>actPt = m_canMarkingPointArray[ptIdx].lock();

		//ポイントのスクリーン座標を算出
		auto screenPos = actPt->GetPosOn2D(Cam.GetViewMat(), Cam.GetProjectionMat(), WinSize);

		Vec2<float>addtionalRange = WinSize * m_canMarkingAdditinalRangeRate;
		//マーキング出来る範囲にいるか
		if (screenPos.x < -addtionalRange.x)continue;
		else if (WinSize.x + addtionalRange.x < screenPos.x)continue;
		if (screenPos.y < -addtionalRange.y)continue;
		else if (WinSize.y + addtionalRange.y < screenPos.y)continue;

		//画面中央からの距離
		auto dist = screenPos.Distance(winCenter);

		//最短距離ならマーキング先として記録
		if (dist < nearestDist)
		{
			m_nowTargetIdx = ptIdx;
			nearestDist = dist;
		}
	}

	//有効なマーキング先がないなら処理終わり
	if (m_nowTargetIdx == -1)return;

	auto nowMarkingTarget = m_canMarkingPointArray[m_nowTargetIdx];

	//マーキング発動
	if (MarkingInput	//入力
		&& m_sumMarkingCount < MaxMarkingCount	//マーキング発動回数制限
		&& !nowMarkingTarget.lock()->IsMaxMarking())		//被マーキング回数制限
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

	//マーキング済でポインタ配列にないなら追加
	if (nowMarkingTarget.lock()->GetMarkingCount())
	{
		int nowTargetID = nowMarkingTarget.lock()->GetID();
		auto result = std::find_if(m_markingPointArray.begin(), m_markingPointArray.end(), [nowTargetID](std::weak_ptr<ActPoint> p)
			{
				return p.lock()->GetID() == nowTargetID;
			});
		if (result == m_markingPointArray.end())
		{
			//予測線の生成
			if (!m_markingPointArray.empty())
			{
				//１つ前の対象
				auto before = m_markingPointArray.back();
				//ラッシュの予測線（配列逆順）
				m_predictionLines.emplace_back(before.lock()->GetPosOn3D(), nowMarkingTarget.lock()->GetPosOn3D(), m_predictionLinePosSpan);
			}
			m_markingPointArray.emplace_back(nowMarkingTarget);
		}
	}
}

#include"DrawFunc3D.h"
void MarkingSystem::DrawPredictionLine(Camera& Cam, Vec3<float>PlayerPos)
{
	//プレイヤーの座標オフセット
	PlayerPos.y += 6.0f;
	
	//プレイヤーから最後に追加されたターゲットに向けて線描画
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
	//マーキング先
	for (int ptIdx = 0; ptIdx < m_canMarkingPointArray.size(); ++ptIdx)
	{
		if (m_canMarkingPointArray[ptIdx].expired())continue;

		std::shared_ptr<ActPoint>actPt = m_canMarkingPointArray[ptIdx].lock();

		bool isNowTarget = (ptIdx == m_nowTargetIdx);

		//画像番号
		int texIdx = isNowTarget ? static_cast<int>(RETICLE::ON) : static_cast<int>(RETICLE::OFF);

		//画面外にあるときも画像が画面外にいかないようにする
		const float expRate = isNowTarget ? 1.0f : 0.7f;
		const Vec2<float> halfGraphSize = m_reticleTex[texIdx]->GetGraphSize().Float() * expRate / 2.0f;

		auto screenPos = actPt->GetPosOn2D(Cam.GetViewMat(), Cam.GetProjectionMat(), WinSize);
		if (screenPos.x - halfGraphSize.x < 0)screenPos.x = halfGraphSize.x;
		else if (WinSize.x < screenPos.x + halfGraphSize.x)screenPos.x = WinSize.x - halfGraphSize.x;
		if (screenPos.y - halfGraphSize.y < 0)screenPos.y = halfGraphSize.y;
		else if (WinSize.y < screenPos.y + halfGraphSize.y)screenPos.y = WinSize.y - halfGraphSize.y;

		DrawFunc2D::DrawRotaGraph2D(screenPos, { expRate,expRate }, 0.0f, m_reticleTex[texIdx]);

		//現在のマーキング対象のマーキング回数描画
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
	//距離計測
	float dist = From.Distance(To);
	//矢印を設置する数
	int arrowNum = dist / SpanDist;
	//向き取得
	Vec3<float> dir = (To - From).GetNormal();
	//回転角度間隔
	Angle angleSpan = Angle::ROUND() / arrowNum;

	//トランスフォーム生成
	for (int arrowIdx = 0; arrowIdx < arrowNum; ++arrowIdx)
	{
		auto pos = From + dir * arrowIdx * SpanDist;
		Transform newTransform;
		newTransform.SetPos(pos);
		newTransform.SetScale(3.0f);
		m_transformArray.push_back(newTransform);
	}
	m_directionPoint = To;

	//区間が狭かった場合は、中間地点に矢印設置
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
	//稼働中でない
	if (!m_isActive)return;

	//矢印を回転させる
	m_spinAngle += Angle(1);

	//回転後の上ベクトル算出
	Vec3<float>up = KuroMath::TransformVec3({ 0,1,0 }, KuroMath::RotateMat({ 0,0,1 }, m_spinAngle));

	for (auto& transform : m_transformArray)
	{
		//目標地点を向いた状態で回転
		transform.SetLookAtRotate(m_directionPoint, up);
		//描画
		DrawFunc3D::DrawNonShadingModel(Model, transform, Cam);
	}
}
