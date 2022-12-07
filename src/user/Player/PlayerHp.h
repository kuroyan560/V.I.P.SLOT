#pragma once
#include<memory>
#include"Transform2D.h"
#include<vector>
#include"HUDInterface.h"
#include"Timer.h"
#include"ImpactShake.h"
class TextureBuffer;
class Sprite;

class PlayerHp : public HUDInterface
{
	//HPのUI全体のトランスフォーム
	Transform2D m_transform;
	//初期化位置
	Vec2<float>m_uiInitPos;

	struct DrawContents
	{
		Vec2<float>m_initPos;
		std::shared_ptr<Sprite>m_sprite;
		bool m_active = true;
	};

	//「HP」文字
	DrawContents m_hpStr;
	//HPバー
	DrawContents m_hpBar;
	//HPバーフレーム
	DrawContents m_hpBarFrame;

	struct Heart
	{
		Vec2<float>m_initPos;
		float m_initScale;
		Transform2D m_offsetTransform;
		DrawContents m_heart;
		DrawContents m_frame;
	};
	std::vector<Heart>m_heartArray;

	//ライフ（０でゲームオーバー）
	int m_life;
	//HP（HP０でライフが１減る）
	int m_hp;

	//描画するスプライト情報の配列
	std::vector<DrawContents*>m_contents;

	/*--- ダメージ演出 ---*/
	struct DamageEffect
	{
	private:
		//待機時間
		Timer m_waitTimer;
		//描画での変化タイマー
		Timer m_drawChangeTimer;
		//揺れ
		ImpactShake m_shake = ImpactShake(Vec3<float>(1.0f, 1.0f, 0.0f));

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;
	public:
		//HPバー（ダメージ）
		DrawContents m_hpBarDamage;
		void Init()
		{
			m_shake.Init();
			m_hpBarDamage.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate, std::weak_ptr<Sprite>arg_hpBarSprite);
		void Update(float arg_timeScale);
		void Interruput() { Init(); }
		Vec2<float>GetOffset()const
		{
			return { m_shake.GetOffset().x,m_shake.GetOffset().y };
		}
	}m_damageEffect;

	/*--- HP回復演出 ---*/
	struct HealEffect
	{
	private:
		//待機時間
		Timer m_waitTimer;
		//描画での変化タイマー
		Timer m_drawChangeTimer;
		//パーティクルの放出スパン
		Timer m_ptEmitTimer;

		//点滅ラジアン
		float m_flashRadian;

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;

		//ParticleMgrから受け取ったパーティクルID
		int m_particleID;

		void EmitParticle(Vec2<float>arg_basePos, float arg_randMaxPosX);

	public:
		//HPバー（回復）
		DrawContents m_hpBarHeal;

		HealEffect();

		void Init()
		{
			m_hpBarHeal.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate);
		void Update(float arg_timeScale, Vec2<float>arg_uiPos, std::weak_ptr<Sprite>arg_hpBarSprite);
		void Interruput() { Init(); }
	}m_healEffect;

	/*--- ライフ消費演出 ---*/
	struct ConsumeLifeEffect
	{
		//メインライフ（ダメージ）
		DrawContents m_damageHeart;

		void Init()
		{
			m_damageHeart.m_active = false;
		}
		void Start(DrawContents* arg_hpBar);
		void Update(float arg_timeScale, DrawContents* arg_hpBar);
	}m_consumeLifeEffect;

	//HPバーサイズ取得
	static Vec2<float>CalculateHpBarSize(Vec2<float>arg_texSize, float arg_rate);

	//描画
	void OnDraw2D()override;

public:
	PlayerHp();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="arg_initAbilityMaxLife">現在解放されている最大ライフ（能力値上のの最大）</param>
	/// <param name="arg_initRemainLife">残ライフ</param>
	void Init(int arg_initAbilityMaxLife, int arg_initRemainLife);

	//更新
	void Update(const float& arg_timeScale);

	//imguiデバッグ
	void ImguiDebug();

	/// <summary>
	/// HP量変動
	/// </summary>
	/// <param name="arg_amount">変動量</param>
	/// <returns>ライフ(ハート)を消費したか</returns>
	bool Change(int arg_amount);

	//ライフゲッタ
	const int& GetLife()const { return m_life; }
	//死亡したか
	bool IsDead()const { return m_hp <= 0 && m_life <= 0; }
};