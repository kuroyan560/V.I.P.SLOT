#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"
#include"Object.h"
#include"GameObject.h"

void YoYo::StatusInit(Vec3<float>arg_accelVec)
{
	//ステータスのパラメータ取得
	auto& param = m_statusParams[(int)m_status];

	//アニメーション再生
	if (!param.m_animName.empty())
	{
		m_modelObj->m_animator->Play(
			param.m_animName, false, false);
	}

	//タイマーリセット
	m_timer.Reset(param.m_finishInterval);

	//加速度初期化
	m_iniAccel = param.m_maxAccelVal * arg_accelVec;
	m_accel = m_iniAccel;
}

void YoYo::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	arg_otherCollider.lock()->GetParentObject<GameObject>()->Damage(m_offensive);
}

YoYo::YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform)
{
	//モデルオブジェクト生成
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "yoyo.glb");

	//コライダー生成
	m_neutralCol = std::make_shared<Collider>();
	m_throwCol = std::make_shared<Collider>();

	/*
		[トランスフォーム親子関係]
		プレイヤー ← 攻撃方向指定 ← ヨーヨー ← 各ボーン
	*/
	m_vecTransform.SetParent(arg_playerTransform);
	m_modelObj->m_transform.SetParent(&m_vecTransform);
	m_modelObj->m_animator->SetParentTransform(m_modelObj->m_transform);

	/*
		・N攻撃の当たり判定親：ヨーヨー
		・投擲用の当たり判定親：各ボーン
	*/
	m_neutralCol->SetParentTransform(&m_modelObj->m_transform);
	m_throwCol->SetParentTransform(&m_modelObj->m_animator->GetBoneTransform("Bone"));

	//攻撃コールバック
	m_neutralCol->SetCallBack("Enemy", this);
	m_throwCol->SetCallBack("Enemy", this);

	//コライダー登録
	arg_collisionMgr.lock()->Register(m_neutralCol);
	arg_collisionMgr.lock()->Register(m_throwCol);

//ステータスごとのパラメータ設定
	//N攻撃
	m_statusParams[NEUTRAL].m_animName = "";
	m_statusParams[NEUTRAL].m_finishInterval = 13;
	m_statusParams[NEUTRAL].m_interruptInterval = 9;
	m_statusParams[NEUTRAL].m_interruptInterval = m_statusParams[NEUTRAL].m_finishInterval;

	//攻撃0
	m_statusParams[THROW_0].m_animName = "Attack_0";
	m_statusParams[THROW_0].m_finishInterval = 32;
	m_statusParams[THROW_0].m_interruptInterval = 18;
	m_statusParams[THROW_0].m_maxAccelVal = { 0.2f,0.0f,0.0f };

	//攻撃1
	m_statusParams[THROW_1].m_animName = "Attack_1";
	m_statusParams[THROW_1].m_finishInterval = 37;
	m_statusParams[THROW_1].m_interruptInterval = 20;
	m_statusParams[THROW_1].m_maxAccelVal = { 0.3f,0.0f,0.0f };

	//攻撃2
	m_statusParams[THROW_2].m_animName = "Attack_2";
	m_statusParams[THROW_2].m_finishInterval = 33;
	m_statusParams[THROW_2].m_interruptInterval = 33;
	m_statusParams[THROW_2].m_maxAccelVal = { 0.6f,0.22f,0.0f };
}


void YoYo::Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius)
{
	static const std::string TAG = "Player_Attack";
	std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;

	//N攻撃用コライダー生成
	m_neutralColSphere = std::make_shared<CollisionSphere>(arg_neutralColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_neutralColSphere };
	m_neutralCol->Generate("YoYo - Neutral", TAG, colPrimitiveArray);

	//投擲用コライダー生成
	m_throwColSphere = std::make_shared<CollisionSphere>(arg_throwColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_throwColSphere };
	m_throwCol->Generate("YoYo - Throw", TAG, colPrimitiveArray);
}

void YoYo::Init()
{
	//アニメーション初期化
	m_modelObj->m_animator->Reset();

	//コライダーオフ
	m_neutralCol->SetActive(false);
	m_throwCol->SetActive(false);

	//手に持ってる状態
	m_status = HAND;

	//先行入力フラグリセット
	m_previousInput = false;

	//加速度リセット
	m_accel = { 0,0,0 };
	m_iniAccel = { 0,0,0 };
}

void YoYo::Update(const TimeScale& arg_timeScale, float arg_playersVecX)
{
	//アニメーション更新
	m_modelObj->m_animator->Update(arg_timeScale.GetTimeScale());

	//手に持ってる状態なら何もしない
	if (m_status == HAND)return;

	//先行入力があった場合、中断可能なら即時移行
	if (m_previousInput && CanInterrupt())
	{
		Throw(arg_playersVecX);
		m_previousInput = false;
	}

	//手に戻った状態に遷移
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		m_neutralCol->SetActive(false);
		m_throwCol->SetActive(false);
		m_status = HAND;
	}

	//勢いの加速度更新
	m_accel = KuroMath::Ease(Out, Exp, m_timer.GetTimeRate(), m_iniAccel, { 0,0,0 });
}

#include"DrawFunc3D.h"
void YoYo::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock());
}

#include"imguiApp.h"
void YoYo::AddImguiDebugItem()
{
	//現在のステータス
	ImGui::Text("Status : { %s }", std::string(magic_enum::enum_name(m_status)).c_str());
	ImGui::Text("Rate : { %f }", m_timer.GetTimeRate());
	ImGui::Text("Elasped : { %f }", m_timer.GetElaspedTime());
	ImGui::Separator();

	//先行入力
	ImGui::Text("PreviousInput : { %s }", m_previousInput ? "true" : "false");

	ImGui::Separator();

	for (int i = 0; i < (int)STATUS::STATUS_NUM; ++i)
	{
		STATUS status = (STATUS)i;
		auto& param = m_statusParams[i];
		if (ImGui::TreeNode(std::string(magic_enum::enum_name(status)).c_str()))
		{
			ImGui::DragInt("FinishInterval", &param.m_finishInterval);
			ImGui::DragInt("InterruptInterval", &param.m_interruptInterval);
			if (param.m_finishInterval < param.m_interruptInterval)
				param.m_interruptInterval = param.m_finishInterval;

			ImGui::DragFloat("AccelX", &param.m_maxAccelVal.x);
			if (param.m_maxAccelVal.x < 0.0f)param.m_maxAccelVal.x = 0.0f;
			ImGui::DragFloat("AccelY", &param.m_maxAccelVal.y);
			if (param.m_maxAccelVal.y < 0.0f)param.m_maxAccelVal.y = 0.0f;

			ImGui::TreePop();
		}
	}
}

bool YoYo::Throw(float arg_vecX)
{
	//攻撃中か
	if (IsThrow())
	{
		//中断可能でない
		if (!CanInterrupt())
		{	
			//先行入力として受付
			m_previousInput = true;
			return false;
		}
	}

	if (!(m_status == HAND || m_status <= THROW_1))return false;

	//投擲用コライダーアクティブ
	m_throwCol->SetActive(true);

	if (arg_vecX <= 0.0f)
	{
		//左向き
		m_vecTransform.SetRotate(XMMatrixIdentity());
	}
	else
	{
		//右向き
		m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(0));
	}

	if (m_status == HAND)m_status = THROW_0;
	else if(m_status != THROW_2) m_status = (STATUS)(m_status + 1);

	StatusInit({ arg_vecX <= 0.0f ? -1.0f : 1.0f,1.0f,0.0f });

	return true;
}

bool YoYo::Neutral()
{
	m_status = NEUTRAL;
	//N攻撃コライダーアクティブ
	m_neutralCol->SetActive(true);

	//投擲中のコライダーを非アクティブに
	m_throwCol->SetActive(false);

	//ステータスのパラメータ取得
	auto& param = m_statusParams[(int)m_status];

	StatusInit({ 0.0f,0.0f,0.0f });

	return true;
}
