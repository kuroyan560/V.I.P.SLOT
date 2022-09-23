#pragma once
#include<memory>
#include<vector>
#include"Light.h"
#include<wrl.h>
#include<d3d12.h>

class ConstantBuffer;
class StructuredBuffer;
class DescriptorData;

class LightManager
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	static const int MAX_LIG_NUM[Light::TYPE_NUM];
private:
	std::vector<Light::Direction*>dirLights;
	std::vector<Light::Point*>ptLights;
	std::vector<Light::Spot*>spotLights;
	std::vector<Light::HemiSphere*>hemiSphereLights;

	//�A�N�e�B�u���̃��C�g�̐��𑗂邽�߂̒萔�o�b�t�@�f�[�^
	struct LightNum
	{
		int dirLigNum;
		int ptLigNum;
		int spotLigNum;
		int hemiSphereNum;
	};
	std::shared_ptr<ConstantBuffer>ligNumConstBuff;

	//���C�g���p�\�����o�b�t�@
	std::shared_ptr<StructuredBuffer>ligStructuredBuff[Light::TYPE_NUM];

	//�A�N�e�B�u���̃��C�g�̐����}�b�s���O
	void MappingLigNum();
	// (�����Ŏw�肵���^�C�v��) ���C�g�����}�b�s���O�ADirty�t���O���m�F���邩
	void MappingLigInfo(const Light::TYPE& Type, const bool& CheckDirty = true);

public:
	LightManager();

	void RegisterDirLight(Light::Direction* DirLight);
	void RegisterPointLight(Light::Point* PtLight);
	void RegisterSpotLight(Light::Spot* SpotLight);
	void RegisterHemiSphereLight(Light::HemiSphere* HemiSphereLight);

	std::shared_ptr<DescriptorData>GetLigNumInfo();
	std::shared_ptr<DescriptorData>GetLigInfo(const Light::TYPE& Type);
};