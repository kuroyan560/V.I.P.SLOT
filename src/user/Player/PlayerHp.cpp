#include "PlayerHp.h"
#include"ConstParameters.h"
#include"D3D12App.h"
#include"DrawFunc2D.h"
#include"Sprite.h"
#include"KuroEngine.h"
#include"ParticleMgr2D.h"

Vec2<float> PlayerHp::CalculateHpBarSize(Vec2<float>arg_texSize, float arg_rate)
{
	auto hpBarSize = arg_texSize;
	hpBarSize.x = arg_rate * hpBarSize.x;
	return hpBarSize;
}

PlayerHp::PlayerHp()
{
	//初期化位置設定
	m_uiInitPos = { 20.0f,640.0f };
	m_hpStr.m_initPos = { 0.0f,-48.0f };
	m_hpBar.m_initPos = { 13.0f,13.0f };
	m_damageEffect.m_hpBarDamage.m_initPos = m_hpBar.m_initPos;
	m_healEffect.m_hpBarHeal.m_initPos = m_hpBar.m_initPos;
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
	m_damageEffect.m_hpBarDamage.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_damage.png"), "Sprite - HpBarDamage");
	m_healEffect.m_hpBarHeal.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_heal.png"), "Sprite - HpBarHeal");
	m_hpBarFrame.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_frame.png"), "Sprite - HpBarFrame");

	//ダメージハート
	m_consumeLifeEffect.m_damageHeart.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_heart_damage.png"), "Sprite - DamageHeart");
	m_consumeLifeEffect.m_damageHeart.m_initPos = heartSpriteInitPos;

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
	m_damageEffect.m_hpBarDamage.m_sprite->m_transform.SetParent(&m_transform);
	m_healEffect.m_hpBarHeal.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBarFrame.m_sprite->m_transform.SetParent(&m_transform);
	for (auto& heart : m_heartArray)
	{
		heart.m_offsetTransform.SetParent(&m_transform);
		heart.m_frame.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
		heart.m_heart.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
	}
	m_consumeLifeEffect.m_damageHeart.m_sprite->m_transform.SetParent(&m_heartArray[0].m_offsetTransform);

}

void PlayerHp::Init(int arg_initMaxLife, int arg_initRemainLife)
{
	using namespace ConstParameter::Player;

	//ライフ初期化
	m_life = arg_initRemainLife;

	//HP初期化
	m_hp = MAX_HP;

	//HPバーサイズ更新
	m_hpBar.m_sprite->m_mesh.SetSize(CalculateHpBarSize(m_hpBar.m_sprite->GetTex()->GetGraphSize().Float(), 1.0f));

	//UI位置初期化
	m_transform.SetPos(m_uiInitPos);

	//描画するスプライトを決定（追加順 = 描画順）
	m_contents.clear();
	m_contents.emplace_back(&m_hpBarFrame);
	m_contents.emplace_back(&m_damageEffect.m_hpBarDamage);
	m_contents.emplace_back(&m_healEffect.m_hpBarHeal);
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

	//ダメージハート
	m_contents.emplace_back(&m_consumeLifeEffect.m_damageHeart);

	//スプライトの位置初期化
	for (auto& content : m_contents)
	{
		content->m_sprite->m_transform.SetPos(content->m_initPos);
	}

	//ダメージ演出初期化
	m_damageEffect.Init();

	//回復演出初期化
	m_healEffect.Init();

	//ライフ消費演出初期化
	m_consumeLifeEffect.Init();
}

void PlayerHp::Update(const float& arg_timeScale)
{
	//ダメージ演出
	m_damageEffect.Update(arg_timeScale);

	//回復演出
	m_healEffect.Update(arg_timeScale, m_transform.GetPos(), m_hpBar.m_sprite);

	m_consumeLifeEffect.Update(arg_timeScale, &m_hpBar);

	//UI振動
	for (auto& content : m_contents)
	{
		content->m_sprite->m_transform.SetPos(content->m_initPos + m_damageEffect.GetOffset());
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

bool PlayerHp::Change(int arg_amount)
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

	//ダメージ
	if (arg_amount < 0)
	{
		//回復演出中断
		m_healEffect.Interruput();

		//ライフ(ハート)消費
		if (consumeLife)
		{
			m_consumeLifeEffect.Start(&m_hpBar);
			m_hpBar.m_active = false;
		}
		else
		{
			//ダメージ演出
			m_damageEffect.Start(beforeHpRate, afterHpRate, m_hpBar.m_sprite);
		}
	}
	//回復
	else
	{
		//ダメージ演出中断
		m_damageEffect.Interruput();

		//回復演出
		m_healEffect.Start(beforeHpRate, afterHpRate);
	}

	return consumeLife;
}

void PlayerHp::ConsumeLifeEffect::Start(DrawContents* arg_hpBar)
{
	m_damageHeart.m_active = true;
	arg_hpBar->m_active = false;
	m_damageHeart.m_sprite->m_transform.SetPos(m_damageHeart.m_initPos);
}

void PlayerHp::ConsumeLifeEffect::Update(float arg_timeScale, DrawContents* arg_hpBar)
{
	if (!m_damageHeart.m_active)return;

	if (arg_timeScale)
	{
		m_damageHeart.m_active = false;
		arg_hpBar->m_active = true;
	}
}

void PlayerHp::DamageEffect::Start(float arg_startHpRate, float arg_endHpRate, std::weak_ptr<Sprite> arg_hpBarSprite)
{
	m_hpBarDamage.m_active = true;
	m_drawChangeTimer.Reset(30.0f);
	m_waitTimer.Reset(45.0f);
	m_shake.Shake(30.0f, 3.0f, 0.0f, 32.0f);
	m_startHpRate = arg_startHpRate;
	m_endHpRate = arg_endHpRate;

	//HPバーサイズ変更
	arg_hpBarSprite.lock()->m_mesh.SetSize(CalculateHpBarSize(arg_hpBarSprite.lock()->GetTex()->GetGraphSize().Float(), arg_endHpRate));
}

void PlayerHp::DamageEffect::Update(float arg_timeScale)
{
	if (!m_hpBarDamage.m_active)return;

	if (m_drawChangeTimer.IsTimeUp())
	{
		m_hpBarDamage.m_active = false;
		return;
	}

	if (m_waitTimer.UpdateTimer(arg_timeScale))
	{
		m_drawChangeTimer.UpdateTimer(arg_timeScale);
	}

	//※タイムスケールの影響を受けない
	m_shake.Update(1.0f);

	//ダメージHPバーが小さくなっていく
	float sizeRate = KuroMath::Ease(In, Sine,
		m_drawChangeTimer.GetTimeRate(),
		m_startHpRate,
		m_endHpRate);
	m_hpBarDamage.m_sprite->m_mesh.SetSize(CalculateHpBarSize(m_hpBarDamage.m_sprite->GetTex()->GetGraphSize().Float(), sizeRate));
}


void PlayerHp::HealEffect::EmitParticle(Vec2<float> arg_basePos, float arg_randMaxPosX)
{
	auto defIni = ParticleMgr2D::Instance()->GetDefaultInitializer(m_particleID);
	//基準の座標
	defIni.m_pos = arg_basePos;
	//乱数で位置をずらす
	defIni.m_pos.x += KuroFunc::GetRand(arg_randMaxPosX);

	static const float OFFSET_Y_MAX = 32.0f;
	defIni.m_pos.y += KuroFunc::GetRand(-OFFSET_Y_MAX, OFFSET_Y_MAX);

	static const float SCALE_MIN = 16.0f;
	static const float SCALE_MAX = 48.0f;
	//乱数でサイズ変動
	defIni.m_scale = KuroFunc::GetRand(SCALE_MIN, SCALE_MAX);

	ParticleMgr2D::Instance()->Emit(m_particleID, defIni);
}

PlayerHp::HealEffect::HealEffect()
{
	std::vector<std::shared_ptr<TextureBuffer>>particleTextures(3);
	D3D12App::Instance()->GenerateTextureBuffer(
		particleTextures.data(),
		"resource/user/img/ui/hp/hp_heal_effect.png",
		3,
		{ 3,1 });

	ParticleInitializer ini;
	ini.m_lifeSpan = 20.0f;

	m_particleID = ParticleMgr2D::Instance()->Prepare(
		50,
		"resource/user/shaders/Particle/PlayerHpHeal.hlsl",
		particleTextures.data(),
		particleTextures.size(),
		&ini,
		AlphaBlendMode_Add
		);
}

void PlayerHp::HealEffect::Start(float arg_startHpRate, float arg_endHpRate)
{
	m_endHpRate = arg_endHpRate;
	//回復HPバーサイズ変更
	m_hpBarHeal.m_sprite->m_mesh.SetSize(CalculateHpBarSize(
		m_hpBarHeal.m_sprite->GetTex()->GetGraphSize().Float(), m_endHpRate));
	

	//既にアクティブ状態かつ待機状態（延長して終了）
	if (m_hpBarHeal.m_active && !m_waitTimer.IsTimeUp())
	{
		//待機時間延長
		m_waitTimer.Reset(45.0f);
		return;
	}

	m_hpBarHeal.m_active = true;
	m_drawChangeTimer.Reset(30.0f);
	m_startHpRate = arg_startHpRate;
	m_flashRadian = 0.0f;
	m_ptEmitTimer.Reset(10.0f);
	m_waitTimer.Reset(45.0f);
}

void PlayerHp::HealEffect::Update(float arg_timeScale, Vec2<float>arg_uiPos, std::weak_ptr<Sprite>arg_hpBarSprite)
{
	if (!m_hpBarHeal.m_active)return;

	if (m_drawChangeTimer.IsTimeUp())
	{
		m_hpBarHeal.m_active = false;
		return;
	}

	m_flashRadian += Angle::ConvertToRadian(10.0f);

	if (m_waitTimer.UpdateTimer(arg_timeScale))
	{
		m_drawChangeTimer.UpdateTimer(arg_timeScale);
	}

	//HPバーが大きくなっていく
	float sizeRate = KuroMath::Ease(In, Sine,
		m_drawChangeTimer.GetTimeRate(),
		m_startHpRate,
		m_endHpRate);
	Vec2<float>hpBarSize = CalculateHpBarSize(arg_hpBarSprite.lock()->GetTex()->GetGraphSize().Float(), sizeRate);
	arg_hpBarSprite.lock()->m_mesh.SetSize(hpBarSize);

	//回復分HPバー点滅
	float alpha = KuroMath::Ease(In, Liner, abs(cos(m_flashRadian)), 1.0f, 0.5f);
	m_hpBarHeal.m_sprite->SetColor(Color(1.0f, 1.0f, 1.0f, alpha));

	if (m_ptEmitTimer.UpdateTimer(arg_timeScale))
	{
		//1度に放出するパーティクルの数を乱数で決定
		int ptNum = KuroFunc::GetRand(1, 3);

		//HPバーの縦方向真ん中
		hpBarSize.y /= 2.0f;

		//延長分のHPバーの長さ
		float extendHpBarDifferX = CalculateHpBarSize(arg_hpBarSprite.lock()->GetTex()->GetGraphSize().Float(), m_endHpRate).x - hpBarSize.x;

		//出現位置の乱数オフセット最大範囲を増やす
		static const float EXPAND_RAND_X = 16.0f;

		for (int i = 0; i < ptNum; ++i)
		{
			//変化するHPバーの右端の座標基準にパーティクル放出
			EmitParticle(arg_uiPos + arg_hpBarSprite.lock()->m_transform.GetPos() + hpBarSize, extendHpBarDifferX + EXPAND_RAND_X);
		}

		m_ptEmitTimer.Reset();
	}
}