#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Player.h"
#include"GameManager.h"
#include"ShadowMapDevice.h"
#include"LightBloomDevice.h"
#include"DOF.h"
#include<map>

#include"ActPoint.h"

class Model;
class ModelObject;
class GaussianBlur;
class Enemy;
class StaticallyCubeMap;
class DynamicCubeMap;
class TextureBuffer;

class GameScene : public BaseScene
{
	ShadowMapDevice m_shadowMapDevice;
	LightBloomDevice m_lightBloomDevice;
	DOF m_dof;

	std::shared_ptr<ModelObject>m_floorModel;
	std::shared_ptr<Collider>m_floorCol;
	std::shared_ptr<ModelObject>m_stageModel;
	std::shared_ptr<ModelObject>m_debugAxisModel;

	LightManager m_ligMgr;
	Light::Direction m_dirLig;
	Light::HemiSphere m_hemiLig;
	Light::Point m_ptLig;
	
	Player m_player;
	//std::shared_ptr<ModelObject>m_testObj;

	std::shared_ptr<StaticallyCubeMap>m_staticCubeMap;
	std::shared_ptr<DynamicCubeMap>m_dynamicCubeMap;

	//std::shared_ptr<ActPoint> m_testRockOnPoint;
	//std::shared_ptr<ActPoint> m_testMarkingPoint[5];


public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};