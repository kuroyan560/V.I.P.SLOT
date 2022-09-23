#include"ColliderWithFloor.h"

ColliderWithFloor::ColliderWithFloor(const std::string& ParentName, const Vec3<float>& Offset, Transform* ParentTransform)
	:m_callBack(this), m_targetTransform(ParentTransform)
{
	//床用足元
	auto footCol_Point = std::make_shared<CollisionPoint>(Offset, ParentTransform);
	m_collider = Collider::Generate(
		ParentName + "'s FootPoint Collider",
		footCol_Point,	//点コライダー
		&m_callBack,				//床との押し戻し処理コールバック
		COLLIDER_ATTRIBUTE::FOOT_POINT,
		COLLIDER_ATTRIBUTE::FLOOR);
}

void ColliderWithFloor::CallBack::OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)
{
	//押し戻し処理を行う対象のTransform
	auto targetTransform = m_parent->m_targetTransform;

	//現在の座標
	Vec3<float>nowPos = targetTransform->GetPos();

	//床の高さ取得
	float floorY = Inter.y;

	//自身のコライダープリミティブ（点）を取得
	auto myPt = (CollisionPoint*)GetAttachCollider().lock()->GetColliderPrimitive().lock().get();

	//押し戻し後の座標格納先
	Vec3<float>pushBackPos = nowPos;
	pushBackPos.y += floorY - myPt->GetWorldPos().y;	//押し戻し

	//新しい座標をセット
	targetTransform->SetPos(pushBackPos);

	//接地フラグON
	m_parent->m_onGround = true;

	//落下速度リセット
	m_parent->m_fallSpeed = 0.0f;
}