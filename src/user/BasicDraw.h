#pragma once
#include<memory>
#include"LightBloomDevice.h"
#include"Transform.h"
class LightManager;
class ModelObject;
class Model;
class Camera;
class CubeMap;
class SpriteMesh;
class GraphicsPipeline;
class ConstantBuffer;

class BasicDraw
{
	static std::shared_ptr<GraphicsPipeline>s_drawPipeline;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_drawTransformBuff;
	static std::unique_ptr<SpriteMesh>s_spriteMesh;
	static int s_drawCount;
public:
	static void Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum = 100);
	static void CountReset() { s_drawCount = 0; }
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap = nullptr, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap = nullptr);
	static void DrawEdge(std::shared_ptr<TextureBuffer>arg_depthMap);
};