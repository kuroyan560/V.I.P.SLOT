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

	public:
		//回転量（UV値のV）
		float m_vOffset = 0.0f;
		//リールメッシュアタッチ
		void Attach(ModelMesh* ReelMesh);
		//初期化
		void Init(std::shared_ptr<TextureBuffer>ReelTex = nullptr);
		//回転をUV値に反映
		void SpinAffectUV();
	};
	std::array<Reel, REEL::NUM>m_reels;
	
public:
	SlotMachine();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};