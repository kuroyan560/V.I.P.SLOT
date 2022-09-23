#include "ActPoint.h"
#include"DrawFuncBillBoard.h"
#include"DrawFunc3D.h"
#include"BasicDraw.h"
#include"Importer.h"

int ActPoint::s_id = 0;
std::vector<std::weak_ptr<ActPoint>>ActPoint::s_points;

ActPoint::ActPoint(const Transform& InitTransform,
	const bool& CanRockOn,
	const bool& CanMarking,
	const int& MaxMarkingCount,
	const float& RayOffsetWhenRockOn)
	:m_id(s_id++), m_canRockOn(CanRockOn), m_canMarking(CanMarking),
	m_markingCountMax(MaxMarkingCount), m_isActive(true), m_rayOffsetWhenRockOn(RayOffsetWhenRockOn)
{
	m_transform = InitTransform;

	//マーキング回数初期化
	InitMarkingCount();
}

std::shared_ptr<ActPoint> ActPoint::Generate(const Transform& InitTransform,
	const bool& CanRockOn, 
	const bool& CanMarking, 
	const int& MaxMarkingCount)
{
	auto instance = std::shared_ptr<ActPoint>(new ActPoint(InitTransform, CanRockOn, CanMarking, MaxMarkingCount));
	s_points.emplace_back(instance);
	return instance;
}

std::shared_ptr<ActPoint>ActPoint::Clone(const std::shared_ptr<ActPoint>& Origin, const Transform& InitTransform)
{
	return ActPoint::Generate(InitTransform, Origin->m_canRockOn, Origin->m_canMarking, Origin->m_markingCountMax);
}

void ActPoint::InitializeEachFrame()
{
	for (auto& p : s_points)
	{
		auto ptLock = p.lock();
		ptLock->m_onMarking = false;
		ptLock->m_onLock = false;
		ptLock->m_onRushBlow = false;
		ptLock->m_onLastRush = false;
	}
}

void ActPoint::Draw(Camera& Cam)
{
	static Vec2<float>s_size = { 1.0,1.0 };
	for (auto& p : s_points)
	{
		Color col;
		if (!p.lock()->m_isActive)
		{
			col = Color(0.4f, 0.4f, 0.4f, 1.0f);
		}
		else if (p.lock()->m_canRockOn)
		{
			if (p.lock()->m_canMarking)
			{
				col = Color(0.0f, 0.0f, 1.0f, 1.0f);
			}
			else
			{
				col = Color(1.0f, 0.0f, 0.0f, 1.0f);
			}
		}

		DrawFuncBillBoard::Box(Cam,p.lock()->GetPosOn3D(), s_size, col);
	}
}


