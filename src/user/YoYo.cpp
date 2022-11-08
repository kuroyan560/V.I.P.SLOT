#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"
#include"Object.h"

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

	//コライダー登録
	arg_collisionMgr.lock()->Register(m_neutralCol);
	arg_collisionMgr.lock()->Register(m_throwCol);

//ステータスごとのパラメータ設定
	//N攻撃
	m_statusParams[NEUTRAL].m_animName = "";
	m_statusParams[NEUTRAL].m_finishInterval = 32;
	m_statusParams[NEUTRAL].m_interruptInterval = m_statusParams[NEUTRAL].m_finishInterval;

	//攻撃0
	m_statusParams[THROW_0].m_animName = "Attack_0";
	m_statusParams[THROW_0].m_finishInterval = m_modelObj->m_model->m_skelton->animations["Attack_0"].finishTime;
	m_statusParams[THROW_0].m_interruptInterval = 18;

	//攻撃1
	m_statusParams[THROW_1].m_animName = "Attack_1";
	m_statusParams[THROW_1].m_finishInterval = m_modelObj->m_model->m_skelton->animations["Attack_1"].finishTime;
	m_statusParams[THROW_1].m_interruptInterval = 20;

	//攻撃2
	m_statusParams[THROW_2].m_animName = "Attack_2";
	m_statusParams[THROW_2].m_finishInterval = m_modelObj->m_model->m_skelton->animations["Attack_2"].finishTime;
	m_statusParams[THROW_2].m_interruptInterval = 44;
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
}

void YoYo::Update(const TimeScale& arg_timeScale)
{
	//アニメーション更新
	m_modelObj->m_animator->Update(arg_timeScale.GetTimeScale());

	//手に持ってる状態なら何もしない
	if (m_status == HAND)return;

	//先行入力があった場合、中断可能なら即時移行
	if (m_previousInput && CanInterrupt())
	{
		Throw(m_previousVec);
		m_previousInput = false;
	}

	//手に戻った状態に遷移
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		m_neutralCol->SetActive(false);
		m_throwCol->SetActive(false);
		m_status = HAND;
	}
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
	ImGui::Indent(16.0f);
	ImGui::Text("Vec : { %f,%f }", m_previousVec.x, m_previousVec.y);
	ImGui::Indent();

	ImGui::Separator();

	for (int i = 0; i < (int)STATUS::STATUS_NUM; ++i)
	{
		STATUS status = (STATUS)i;
		if (ImGui::TreeNode(std::string(magic_enum::enum_name(status)).c_str()))
		{
			ImGui::DragInt("FinishInterval", &m_statusParams[i].m_finishInterval);
			ImGui::DragInt("InterruptInterval", &m_statusParams[i].m_interruptInterval);
			if (m_statusParams[i].m_finishInterval < m_statusParams[i].m_interruptInterval)
				m_statusParams[i].m_interruptInterval = m_statusParams[i].m_finishInterval;
			ImGui::TreePop();
		}
	}
}

void YoYo::Throw(Vec2<float>arg_vec)
{
	//N攻撃（パリィ狙うため条件なしで即時発動）
	if (arg_vec.IsZero())
	{
		m_status = NEUTRAL;
		//N攻撃コライダーアクティブ
		m_neutralCol->SetActive(true);
	}
	//攻撃中か
	else if (IsActive())
	{
		//中断可能でない
		if (!CanInterrupt())
		{	
			//先行入力として受付
			m_previousInput = true;
			m_previousVec = arg_vec;
			return;
		}
	}

	//投擲攻撃
	if (m_status == HAND || m_status <= THROW_1)
	{
		//投擲用コライダーアクティブ
		m_throwCol->SetActive(true);

		static const Vec2<float>LEFT_VEC = { -1.0f,0.0f };
		static const Vec2<float>RIGHT_VEC = { 1.0f,0.0f };

		//スティック方向から攻撃左右方向選択
		const auto lefAngle = KuroMath::GetAngleAbs(arg_vec, LEFT_VEC);
		const auto rightAngle = KuroMath::GetAngleAbs(arg_vec, RIGHT_VEC);
		if (lefAngle < rightAngle)
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

		m_modelObj->m_animator->Play(
			m_statusParams[(int)m_status].m_animName, false, false);
	}

	m_timer.Reset(m_statusParams[(int)m_status].m_finishInterval);
}
