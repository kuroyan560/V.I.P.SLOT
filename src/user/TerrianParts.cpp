#include"TerrianParts.h"
#include"Collider.h"
#include"ActPoint.h"

TerrianPartsType::TerrianPartsType(
	std::shared_ptr<Model>Model,
	const std::vector<std::shared_ptr<Collider>>& CollidersOrigin,
	std::shared_ptr<TerrianPartsControl> ControlOrigin,
	std::vector<std::shared_ptr<ActPoint>>ActPointOrigin)
	: m_model(Model), m_collidersOrigin(CollidersOrigin), m_actPointOrigin(ActPointOrigin)
{
	//ただのクローン元なのでコライダーは非アクティブ
	for (auto& col : m_collidersOrigin)
	{
		col->SetActive(false);
	}

	//ActPointも同様
	for (auto& actPt : m_actPointOrigin)
	{
		actPt->SetIsActive(false);
	}

	if (ControlOrigin != nullptr)m_controlOrigin = ControlOrigin;
}

std::shared_ptr<TerrianParts>TerrianPartsType::GenerateParts(const Transform& InitTransform)
{
	//生成したパーツに自身（型オブジェクト）の情報を持たせる
	auto result = std::shared_ptr<TerrianParts>(new TerrianParts(this));

	//初期化トランスフォーム
	result->m_transform = InitTransform;

	//コライダーのクローン生成
	for (auto& colOrigin : m_collidersOrigin)
	{
		auto cloneCol = Collider::Clone(colOrigin, &result->m_transform);
		cloneCol->SetActive(true);	//クローンのコライダーをActive状態にする
		result->m_colliders.emplace_back(cloneCol);
	}

	//ActPointのクローン生成
	for (auto& actPt : m_actPointOrigin)
	{
		Transform actPointInitTransform = actPt->GetTransform();
		actPointInitTransform.SetParent(&result->m_transform);
		auto cloneActPt = ActPoint::Clone(actPt, actPointInitTransform);
		result->m_actPoints.emplace_back(cloneActPt);
	}

	//挙動のクローン生成
	if (!m_controlOrigin.expired())
	{
		result->m_control = std::shared_ptr<TerrianPartsControl>(m_controlOrigin.lock()->Clone());
	}

	return result;
}
