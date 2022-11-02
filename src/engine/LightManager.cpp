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
	//送信するかどうかのフラグ
	bool send = !CheckDirty;

	if (Type == Light::DIRECTION)
	{
		//Dirtyフラグをチェックしつつ、登録したディレクションライトを送信用の配列にコピー
		std::vector<Light::Direction::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::DIRECTION]);
		for (int i = 0; i < dirLights.size(); ++i)
		{
			if (!send && dirLights[i]->dirty)send = true;
			dirLights[i]->dirty = false;
			sendData.emplace_back(dirLights[i]->constData);
		}

		if (!send)return;

		//コピーしたデータをマッピング
		ligStructuredBuff[Light::DIRECTION]->Mapping(&sendData[0]);
	}
	else if (Type == Light::POINT)
	{
		//登録したポイントライトを送信用の配列にコピー
		std::vector<Light::Point::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::POINT]);
		for (int i = 0; i < ptLights.size(); ++i)
		{
			if (!send && ptLights[i]->dirty)send = true;
			ptLights[i]->dirty = false;
			sendData.emplace_back(ptLights[i]->constData);
		}

		if (!send)return;

		//コピーしたデータをマッピング
		ligStructuredBuff[Light::POINT]->Mapping(&sendData[0]);
	}
	else if (Type == Light::SPOT)
	{
		//登録したポイントライトを送信用の配列にコピー
		std::vector<Light::Spot::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::SPOT]);
		for (int i = 0; i < spotLights.size(); ++i)
		{
			if (!send && spotLights[i]->dirty)send = true;
			spotLights[i]->dirty = false;
			sendData.emplace_back(spotLights[i]->constData);
		}

		if (!send)return;

		//コピーしたデータをマッピング
		ligStructuredBuff[Light::SPOT]->Mapping(&sendData[0]);
	}
	else if (Type == Light::HEMISPHERE)
	{
		//登録したポイントライトを送信用の配列にコピー
		std::vector<Light::HemiSphere::ConstData>sendData;
		sendData.reserve(MAX_LIG_NUM[Light::HEMISPHERE]);
		for (int i = 0; i < hemiSphereLights.size(); ++i)
		{
			if (!send && hemiSphereLights[i]->dirty)send = true;
			hemiSphereLights[i]->dirty = false;
			sendData.emplace_back(hemiSphereLights[i]->constData);
		}

		if (!send)return;

		//コピーしたデータをマッピング
		ligStructuredBuff[Light::HEMISPHERE]->Mapping(&sendData[0]);
	}
}

LightManager::LightManager()
{
	//０個の状態の送信用データ
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

	//Dirtyフラグは見ず、ディレクションライト情報を強制マッピング
	MappingLigInfo(Light::DIRECTION, false);

	//数の情報も更新
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

	//Dirtyフラグは見ず、ポイントライト情報を強制マッピング
	MappingLigInfo(Light::POINT, false);

	//数の情報も更新
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

	//Dirtyフラグは見ず、スポットライト情報を強制マッピング
	MappingLigInfo(Light::SPOT, false);

	//数の情報も更新
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

	//Dirtyフラグは見ず、天球ライト情報を強制マッピング
	MappingLigInfo(Light::HEMISPHERE, false);

	//数の情報も更新
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

#include"ImguiApp.h"
void LightManager::ImguiDebug()
{
	ImGui::Begin("LightManager");

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
				ImGui::DragFloat("Red", &col.m_r, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Green", &col.m_g, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Blue", &col.m_b, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Alpha", &col.m_a, 0.01f, 0.0f, 1.0f);
				dirLig->SetColor(col);

				auto dir = dirLig->constData.dir;
				ImGui::DragFloat("DirX", &dir.x, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("DirY", &dir.y, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("DirZ", &dir.z, 0.01f, 0.0f, 1.0f);
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
			for (auto& ptLig	 : ptLights)
			{
				bool active = ptLig->constData.active == 1;
				ImGui::Checkbox("Active", &active);
				ptLig->SetActive(active);

				auto col = ptLig->constData.color;
				ImGui::DragFloat("Red", &col.m_r, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Green", &col.m_g, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Blue", &col.m_b, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Alpha", &col.m_a, 0.01f, 0.0f, 1.0f);
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
				ImGui::DragFloat("Red", &col.m_r, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Green", &col.m_g, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Blue", &col.m_b, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Alpha", &col.m_a, 0.01f, 0.0f, 1.0f);
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

	ImGui::End();
}
