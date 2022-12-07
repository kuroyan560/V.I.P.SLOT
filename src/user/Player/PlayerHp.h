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
		//HPバー（ダメージ）
		DrawContents m_hpBarDamage;
		//待機時間
		Timer m_waitTimer;
		//描画での変化タイマー
		Timer m_drawChangeTimer;
		//揺れ
		ImpactShake m_shake = ImpactShake(Vec3<float>(1.0f, 1.0f, 0.0f));

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;

		void Init()
		{
			m_shake.Init();
			m_hpBarDamage.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate)
		{
			m_hpBarDamage.m_active = true;
			m_drawChangeTimer.Reset(30.0f);
			m_waitTimer.Reset(45.0f);
			m_shake.Shake(30.0f, 3.0f, 0.0f, 32.0f);
			m_startHpRate = arg_startHpRate;
			m_endHpRate = arg_endHpRate;
		}
		void Update(float arg_timeScale)
		{
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
		}
		void Interruput() { Init(); }
	}m_damageEffect;

	/*--- HP回復演出 ---*/
	struct HealEffect
	{
		//HPバー（回復）
		DrawContents m_hpBarHeal;
		//待機時間
		Timer m_waitTimer;
		//描画での変化タイマー
		Timer m_drawChangeTimer;
		//点滅ラジアン
		float m_flashRadian;

		float m_startHpRate = 0.0f;
		float m_endHpRate = 0.0f;

		void Init()
		{
			m_hpBarHeal.m_active = false;
		}
		void Start(float arg_startHpRate, float arg_endHpRate)
		{
			m_hpBarHeal.m_active = true;
			m_drawChangeTimer.Reset(30.0f);
			m_waitTimer.Reset(45.0f);
			m_startHpRate = arg_startHpRate;
			m_endHpRate = arg_endHpRate;
			m_flashRadian = 0.0f;
		}
		void Update(float arg_timeScale)
		{
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
		}
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
	Vec2<float>CalculateHpBarSize(float arg_rate);

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