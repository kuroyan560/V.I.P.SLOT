#include "LightManager.h"
#include"D3D12App.h"

const int LightManager::MAX_LIG_NUM[Light::TYPE_NUM] =
{
	10,100,100,3
};

void LightManager::MappingLigNum()
{
	LightNum ligNum = 
	{
		static_cast<int>(dirLights.size()),
		static_cast<int>(ptLights.size()),
		static_cast<int>(spotLights.size()),
		static_cast<int>(hemiSphereLights.size())
	};
	ligNumConstBuff->Mapping(&ligNum);
}

void LightManager::MappingLigInfo(const Light::TYPE& Type, const bool& CheckDirty)
{
	//���M���邩�ǂ����̃t���O
	bool send = !CheckDirty;

	if (Type == Light::DIRECTION)
	{
		//Dirty�t���O���`�F�b�N���A�o�^�����f�B���N�V�������C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::Direction::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::DIRECTION]);
		for (int i = 0; i < dirLights.size(); ++i)
		{
			if (!send && dirLights[i]->dirty)send = true;
			dirLights[i]->dirty = false;
			sendData.emplace_back(dirLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::DIRECTION]->Mapping(&sendData[0]);
	}
	else if (Type == Light::POINT)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::Point::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::POINT]);
		for (int i = 0; i < ptLights.size(); ++i)
		{
			if (!send && ptLights[i]->dirty)send = true;
			ptLights[i]->dirty = false;
			sendData.emplace_back(ptLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::POINT]->Mapping(&sendData[0]);
	}
	else if (Type == Light::SPOT)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::Spot::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::SPOT]);
		for (int i = 0; i < spotLights.size(); ++i)
		{
			if (!send && spotLights[i]->dirty)send = true;
			spotLights[i]->dirty = false;
			sendData.emplace_back(spotLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::SPOT]->Mapping(&sendData[0]);
	}
	else if (Type == Light::HEMISPHERE)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::HemiSphere::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::HEMISPHERE]);
		for (int i = 0; i < hemiSphereLights.size(); ++i)
		{
			if (!send && hemiSphereLights[i]->dirty)send = true;
			hemiSphereLights[i]->dirty = false;
			sendData.emplace_back(hemiSphereLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::HEMISPHERE]->Mapping(&sendData[0]);
	}
}

LightManager::LightManager()
{
	//�O�̏�Ԃ̑��M�p�f�[�^
	LightNum sendLigNum = { 0,0,0,0 };
	ligNumConstBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(LightNum), 1, &sendLigNum, "LightsNum");
}

void LightManager::RegisterDirLight(Light::Direction* DirLight)
{
	if (!ligStructuredBuff[Light::DIRECTION])
	{
		ligStructuredBuff[Light::DIRECTION] = D3D12App::Instance()->GenerateStructuredBuffer(
			sizeof(Light::Direction::ConstData), MAX_LIG_NUM[Light::DIRECTION], nullptr, "LightInfo - Direction");
	}

	dirLights.emplace_back(DirLight);
	assert(dirLights.size() <= MAX_LIG_NUM[Light::DIRECTION]);

	//Dirty�t���O�͌����A�f�B���N�V�������C�g���������}�b�s���O
	MappingLigInfo(Light::DIRECTION, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterPointLight(Light::Point* PtLight)
{
	if (!ligStructuredBuff[Light::POINT])
	{
		ligStructuredBuff[Light::POINT] = D3D12App::Instance()->GenerateStructuredBuffer(
			sizeof(Light::Point::ConstData), MAX_LIG_NUM[Light::POINT], nullptr, "LightInfo - Point");
	}

	ptLights.emplace_back(PtLight);
	assert(ptLights.size() <= MAX_LIG_NUM[Light::POINT]);

	//Dirty�t���O�͌����A�|�C���g���C�g���������}�b�s���O
	MappingLigInfo(Light::POINT, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterSpotLight(Light::Spot* SpotLight)
{
	if (!ligStructuredBuff[Light::SPOT])
	{
		ligStructuredBuff[Light::SPOT] = D3D12App::Instance()->GenerateStructuredBuffer(
			sizeof(Light::Spot::ConstData), MAX_LIG_NUM[Light::SPOT], nullptr, "LightInfo - Spot");
	}

	spotLights.emplace_back(SpotLight);
	assert(spotLights.size() <= MAX_LIG_NUM[Light::SPOT]);

	//Dirty�t���O�͌����A�X�|�b�g���C�g���������}�b�s���O
	MappingLigInfo(Light::SPOT, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterHemiSphereLight(Light::HemiSphere* HemiSphereLight)
{
	if (!ligStructuredBuff[Light::HEMISPHERE])
	{
		ligStructuredBuff[Light::HEMISPHERE] = D3D12App::Instance()->GenerateStructuredBuffer(
			sizeof(Light::HemiSphere::ConstData), MAX_LIG_NUM[Light::HEMISPHERE], nullptr, "LightInfo - HemiSphere");
	}

	hemiSphereLights.emplace_back(HemiSphereLight);
	assert(hemiSphereLights.size() <= MAX_LIG_NUM[Light::HEMISPHERE]);

	//Dirty�t���O�͌����A�V�����C�g���������}�b�s���O
	MappingLigInfo(Light::HEMISPHERE, false);

	//���̏����X�V
	MappingLigNum();
}

std::shared_ptr<DescriptorData> LightManager::GetLigNumInfo()
{
	return ligNumConstBuff;
}

std::shared_ptr<DescriptorData> LightManager::GetLigInfo(const Light::TYPE& Type)
{
	MappingLigInfo(Type);
	return ligStructuredBuff[Type];
}
