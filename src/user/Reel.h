#pragma once
#include<vector>
#include<memory>
class ModelMesh;
class TextureBuffer;

class Reel
{
private:
	//対象のメッシュポインタ
	ModelMesh* m_meshPtr = nullptr;
	//UV値の初期化値
	std::vector<float>m_initV;

	//絵柄の数
	int m_patternNum = 20;

	//回転量（UV値のV）
	float m_vOffset = 0.0f;

	//回転中か
	bool m_isSpin = false;
	//回転速度
	float m_spinSpeed;

	//回転始めフラグ（助走？最高速度になるまでの間はStop出来ない）
	bool m_isStartSpin = false;
	//回転終了フラグ
	bool m_isEndSpin = false;

	//リール停止時のVオフセット（中途半端な位置にならないよう補正をかけたもの）
	float m_vOffsetFixedStop;

	//時間計測
	int m_timer;

	//スロット結果の絵柄インデックス
	int m_nowPatternIdx = 0;

	//回転をUV値に反映
	void SpinAffectUV();

public:
	//リールメッシュアタッチ
	void Attach(ModelMesh* ReelMesh);
	//初期化
	void Init(std::shared_ptr<TextureBuffer>ReelTex = nullptr, int PatternNum = 20);
	//更新（回転）
	void Update();

	//回転スタート
	void Start();
	//回転ストップ
	void Stop();

	//回転停止を受け付けているか
	bool IsCanStop()const { return !m_isStartSpin; }
};