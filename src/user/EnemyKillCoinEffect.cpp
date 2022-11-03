#include "EnemyKillCoinEffect.h"
#include"Angle.h"
#include"Importer.h"

EnemyKillCoinEffect::Info::Info(const EnemyKillCoinEffect& arg_parent, Vec3<float> arg_playerPos, int arg_coinNum)
	:m_coinNum(arg_coinNum) 
{
	//出現位置オフセット
	const Vec3<float>APPEAR_POS_OFFSET = { 0.0f,1.0f,0.0f };
	arg_playerPos += APPEAR_POS_OFFSET;

	const std::array<int, STATUS::NUM>TIME = { 60,45,60 };
	for (int statusIdx = 0; statusIdx < STATUS::NUM; ++statusIdx)
	{
		m_timer[statusIdx].Reset(TIME[statusIdx]);
	}

	//コインの桁数取得
	int digit = KuroFunc::GetDigit(arg_coinNum);

	//表示文字数
	int modelNum = digit + 1;
	m_numbers.resize(digit + 1);

	//０番目は「＋」
	m_numbers[0].m_numIdx = arg_parent.PLUS_IDX;
	//各桁の数字取得
	for (int i = 1; i < modelNum; ++i)
	{
		//桁数取得（＋のインデックス分 -1）
		int d = i - 1;
		//指定桁の数字取得
		m_numbers[i].m_numIdx = KuroFunc::GetSpecifiedDigitNum(arg_coinNum, d, true);
	}

	//字間
	const float LETTER_SPACE = 1.8f;

	//左端のオフセット位置を求める
	float left = -LETTER_SPACE * std::floor(modelNum / 2.0f);
	if (modelNum % 2 == 0)left -= LETTER_SPACE / 2.0f;	//桁数が偶数

	//トランスフォーム設定
	for (int i = 0; i < modelNum; ++i)
	{
		//親トランスフォーム登録
		m_numbers[i].m_transform.SetParent(&m_transform);
		//オフセット適用
		m_numbers[i].m_transform.SetPos({ left + LETTER_SPACE * i,0.0f,0.0f });
	}

	//初期座標
	m_transform.SetPos(arg_playerPos);
	m_startPos = arg_playerPos;
}

void EnemyKillCoinEffect::Info::Update(float arg_timeScale)
{
	//数字表記のY軸上昇量
	const std::array<float, STATUS::NUM> FLOAT_HEIGHT = { 2.0f,0.0f,2.0f };
	//イージング変化タイプ
	const std::array<EASE_CHANGE_TYPE, STATUS::NUM> EASE_CHANGE_TYPE_ARRAY = { Out,In,In };
	//イージング種類
	const std::array<EASING_TYPE, STATUS::NUM> EASING_TYPE_ARRAY = { Quint,Liner,Back };
	//回転量
	const std::array<Angle, STATUS::NUM> SPIN_ANGLE_ARRAY = { Angle(360),0,Angle(360) };


	//現在の座標取得
	auto pos = m_transform.GetPos();

	//タイマー計測
	if (m_timer[m_status].UpdateTimer(arg_timeScale))
	{
		//ステータスの進行
		if (m_status < NUM)
		{
			m_status = STATUS(m_status + 1);
			m_startPos = m_transform.GetPos();
			m_startAngleY = m_angleY;
		}
	}
	//更新
	else
	{
		//経過時間レート取得
		float rate = m_timer[m_status].GetTimeRate();

		//上昇
		pos.y = KuroMath::Ease(EASE_CHANGE_TYPE_ARRAY[m_status], EASING_TYPE_ARRAY[m_status], rate, m_startPos.y, m_startPos.y + FLOAT_HEIGHT[m_status]);

		//回転
		m_angleY = KuroMath::Ease(EASE_CHANGE_TYPE_ARRAY[m_status], EASING_TYPE_ARRAY[m_status], rate, m_startAngleY, m_startAngleY + SPIN_ANGLE_ARRAY[m_status]);
	}

	//変更後の座標適用
	m_transform.SetPos(pos);
	//変更後の回転量を各オフセットトランスフォームに適用
	for (auto& num : m_numbers)
	{
		num.m_transform.SetRotate(0, m_angleY, 0);
	}
}

#include"DrawFunc3D.h"
void EnemyKillCoinEffect::Info::Draw(const EnemyKillCoinEffect& arg_parent, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	for (auto& number : m_numbers)
	{
		DrawFunc3D::DrawNonShadingModel(
			arg_parent.m_numModels[number.m_numIdx], 
			number.m_transform, 
			*arg_cam.lock(),
			1.0f, 
			nullptr,
			AlphaBlendMode_None);
	}
}

EnemyKillCoinEffect::EnemyKillCoinEffect()
{
	//モデル読み込み
	std::string dir = "resource/user/model/num/";
	std::array<std::string, MODEL_NUM>modelName =
	{
		"zero","one","two","three","four","five","six","seven","eight","nine","plus"
	};
	for (int modelIdx = 0; modelIdx < MODEL_NUM; ++modelIdx)
	{
		m_numModels[modelIdx] = Importer::Instance()->LoadModel(dir, modelName[modelIdx] + ".glb");
	}
}

void EnemyKillCoinEffect::Init()
{
	m_infoArray.clear();
}

void EnemyKillCoinEffect::Update(float arg_timeScale)
{
	for (auto& info : m_infoArray)
	{
		info.Update(arg_timeScale);
	}

	//死んでいたら削除
	m_infoArray.remove_if([](Info& info)
		{
			return info.IsEnd();
		});
}

void EnemyKillCoinEffect::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& info : m_infoArray)
	{
		info.Draw(*this, arg_lightMgr, arg_cam);
	}
}

void EnemyKillCoinEffect::Emit(Vec3<float> arg_emitPos, int arg_coinNum)
{
	m_infoArray.emplace_front(*this, arg_emitPos, arg_coinNum);
}
