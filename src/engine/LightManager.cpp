#include "LightManager.h"
#include"D3D12App.h"

const std::array<int, Light::TYPE_NUM> LightManager::MAX_LIG_NUM =
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
			if (dirLights[i]->dirty)send = true;
			dirLights[i]->dirty = false;
			sendData.emplace_back(dirLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::DIRECTION]->Mapping(sendData.data(), static_cast<int>(sendData.size()));
	}
	else if (Type == Light::POINT)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::Point::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::POINT]);
		for (int i = 0; i < ptLights.size(); ++i)
		{
			if (ptLights[i]->dirty)send = true;
			ptLights[i]->dirty = false;
			sendData.emplace_back(ptLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::POINT]->Mapping(sendData.data(), static_cast<int>(sendData.size()));
	}
	else if (Type == Light::SPOT)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::Spot::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::SPOT]);
		for (int i = 0; i < spotLights.size(); ++i)
		{
			if (spotLights[i]->dirty)send = true;
			spotLights[i]->dirty = false;
			sendData.emplace_back(spotLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::SPOT]->Mapping(sendData.data(), static_cast<int>(sendData.size()));
	}
	else if (Type == Light::HEMISPHERE)
	{
		//�o�^�����|�C���g���C�g�𑗐M�p�̔z��ɃR�s�[
		std::vector<Light::HemiSphere::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::HEMISPHERE]);
		for (int i = 0; i < hemiSphereLights.size(); ++i)
		{
			if (hemiSphereLights[i]->dirty)send = true;
			hemiSphereLights[i]->dirty = false;
			sendData.emplace_back(hemiSphereLights[i]->constData);
		}

		if (!send)return;

		//�R�s�[�����f�[�^���}�b�s���O
		ligStructuredBuff[Light::HEMISPHERE]->Mapping(sendData.data(), static_cast<int>(sendData.size()));
	}
}

void LightManager::OnImguiItems()
{
	if (ImGui::TreeNode("Direcion"))
	{
		if (dirLights.empty())ImGui::Text("Empty");
		else
		{
			ImGui::BeginChild(ImGui::GetID((void*)0));
			for (auto& dirLig : dirLights)
			{
				bool active = dirLig->constData.active == 1;
				ImGui::Checkbox("Active", &active);
				dirLig->SetActive(active);

				auto col = dirLig->constData.color;
				ImGui::ColorPicker4("Color", (float*)&col);
				dirLig->SetColor(col);

				auto dir = dirLig->constData.dir;
				ImGui::DragFloat("DirX", &dir.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("DirY", &dir.y, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("DirZ", &dir.z, 0.01f, -1.0f, 1.0f);
				dirLig->SetDir(dir);
			}
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Point"))
	{
		if (ptLights.empty())ImGui::Text("Empty");
		else
		{
			ImGui::BeginChild(ImGui::GetID((void*)0));
			for (auto& ptLig : ptLights)
			{
				bool active = ptLig->constData.active == 1;
				ImGui::Checkbox("Active", &active);
				ptLig->SetActive(active);

				auto col = ptLig->constData.color;
				ImGui::ColorPicker4("Color", (float*)&col);
				ptLig->SetColor(col);

				auto pos = ptLig->constData.pos;
				ImGui::DragFloat("PosX", &pos.x, 0.01f);
				ImGui::DragFloat("PosY", &pos.y, 0.01f);
				ImGui::DragFloat("PosZ", &pos.z, 0.01f);
				ptLig->SetPos(pos);

				auto range = ptLig->constData.influenceRange;
				ImGui::DragFloat("InfluenceRange", &range);
				ptLig->SetInfluenceRange(range);
			}
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Spot"))
	{
		if (spotLights.empty())ImGui::Text("Empty");
		else
		{
			ImGui::BeginChild(ImGui::GetID((void*)0));
			for (auto& spotLig : spotLights)
			{
				bool active = spotLig->constData.active == 1;
				ImGui::Checkbox("Active", &active);
				spotLig->SetActive(active);

				auto col = spotLig->constData.color;
				ImGui::ColorPicker4("Color", (float*)&col);
				spotLig->SetColor(col);

				auto pos = spotLig->constData.pos;
				ImGui::DragFloat("PosX", &pos.x, 0.01f);
				ImGui::DragFloat("PosY", &pos.y, 0.01f);
				ImGui::DragFloat("PosZ", &pos.z, 0.01f);
				spotLig->SetPos(pos);

				auto target = spotLig->constData.target;
				ImGui::DragFloat("TargetX", &target.x, 0.01f);
				ImGui::DragFloat("TargetY", &target.y, 0.01f);
				ImGui::DragFloat("TargetZ", &target.z, 0.01f);
				spotLig->SetTarget(target);

				auto range = spotLig->constData.influenceRange;
				ImGui::DragFloat("InfluenceRange", &range);
				spotLig->SetInfluenceRange(range);

				auto angle = spotLig->constData.angle.GetDegree();
				ImGui::DragFloat("Angle(Degree)", &angle);
				spotLig->SetAngle(Angle(Angle::ConvertToRadian(angle)));
			}
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}
}

LightManager::LightManager() : Debugger("LightManager")
{
	//�O�̏�Ԃ̑��M�p�f�[�^
	LightNum sendLigNum = { 0,0,0,0 };
	ligNumConstBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(LightNum), 1, &sendLigNum, "LightsNum");

	//�f�B���N�V�������C�g�o�b�t�@
	std::vector<Light::Direction::ConstData>dirInitSendData(MAX_LIG_NUM[Light::DIRECTION], Light::Direction::ConstData::ZeroData());
	ligStructuredBuff[Light::DIRECTION] = D3D12App::Instance()->GenerateStructuredBuffer(
		sizeof(Light::Direction::ConstData), MAX_LIG_NUM[Light::DIRECTION], dirInitSendData.data(), "LightInfo - Direction");

	//�|�C���g���C�g�o�b�t�@
	std::vector<Light::Point::ConstData>ptInitSendData(MAX_LIG_NUM[Light::POINT], Light::Point::ConstData::ZeroData());
	ligStructuredBuff[Light::POINT] = D3D12App::Instance()->GenerateStructuredBuffer(
		sizeof(Light::Point::ConstData), MAX_LIG_NUM[Light::POINT], ptInitSendData.data(), "LightInfo - Point");

	//�X�|�b�g���C�g�o�b�t�@
	std::vector<Light::Spot::ConstData>spotInitSendData(MAX_LIG_NUM[Light::SPOT], Light::Spot::ConstData::ZeroData());
	ligStructuredBuff[Light::SPOT] = D3D12App::Instance()->GenerateStructuredBuffer(
		sizeof(Light::Spot::ConstData), MAX_LIG_NUM[Light::SPOT], spotInitSendData.data(), "LightInfo - Spot");

	//�X�J�C�h�[���o�b�t�@
	std::vector<Light::HemiSphere::ConstData>hemiInitSendData(MAX_LIG_NUM[Light::HEMISPHERE], Light::HemiSphere::ConstData::ZeroData());
	ligStructuredBuff[Light::HEMISPHERE] = D3D12App::Instance()->GenerateStructuredBuffer(
		sizeof(Light::HemiSphere::ConstData), MAX_LIG_NUM[Light::HEMISPHERE], hemiInitSendData.data(), "LightInfo - HemiSphere");
}

void LightManager::RegisterDirLight(Light::Direction* DirLight)
{
	dirLights.emplace_back(DirLight);
	assert(dirLights.size() <= MAX_LIG_NUM[Light::DIRECTION]);

	//Dirty�t���O�͌����A�f�B���N�V�������C�g���������}�b�s���O
	MappingLigInfo(Light::DIRECTION, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterPointLight(Light::Point* PtLight)
{
	ptLights.emplace_back(PtLight);
	assert(ptLights.size() <= MAX_LIG_NUM[Light::POINT]);

	//Dirty�t���O�͌����A�|�C���g���C�g���������}�b�s���O
	MappingLigInfo(Light::POINT, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterSpotLight(Light::Spot* SpotLight)
{
	spotLights.emplace_back(SpotLight);
	assert(spotLights.size() <= MAX_LIG_NUM[Light::SPOT]);

	//Dirty�t���O�͌����A�X�|�b�g���C�g���������}�b�s���O
	MappingLigInfo(Light::SPOT, false);

	//���̏����X�V
	MappingLigNum();
}

void LightManager::RegisterHemiSphereLight(Light::HemiSphere* HemiSphereLight)
{
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