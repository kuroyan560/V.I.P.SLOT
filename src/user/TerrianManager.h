#pragma once
#include"Transform.h"
#include"Singleton.h"
#include<memory>
#include<array>
#include<forward_list>
#include"TerrianParts.h"

const enum TERRIAN_PARTS { SINGLE_ACT_POINT, TERRIAN_PARTS_NUM };
class LightManager;
class Camera;
class CubeMap;

class TerrianManager : public Singleton<TerrianManager>
{
private:
	friend class Singleton<TerrianManager>;
	TerrianManager();

private:
	//�n�`�p�[�c�̌^�I�u�W�F�N�g
	std::array<std::unique_ptr<TerrianPartsType>, TERRIAN_PARTS_NUM>m_stagePartsTypeArray;
	//�����ϒn�`�z��
	std::forward_list<std::shared_ptr<TerrianParts>>m_terrians;

public:
	//�p�[�c��ݒu
	void Spawn(const TERRIAN_PARTS& PartsType, const Transform& InitTransform);
	//�����ϒn�`���N���A
	void Clear();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(LightManager& LigManager, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap = nullptr);
};