#pragma once
#include"Vec.h"
#include<string>
#include<memory>
#include<array>

class ConstantBuffer;
class TextureBuffer;
enum MATERIAL_TEX_TYPE { COLOR_TEX, EMISSIVE_TEX, NORMAL_TEX, BASE_COLOR_TEX, METALNESS_TEX, ROUGHNESS_TEX, MATERIAL_TEX_TYPE_NUM };

class Material
{
private:
	bool invalid = true;
public:
	std::string name = "DefaultMaterial";
	struct Lambert
	{
		Vec3<float> ambient = { 0.2f,0.2f,0.2f };	//����(�S�������瓖�����)
		float ambientFactor = 1.0f;			//���x(weight)
		Vec3<float> diffuse = { 1.0f,1.0f,1.0f };	//�g�U���ˌ�(���f�����{�������Ă���F��)
		float diffuseFactor = 1.0f;
		Vec3<float> emissive = { 0,0,0 };		//���ˌ�(���f�����g�������A�Â����Ō���)
		float emissiveFactor = 1.0f;
		float transparent = 0.0f;				//���ߓx
	};
	struct Phong
	{
		Vec3<float> specular = { 1,1,1 };	//�X�y�L����(���f���̉�)
		float specularFactor = 1.0f;
		float shininess = 0.0f;					//����
		float reflection = 0.0f;					//����(�l���傫���Ƌ��̂悤�ɂȂ�)
		float pad;
	};
	struct PBR
	{
		Vec3<float>baseColor = { 0.0f,0.0f,0.0f };
		//float subsurface = 0.0f;
		float metalness = 0.0f;
		float specular = 0.5f;
		//Vec3<float> specularColor = { 1.0f,1.0f,1.0f };
		float roughness = 0.0f;
		//float specularAnisotropy = 0.0f;
		//float sheen = 0.0f;
		//Vec3<float> sheenColor = { 1.0f,1.0f,1.0f };
		//float coarRoughness = 0.0f;
		//float coat = 0.0f;
	};
	struct ConstData
	{
		Lambert lambert;
		Phong phong;
		PBR pbr;
	}constData;
	std::shared_ptr<ConstantBuffer>buff;

	std::array<std::shared_ptr<TextureBuffer>, MATERIAL_TEX_TYPE_NUM> texBuff;

	Material();
	void CreateBuff();
	void Mapping();
};