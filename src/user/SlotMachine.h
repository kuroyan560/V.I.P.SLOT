#pragma once
#include<memory>
#include<array>
#include<vector>
#include<string>
class ModelObject;
class LightManager;
class Camera;
class TextureBuffer;
class ModelMesh;

class SlotMachine
{
	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//リール
	enum REEL { LEFT, CENTER, RIGHT, NUM };
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	class Reel
	{
	private:
		//対象のメッシュポインタ
		ModelMesh* m_meshPtr = nullptr;
		//UV値の初期化値
		std::vector<float>m_initV;

		//回転量（UV値のV）
		float m_vOffset = 0.0f;

		//回転中か
		bool m_isSpin = false;
		//回転速度
		float m_spinSpeed;

		//回転始めフラグ（助走？最高速度になるまでの間はStop出来ない）
		bool m_isStartSpin = false;
		//時間計測
		int m_timer;

		//回転をUV値に反映
		void SpinAffectUV();
		
	public:
		//リールメッシュアタッチ
		void Attach(ModelMesh* ReelMesh);
		//初期化
		void Init(std::shared_ptr<TextureBuffer>ReelTex = nullptr);
		//更新（回転）
		void Update();

		//回転スタート
		void Start();
		//回転ストップ
		void Stop();

		//回転停止を受け付けているか
		bool IsCanStop() { return !m_isStartSpin; }
	};
	std::array<Reel, REEL::NUM>m_reels;

	//スロットのレバーフラグ（StartかStopか）
	int m_lever;

	//サウンド
	int m_spinStartSE;	//回転スタート
	int m_reelStopSE;		//リールストップ
	
public:
	SlotMachine();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};