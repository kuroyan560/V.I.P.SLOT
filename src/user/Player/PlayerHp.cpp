#include "PlayerHp.h"
#include"ConstParameters.h"
#include"D3D12App.h"
#include"DrawFunc2D.h"
#include"Sprite.h"

Vec2<float> PlayerHp::CalculateHpBarSize(float arg_rate)
{
	auto hpBarSize = m_hpBar.m_sprite->GetTex()->GetGraphSize().Float();
	hpBarSize.x = arg_rate * hpBarSize.x;
	return hpBarSize;
}

PlayerHp::PlayerHp()
{
	//初期化位置設定
	m_uiInitPos = { 20.0f,640.0f };
	m_hpStr.m_initPos = { 0.0f,-48.0f };
	m_hpBar.m_initPos = { 13.0f,13.0f };
	m_hpBarDamage.m_initPos = { 13.0f,13.0f };
	m_hpBarFrame.m_initPos = { 0.0f,0.0f };
	Vec2<float>heartSpriteInitPos = { 6.0f,4.0f };
	Vec2<float>heartSpriteFrameInitPos = { 0.0f,0.0f };

	//ライフ関連
	//メインライフ（一番右、次に消費される）の位置
	Vec2<float>	mainHeartInitPos = { 317.0f,-50.0f };
	//メインライフのスケール
	float mainHeartScale = 1.0f;
	//その他のライフの位置オフセット（メインハート基準）
	Vec2<float>subHeartPosOffsetWithMain = { -39.0f,13.0f };
	//サブ同士のX座標オフセット
	float subHeartXOffset = -39.0f;
	//サブライフのスケール
	float subHeartScale = 0.8f;

	//DirectX12機構取得
	auto app = D3D12App::Instance();
	//画像ファイルのディレクトリ
	std::string dir = "resource/user/img/ui/hp/";

	//スプライト生成、画像読み込み
	m_hpStr.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_str.png"), "Sprite - HpStr");
	m_hpBar.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar.png"), "Sprite - HpBar");
	m_hpBarDamage.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_damage.png"), "Sprite - HpBarDamage");
	m_hpBarFrame.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_frame.png"), "Sprite - HpBarFrame");
	auto heartTex = app->GenerateTextureBuffer(dir + "hp_heart.png");
	auto heartFrameTex = app->GenerateTextureBuffer(dir + "hp_heart_frame.png");

	//ハートスプライト生成（最大ライフ分用意）
	m_heartArray.resize(ConstParameter::Player::MAX_LIFE);
	for (int i = 0; i < ConstParameter::Player::MAX_LIFE; ++i)
	{
		//スプライト生成
		m_heartArray[i].m_heart.m_sprite = std::make_shared<Sprite>(heartTex, ("Sprite - Heart - " + std::to_string(i)).c_str());
		m_heartArray[i].m_frame.m_sprite = std::make_shared<Sprite>(heartFrameTex, ("Sprite - HeartFrame - " + std::to_string(i)).c_str());
		//スプライト初期位置設定
		m_heartArray[i].m_heart.m_initPos = heartSpriteInitPos;
		m_heartArray[i].m_frame.m_initPos = heartSpriteFrameInitPos;

		//ハート位置設定
		if (i == 0)
		{
			//メインライフの設定をして終了
			m_heartArray[i].m_initPos = mainHeartInitPos;
			m_heartArray[i].m_initScale = mainHeartScale;
			continue;
		}

		//サブライフのオフセット適用
		m_heartArray[i].m_initPos = mainHeartInitPos + subHeartPosOffsetWithMain;
		//サブライフ間のXオフセット適用
		m_heartArray[i].m_initPos.x += (i - 1) * subHeartXOffset;
		//サブライフのスケール適用
		m_heartArray[i].m_initScale = subHeartScale;
	}

	//トランスフォームの親子関係構築
	m_hpStr.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBar.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBarDamage.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBarFrame.m_sprite->m_transform.SetParent(&m_transform);
	for (auto& heart : m_heartArray)
	{
		heart.m_offsetTransform.SetParent(&m_transform);
		heart.m_frame.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
		heart.m_heart.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
	}
}

void PlayerHp::Init(int arg_initMaxLife, int arg_initRemainLife)
{
	using namespace ConstParameter::Player;

	//ライフ初期化
	m_life = arg_initRemainLife;

	//HP初期化
	m_hp = MAX_HP;

	//HPバーサイズ更新
	m_hpBar.m_sprite->m_mesh.SetSize(CalculateHpBarSize(1.0f));

	//UI位置初期化
	m_transform.SetPos(m_uiInitPos);

	//描画するスプライトを決定（追加順 = 描画順）
	//HPバーと文字は必須
	m_contents.clear();
	m_contents.emplace_back(&m_hpBarFrame);
	m_contents.emplace_back(&m_hpBarDamage);
	m_contents.emplace_back(&m_hpBar);
	m_contents.emplace_back(&m_hpStr);

	//ライフ関連↓（ハートの個数によって変動）
	
	//最大ライフを超えている
	if (static_cast<int>(m_heartArray.size()) < arg_initMaxLife)assert(0);

	//必要な数だけスプライト追加
	for (int i = arg_initMaxLife - 1; 0 <= i; --i)	//メインライフを手前側にするため降順
	{
		//ハート枠スプライト
		m_contents.emplace_back(&m_heartArray[i].m_frame);
		//ハートスプライト
		m_contents.emplace_back(&m_heartArray[i].m_heart);
		//位置初期化
		m_heartArray[i].m_offsetTransform.SetPos(m_heartArray[i].m_initPos);
		//スケール初期化
		m_heartArray[i].m_offsetTransform.SetScale({ m_heartArray[i].m_initScale,m_heartArray[i].m_initScale });

		//残ライフならハート描画をアクティブ状態に
		m_heartArray[i].m_heart.m_active = (i < arg_initRemainLife);
	}

	//スプライトの位置初期化
	for (auto& content : m_contents)
	{
		content->m_sprite->m_transform.SetPos(content->m_initPos);
	}

	//ダメージ演出初期化
	m_damageEffect.Init();
}

void PlayerHp::Update(const float& arg_timeScale)
{
	m_damageEffect.Update(arg_timeScale);

	if (m_damageEffect.m_active)
	{
		if (m_damageEffect.m_consumeLife)
		{

		}
		else
		{
			float sizeRate = KuroMath::Ease(In, Sine,
				m_damageEffect.m_drawChangeTimer.GetTimeRate(),
				m_damageEffect.m_startHpRate,
				m_damageEffect.m_endHpRate);
			m_hpBarDamage.m_sprite->m_mesh.SetSize(CalculateHpBarSize(sizeRate));
		}

		for (auto& content : m_contents)
		{
			content->m_sprite->m_transform.SetPos(content->m_initPos + Vec2<float>(m_damageEffect.m_shake.GetOffset().x, m_damageEffect.m_shake.GetOffset().y));
		}
	}
}


void PlayerHp::OnDraw2D()
{
	for (auto& content : m_contents)
	{
		if (!content->m_active)continue;
		content->m_sprite->Draw();
	}
}

#include"ImguiApp.h"
void PlayerHp::ImguiDebug()
{
	ImGui::Begin("PlayerHp");

	auto parentPos = m_transform.GetPos();
	if (ImGui::DragFloat2("PosUI", (float*)&parentPos))m_transform.SetPos(parentPos);

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 300), ImGuiWindowFlags_NoTitleBar);
	for (auto& content : m_contents)
	{
		auto sp = content->m_sprite;
		if (ImGui::TreeNode(sp->m_name.c_str()))
		{
			auto pos = sp->m_transform.GetPos();
			if (ImGui::DragFloat2("Pos", (float*)&pos))sp->m_transform.SetPos(pos);

			ImGui::TreePop();
		}
	}

	for (int i = 0; i < static_cast<int>(m_heartArray.size()); ++i)
	{
		if (ImGui::TreeNode(("Heart - " + std::to_string(i)).c_str()))
		{
			auto pos = m_heartArray[i].m_offsetTransform.GetPos();
			if (ImGui::DragFloat2("Pos", (float*)&pos))m_heartArray[i].m_offsetTransform.SetPos(pos);

			auto scale = m_heartArray[i].m_offsetTransform.GetScale().x;
			if (ImGui::DragFloat("Scale", &scale, 0.05f))m_heartArray[i].m_offsetTransform.SetScale({ scale,scale });

			ImGui::TreePop();
		}
	}
	ImGui::EndChild();


	ImGui::End();
}

void PlayerHp::Change(int arg_amount)
{
	//変化前のHPレート
	float beforeHpRate = static_cast<float>(m_hp) / static_cast<float>(ConstParameter::Player::MAX_HP);
	//ハートを消費したか
	bool consumeLife = false;

	//HPが上下限界を超えないようにする
	m_hp = std::max(0, std::min(ConstParameter::Player::MAX_HP, m_hp + arg_amount));

	//HPがなくなったらライフ消費してHP回復
	if (m_hp <= 0 && m_life)
	{
		//ハート描画を非アクティブに
		m_heartArray[m_life - 1].m_heart.m_active = false;
		m_life = std::max(0, m_life - 1);
		m_hp = ConstParameter::Player::MAX_HP;
		consumeLife = true;
	}

	//変化後のHPレート
	float afterHpRate = static_cast<float>(m_hp) / static_cast<float>(ConstParameter::Player::MAX_HP);
	//HPバーサイズ変更
	m_hpBar.m_sprite->m_mesh.SetSize(CalculateHpBarSize(afterHpRate));

	//ダメージ
	if (arg_amount < 0)
	{
		//ダメージ演出
		m_damageEffect.Start(beforeHpRate, afterHpRate, consumeLife);
	}
}