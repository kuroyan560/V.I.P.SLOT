#pragma once
#include<string>
#include<memory>
class ModelObject;
class LightManager;
class Camera;

class Player
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;
public:
	Player();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};