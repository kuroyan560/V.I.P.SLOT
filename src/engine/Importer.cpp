﻿#include "Importer.h"
#include"KuroFunc.h"
#include<Windows.h>
#include"Model.h"
#include"D3D12App.h"

void Importer::ErrorMessage(const std::string& FuncName, const bool& Fail, const std::string& Comment)
{
	if (Fail)
	{
		OutputDebugStringA((FuncName + Comment).c_str());
		assert(0);
	}
}

bool Importer::LoadData(FILE* Fp, void* Data, const size_t& Size, const int& ElementNum)
{
	return 1 <= fread(Data, Size, ElementNum, Fp);
}

bool Importer::SaveData(FILE* Fp, const void* Data, const size_t& Size, const int& ElementNum)
{
	return 1 <= fwrite(Data, Size, ElementNum, Fp);
}

void Importer::LoadGLTFPrimitive(ModelMesh& ModelMesh, const Microsoft::glTF::MeshPrimitive& GLTFPrimitive, const Microsoft::glTF::Skin* GLTFSkin, const Microsoft::glTF::GLTFResourceReader& Reader, const Microsoft::glTF::Document& Doc)
{
	using namespace Microsoft::glTF;

	// 頂点位置情報アクセッサの取得
	auto& idPos = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_POSITION);
	auto& accPos = Doc.accessors.Get(idPos);
	auto vertPos = Reader.ReadBinaryData<float>(Doc, accPos);
	// 法線情報アクセッサの取得
	auto& idNrm = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_NORMAL);
	auto& accNrm = Doc.accessors.Get(idNrm);
	auto vertNrm = Reader.ReadBinaryData<float>(Doc, accNrm);
	// テクスチャ座標情報アクセッサの取得
	auto& idUV = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_TEXCOORD_0);
	auto& accUV = Doc.accessors.Get(idUV);
	auto vertUV = Reader.ReadBinaryData<float>(Doc, accUV);

	std::vector<uint8_t>vertJoint;
	if (GLTFPrimitive.HasAttribute(ACCESSOR_JOINTS_0))
	{
		ErrorMessage("LoadGLTFPrimitive", !GLTFSkin, "This primitive has joint's info,but any skin node didn't be passed.");
		auto& idJoint = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_JOINTS_0);
		auto& accJoint = Doc.accessors.Get(idJoint);
		vertJoint = Reader.ReadBinaryData<uint8_t>(Doc, accJoint);
	}

	std::vector<float>vertWeight;
	if (GLTFPrimitive.HasAttribute(ACCESSOR_WEIGHTS_0))
	{
		auto& idWeight = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_WEIGHTS_0);
		auto& accWeight = Doc.accessors.Get(idWeight);
		vertWeight = Reader.ReadBinaryData<float>(Doc, accWeight);
	}

	auto vertexCount = accPos.count;
	for (int vertIdx = 0; vertIdx < vertexCount; ++vertIdx)
	{
		// 頂点データの構築
		int vid0 = 3 * vertIdx, vid1 = 3 * vertIdx + 1, vid2 = 3 * vertIdx + 2;
		int tid0 = 2 * vertIdx, tid1 = 2 * vertIdx + 1;
		int jid0 = 4 * vertIdx, jid1 = 4 * vertIdx + 1, jid2 = 4 * vertIdx + 2, jid3 = 4 * vertIdx + 3;

		ModelMesh::Vertex vertex;
		vertex.pos = { -vertPos[vid0],vertPos[vid1],vertPos[vid2] };
		vertex.normal = { -vertNrm[vid0],vertNrm[vid1],vertNrm[vid2] };
		vertex.uv = { vertUV[tid0],vertUV[tid1] };

		if (!vertWeight.empty())
		{
			vertex.boneWeight = { vertWeight[jid0],vertWeight[jid1],vertWeight[jid2],vertWeight[jid3] };
		}
		if (!vertJoint.empty())
		{
			//ボーン割当がない場合 -1 のままにする必要がある
			auto joint0 = atoi(GLTFSkin->jointIds[vertJoint[jid0]].c_str());
			auto joint1 = atoi(GLTFSkin->jointIds[vertJoint[jid1]].c_str());
			auto joint2 = atoi(GLTFSkin->jointIds[vertJoint[jid2]].c_str());
			auto joint3 = atoi(GLTFSkin->jointIds[vertJoint[jid3]].c_str());
			
			if (vertex.boneWeight.x)vertex.boneIdx.x = static_cast<int>(joint0);
			if (vertex.boneWeight.y)vertex.boneIdx.y = static_cast<int>(joint1);
			if (vertex.boneWeight.z)vertex.boneIdx.z = static_cast<int>(joint2);
			if (vertex.boneWeight.w)vertex.boneIdx.w = static_cast<int>(joint3);
		}

		ModelMesh.mesh->vertices.emplace_back(vertex);
	}

	//タンジェント情報アクセッサ取得
	if (GLTFPrimitive.HasAttribute(ACCESSOR_TANGENT))
	{
		auto& idTangent = GLTFPrimitive.GetAttributeAccessorId(ACCESSOR_TANGENT);
		auto& accTangent = Doc.accessors.Get(idTangent);
		auto vertTangent = Reader.ReadBinaryData<float>(Doc, accTangent);
		for (uint32_t i = 0; i < vertexCount; ++i)
		{
			int vid0 = 3 * i, vid1 = 3 * i + 1, vid2 = 3 * i + 2;
			ModelMesh.mesh->vertices[i].tangent = { vertTangent[vid0],vertTangent[vid1],vertTangent[vid2] };
			ModelMesh.mesh->vertices[i].binormal = ModelMesh.mesh->vertices[i].normal.Cross(ModelMesh.mesh->vertices[i].tangent);
		}
	}

	// 頂点インデックス用アクセッサの取得
	auto& idIndex = GLTFPrimitive.indicesAccessorId;
	auto& accIndex = Doc.accessors.Get(idIndex);

	// インデックスデータ
	std::vector<uint16_t>indices = Reader.ReadBinaryData<uint16_t>(Doc, accIndex);
	auto idxCount = accIndex.count;
	for (int i = 0; i < idxCount; i += 3)
	{
		ModelMesh.mesh->indices.emplace_back(static_cast<unsigned int>(indices[i + 1]));
		ModelMesh.mesh->indices.emplace_back(static_cast<unsigned int>(indices[i]));
		ModelMesh.mesh->indices.emplace_back(static_cast<unsigned int>(indices[i + 2]));
	}
}

void Importer::PrintDocumentInfo(const Microsoft::glTF::Document& document)
{
	// Asset Info
	std::cout << "Asset Version:    " << document.asset.version << "\n";
	std::cout << "Asset MinVersion: " << document.asset.minVersion << "\n";
	std::cout << "Asset Generator:  " << document.asset.generator << "\n";
	std::cout << "Asset Copyright:  " << document.asset.copyright << "\n\n";

	// Scene Info
	std::cout << "Scene Count: " << document.scenes.Size() << "\n";

	if (document.scenes.Size() > 0U)
	{
		std::cout << "Default Scene Index: " << document.GetDefaultScene().id << "\n\n";
	}
	else
	{
		std::cout << "\n";
	}

	// Entity Info
	std::cout << "Node Count:     " << document.nodes.Size() << "\n";
	std::cout << "Camera Count:   " << document.cameras.Size() << "\n";
	std::cout << "Material Count: " << document.materials.Size() << "\n\n";

	// Mesh Info
	std::cout << "Mesh Count: " << document.meshes.Size() << "\n";
	std::cout << "Skin Count: " << document.skins.Size() << "\n\n";

	// Texture Info
	std::cout << "Image Count:   " << document.images.Size() << "\n";
	std::cout << "Texture Count: " << document.textures.Size() << "\n";
	std::cout << "Sampler Count: " << document.samplers.Size() << "\n\n";

	// Buffer Info
	std::cout << "Buffer Count:     " << document.buffers.Size() << "\n";
	std::cout << "BufferView Count: " << document.bufferViews.Size() << "\n";
	std::cout << "Accessor Count:   " << document.accessors.Size() << "\n\n";

	// Animation Info
	std::cout << "Animation Count: " << document.animations.Size() << "\n\n";

	for (const auto& extension : document.extensionsUsed)
	{
		std::cout << "Extension Used: " << extension << "\n";
	}

	if (!document.extensionsUsed.empty())
	{
		std::cout << "\n";
	}

	for (const auto& extension : document.extensionsRequired)
	{
		std::cout << "Extension Required: " << extension << "\n";
	}

	if (!document.extensionsRequired.empty())
	{
		std::cout << "\n";
	}
}

void Importer::PrintResourceInfo(const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader)
{
	using namespace Microsoft::glTF;
	// Use the resource reader to get each mesh primitive's position data
	for (const auto& mesh : document.meshes.Elements())
	{
		std::cout << "Mesh: " << mesh.id << "\n";

		for (const auto& meshPrimitive : mesh.primitives)
		{
			std::string accessorId;

			if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_POSITION, accessorId))
			{
				const Accessor& accessor = document.accessors.Get(accessorId);

				const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
				const auto dataByteLength = data.size() * sizeof(float);

				std::cout << "MeshPrimitive: " << dataByteLength << " bytes of position data\n";
			}
		}

		std::cout << "\n";
	}

	// Use the resource reader to get each image's data
	for (const auto& image : document.images.Elements())
	{
		std::string filename;

		if (image.uri.empty())
		{
			assert(!image.bufferViewId.empty());

			auto& bufferView = document.bufferViews.Get(image.bufferViewId);
			auto& buffer = document.buffers.Get(bufferView.bufferId);

			filename += buffer.uri; //NOTE: buffer uri is empty if image is stored in GLB binary chunk
		}
		else if (IsUriBase64(image.uri))
		{
			filename = "Data URI";
		}
		else
		{
			filename = image.uri;
		}

		auto data = resourceReader.ReadBinaryData(document, image);

		std::cout << "Image: " << image.id << "\n";
		std::cout << "Image: " << data.size() << " bytes of image data\n";

		if (!filename.empty())
		{
			std::cout << "Image filename: " << filename << "\n\n";
		}
	}
}

std::shared_ptr<Model> Importer::CheckAlreadyExsit(const std::string& Dir, const std::string& FileName)
{
	//生成済
	for (auto& m : m_models)
	{
		//既に生成しているものを渡す
		if (m.first == Dir + FileName)return m.second;
	}

	return std::shared_ptr<Model>();
}

/*
std::shared_ptr<Model> Importer::LoadHSMModel(const std::string& Dir, const std::string& FileName, const std::string& Path)
{
	static const std::string FUNC_NAME = "LoadHSMModel";
	static const std::string MSG_TAIL = "の読み取りに失敗\n";
	std::shared_ptr<Model>model = std::make_shared<Model>(Dir, FileName);

	FILE* fp;
	fopen_s(&fp, Path.c_str(), "rb");
	ErrorMessage(FUNC_NAME, fp == nullptr, ".hsmファイルのバイナリ読み取りモードでのオープンに失敗\n");

	//最終更新日時（データ格納はしない）
	FILETIME dummy;
	ErrorMessage(FUNC_NAME, !LoadData(fp, &dummy, sizeof(dummy), 1), "最終更新日時(ダミー)" + MSG_TAIL);

	size_t size;

	//メッシュの数
	unsigned short meshNum;
	ErrorMessage(FUNC_NAME, !LoadData(fp, &meshNum, sizeof(meshNum), 1), "メッシュの数" + MSG_TAIL);

	//メッシュ生成
	model->meshes.resize(meshNum);

	//メッシュ情報
	for (int meshIdx = 0; meshIdx < meshNum; ++meshIdx)
	{
		//メッシュ取得
		auto& mesh = model->meshes[meshIdx];
		mesh.mesh = std::make_shared<Mesh<ModelMesh::Vertex>>();
		mesh.material = std::make_shared<Material>();

		//メッシュ名サイズ
		ErrorMessage(FUNC_NAME, !LoadData(fp, &size, sizeof(size), 1), "メッシュ名サイズ" + MSG_TAIL);
		//メッシュ名
		mesh.mesh->name.resize(size);
		ErrorMessage(FUNC_NAME, !LoadData(fp, &mesh.mesh->name[0], size, 1), "メッシュ名" + MSG_TAIL);

		//頂点数
		unsigned short vertNum;
		ErrorMessage(FUNC_NAME, !LoadData(fp, &vertNum, sizeof(vertNum), 1), "頂点数" + MSG_TAIL);
		//頂点情報
		mesh.mesh->vertices.resize(vertNum);
		ErrorMessage(FUNC_NAME, !LoadData(fp, &mesh.mesh->vertices[0], sizeof(ModelMesh::Vertex), vertNum), "頂点情報" + MSG_TAIL);

		//インデックス数
		unsigned short idxNum;
		ErrorMessage(FUNC_NAME, !LoadData(fp, &idxNum, sizeof(idxNum), 1), "インデックス数" + MSG_TAIL);
		//インデックス情報
		mesh.mesh->indices.resize(idxNum);
		ErrorMessage(FUNC_NAME, !LoadData(fp, &mesh.mesh->indices[0], sizeof(unsigned int), idxNum), "インデックス情報" + MSG_TAIL);

		//マテリアル取得
		auto& material = mesh.material;
		//マテリアル名サイズ
		ErrorMessage(FUNC_NAME, !LoadData(fp, &size, sizeof(size), 1), "マテリアル名サイズ" + MSG_TAIL);
		//マテリアル名
		material->name.resize(size);
		ErrorMessage(FUNC_NAME, !LoadData(fp, &material->name[0], size, 1), "マテリアル名" + MSG_TAIL);
		//マテリアル情報
		ErrorMessage(FUNC_NAME, !LoadData(fp, &material->constData, sizeof(Material::ConstData), 1), "マテリアル情報" + MSG_TAIL);

		//画像ファイルテクスチャ保持フラグ
		unsigned char haveTex[MATERIAL_TEX_TYPE_NUM];
		ErrorMessage(FUNC_NAME, !LoadData(fp, haveTex, sizeof(unsigned char), MATERIAL_TEX_TYPE_NUM), "画像ファイルテクスチャ保持フラグ" + MSG_TAIL);
		//画像ファイルテクスチャパス情報
		for (int texIdx = 0; texIdx < MATERIAL_TEX_TYPE_NUM; ++texIdx)
		{
			if (!haveTex[texIdx])continue;

			//ファイルパスサイズ
			ErrorMessage(FUNC_NAME, !LoadData(fp, &size, sizeof(size), 1), "ファイルパスサイズ" + MSG_TAIL);
			//ファイルパス
			material->textures[texIdx].path.resize(size);
			ErrorMessage(FUNC_NAME, !LoadData(fp, &material->textures[texIdx].path[0], size, 1), "画像ファイルパス" + MSG_TAIL);
		}

		mesh.material->CreateBuff();
		mesh.mesh->CreateBuff();
	}

	//スケルトン取得
	auto& skel = model->skelton;
	//ボーンの数
	unsigned short boneNum;
	ErrorMessage(FUNC_NAME, !LoadData(fp, &boneNum, sizeof(boneNum), 1), "ボーン数" + MSG_TAIL);
	skel.bones.resize(boneNum);

	//ボーン情報
	for (int boneIdx = 0; boneIdx < boneNum; ++boneIdx)
	{
		//ボーン取得
		auto& bone = skel.bones[boneIdx];
		//ボーン名サイズ
		ErrorMessage(FUNC_NAME, !LoadData(fp, &size, sizeof(size), 1), "ボーン名サイズ" + MSG_TAIL);
		//ボーン名
		bone.name.resize(size);
		ErrorMessage(FUNC_NAME, !LoadData(fp, &bone.name[0], size, 1), "ボーン名" + MSG_TAIL);

		//名前以外のボーン情報
		ErrorMessage(FUNC_NAME, !LoadData(fp, &bone.parent, Bone::GetSizeWithOutName(), 1), "ボーン情報" + MSG_TAIL);
	}
	skel.CreateBoneTree();

	fclose(fp);

	RegisterImportModel(Dir, FileName, model);

	return model;
}

void Importer::SaveHSMModel(const std::string& FileNameTail, std::shared_ptr<Model> Model, const FILETIME& LastWriteTime)
{
	static const std::string FUNC_NAME = "SaveHSMModel";
	static const std::string MSG_TAIL = "の書き込みに失敗\n";

	FILE* fp;
	fopen_s(&fp, (Model->header.dir + Model->header.GetModelName() + FileNameTail + ".hsm").c_str(), "wb");
	ErrorMessage(FUNC_NAME, fp == nullptr, ".hsmファイルのバイナリ書き込みモードでのオープンに失敗\n");

	//最終更新日時
	ErrorMessage(FUNC_NAME, !SaveData(fp, &LastWriteTime, sizeof(LastWriteTime), 1), "最終更新日時" + MSG_TAIL);

	size_t size;

	//メッシュの数
	unsigned short meshNum = Model->meshes.size();
	ErrorMessage(FUNC_NAME, !SaveData(fp, &meshNum, sizeof(meshNum), 1), "メッシュの数" + MSG_TAIL);

	//メッシュ情報
	for (int meshIdx = 0; meshIdx < meshNum; ++meshIdx)
	{
		//メッシュ取得
		auto& mesh = Model->meshes[meshIdx];
		//メッシュ名サイズ
		size = mesh.mesh->name.length();
		ErrorMessage(FUNC_NAME, !SaveData(fp, &size, sizeof(size), 1), "メッシュ名サイズ" + MSG_TAIL);
		//メッシュ名
		ErrorMessage(FUNC_NAME, !SaveData(fp, mesh.mesh->name.data(), size, 1), "メッシュ名" + MSG_TAIL);

		//頂点数
		unsigned short vertNum = mesh.mesh->vertices.size();
		ErrorMessage(FUNC_NAME, !SaveData(fp, &vertNum, sizeof(vertNum), 1), "頂点数" + MSG_TAIL);
		//頂点情報
		ErrorMessage(FUNC_NAME, !SaveData(fp, mesh.mesh->vertices.data(), sizeof(ModelMesh::Vertex), vertNum), "頂点情報" + MSG_TAIL);

		//インデックス数
		unsigned short idxNum = mesh.mesh->indices.size();
		ErrorMessage(FUNC_NAME, !SaveData(fp, &idxNum, sizeof(idxNum), 1), "インデックス数" + MSG_TAIL);
		//インデックス情報
		ErrorMessage(FUNC_NAME, !SaveData(fp, mesh.mesh->indices.data(), sizeof(unsigned int), idxNum), "インデックス情報" + MSG_TAIL);

		//マテリアル取得
		auto& material = mesh.material;
		//マテリアル名サイズ
		size = material->name.length();
		ErrorMessage(FUNC_NAME, !SaveData(fp, &size, sizeof(size), 1), "マテリアル名サイズ" + MSG_TAIL);
		//マテリアル名
		ErrorMessage(FUNC_NAME, !SaveData(fp, material->name.data(), size, 1), "マテリアル名" + MSG_TAIL);
		//マテリアル情報
		ErrorMessage(FUNC_NAME, !SaveData(fp, &material->constData, sizeof(Material::ConstData), 1), "マテリアル情報" + MSG_TAIL);

		//画像ファイルテクスチャ保持フラグ
		unsigned char haveTex[MATERIAL_TEX_TYPE_NUM] = { 0 };
		for (int texIdx = 0; texIdx < MATERIAL_TEX_TYPE_NUM; ++texIdx)
		{
			if (material->textures[texIdx].path.empty())continue;

			//テクスチャが割り当てられている
			haveTex[texIdx] = 1;
		}
		ErrorMessage(FUNC_NAME, !SaveData(fp, haveTex, sizeof(unsigned char), MATERIAL_TEX_TYPE_NUM), "マテリアルのテクスチャ保持フラグ" + MSG_TAIL);

		//画像ファイルテクスチャパス情報
		for(int texIdx = 0;texIdx < MATERIAL_TEX_TYPE_NUM;++texIdx)
		{
			if (!haveTex[texIdx])continue;

			//画像ファイルパスサイズ
			size = material->textures[texIdx].path.length();
			ErrorMessage(FUNC_NAME, !SaveData(fp, &size, sizeof(size), 1), "テクスチャ画像ファイルパスサイズ" + MSG_TAIL);
			ErrorMessage(FUNC_NAME, !SaveData(fp, material->textures[texIdx].path.data(), size, 1), "テクスチャ画像ファイルパス" + MSG_TAIL);
		}
	}

	//スケルトン取得
	auto& skel = Model->skelton;
	//ボーンの数
	unsigned short boneNum = skel.bones.size();
	ErrorMessage(FUNC_NAME, !SaveData(fp, &boneNum, sizeof(boneNum), 1), "ボーン数" + MSG_TAIL);
	//ボーン情報
	for (int boneIdx = 0; boneIdx < boneNum; ++boneIdx)
	{
		//ボーン取得
		auto& bone = skel.bones[boneIdx];
		//ボーン名サイズ
		size = bone.name.length();
		ErrorMessage(FUNC_NAME, !SaveData(fp, &size, sizeof(size), 1), "ボーン名サイズ" + MSG_TAIL);
		//ボーン名
		ErrorMessage(FUNC_NAME, !SaveData(fp, bone.name.data(), size, 1), "ボーン名" + MSG_TAIL);
		//ボーン情報
		ErrorMessage(FUNC_NAME, !SaveData(fp, &bone.parent, Bone::GetSizeWithOutName(), 1), "ボーン情報" + MSG_TAIL);
	}

	fclose(fp);
}
*/

Importer::Importer()
{
}

void Importer::LoadGLTFMaterial(const MATERIAL_TEX_TYPE& Type, std::weak_ptr<Material> AttachMaterial, const Microsoft::glTF::Image& Img, const std::string& Dir, const Microsoft::glTF::GLTFResourceReader& Reader, const Microsoft::glTF::Document& Doc)
{
	auto material = AttachMaterial.lock();
	//テクスチャ画像ファイル読み込み
	if (!Img.uri.empty())
	{
		std::string path = Dir + Img.uri;
		material->texBuff[Type] = D3D12App::Instance()->GenerateTextureBuffer(path);
	}
	//テクスチャ画像がgltfに埋め込まれている
	else if (!Img.bufferViewId.empty())
	{
		auto imageBufferView = Doc.bufferViews.Get(Img.bufferViewId);
		auto imageData = Reader.ReadBinaryData<char>(Doc, imageBufferView);
		std::string path = "glTF - Load (" + Img.mimeType + ") - " + Img.name;
		material->texBuff[Type] = D3D12App::Instance()->GenerateTextureBuffer(imageData);
	}
}

#include<sstream>
std::shared_ptr<Model> Importer::LoadGLTFModel(const std::string& Dir, const std::string& FileName, const std::string& Ext)
{
	printf("glTFロード\nDir : %s , FileName : %s\n", Dir.c_str(), FileName.c_str());
	std::shared_ptr<Model>result = std::make_shared<Model>(Dir, FileName);

	auto modelFilePath = std::experimental::filesystem::path(Dir + FileName);
	if (modelFilePath.is_relative())
	{
		auto current = std::experimental::filesystem::current_path();
		current /= modelFilePath;
		current.swap(modelFilePath);
	}
	auto reader = std::make_unique<StreamReader>(modelFilePath.parent_path());
	auto stream = reader->GetInputStream(modelFilePath.filename().u8string());

	std::string manifest;

	auto MakePathExt = [](const std::string& ext)
	{
		return "." + ext;
	};

	std::unique_ptr<Microsoft::glTF::GLTFResourceReader> resourceReader;

	if (Ext == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
	{
		auto gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(reader));

		std::stringstream manifestStream;

		// Read the contents of the glTF file into a string using a std::stringstream
		manifestStream << stream->rdbuf();
		manifest = manifestStream.str();

		resourceReader = std::move(gltfResourceReader);
	}
	else if (Ext == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
	{
		auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(reader), std::move(stream));

		manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

		resourceReader = std::move(glbResourceReader);
	}

	assert(resourceReader);

	Microsoft::glTF::Document doc;
	try
	{
		doc = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException& ex)
	{
		std::stringstream ss;
		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();
		throw std::runtime_error(ss.str());
	}

	PrintDocumentInfo(doc);
	PrintResourceInfo(doc, *resourceReader);

	Skeleton skel;

	//ノード読み込み
	std::vector<Microsoft::glTF::Node>skinNodes;	//後に使うためスキンノードを格納する配列
	for (const auto& gltfNode : doc.nodes.Elements())
	{
		// ローカル変形行列の計算
		XMVECTOR rotation = { gltfNode.rotation.x, gltfNode.rotation.y, gltfNode.rotation.z, gltfNode.rotation.w };
		XMVECTOR scaling = { gltfNode.scale.x, gltfNode.scale.y, gltfNode.scale.z, 1.0f };
		XMVECTOR translation = { gltfNode.translation.x, gltfNode.translation.y, gltfNode.translation.z, 1.0f };

		XMMATRIX matScaling, matRotation, matTranslation;
		matScaling = XMMatrixScalingFromVector(scaling);
		matRotation = XMMatrixRotationQuaternion(rotation);
		matTranslation = XMMatrixTranslationFromVector(translation);

		auto nodeTransform = XMMatrixIdentity();
		nodeTransform *= matScaling * matRotation * matTranslation;

		//スキン情報
		if (!gltfNode.skinId.empty())
		{
			skinNodes.emplace_back(gltfNode);
			continue;
		}

		//ボーン → スキンの順番、その後に来るデータはアーマチュア？の情報でいらない情報ぽい
		if (!skinNodes.empty())break;

		//ボーン情報
		skel.bones.emplace_back();
		auto& bone = skel.bones.back();
		bone.name = gltfNode.name;
		for (auto& child : gltfNode.children)
		{
			auto childIdx = doc.nodes.GetIndex(child);
			skel.bones[childIdx].parent = static_cast<char>(skel.bones.size() - 1);
		}

		bone.invBindMat = XMMatrixInverse(nullptr, nodeTransform);

	}

	//スキン読み込み
	for (const auto& gltfSkin : doc.skins.Elements())
	{
		auto invMatAcc = doc.accessors.Get(gltfSkin.inverseBindMatricesAccessorId);

		auto data = resourceReader->ReadFloatData(doc, invMatAcc);
		for (int matIdx = 0; matIdx < invMatAcc.count; ++matIdx)
		{
			int offset = matIdx * 16;
			Matrix invBindMat = XMMatrixSet(
				data[offset], data[offset + 1], data[offset + 2], data[offset + 3],
				data[offset + 4], data[offset + 5], data[offset + 6], data[offset + 7],
				data[offset + 8], data[offset + 9], data[offset + 10], data[offset + 11],
				data[offset + 12], data[offset + 13], data[offset + 14], data[offset + 15]);

			int boneIdx = std::atoi(gltfSkin.jointIds[matIdx].c_str());
			skel.bones[boneIdx].invBindMat = invBindMat/* * skel.bones[boneIdx].invBindMat*/;
		}
	}

	//アニメーション
	for (const auto& gltfAnimNode : doc.animations.Elements())
	{
		std::string animName = gltfAnimNode.name;
		auto& modelAnim = skel.animations[animName];

		//アニメーションの詳細な情報を持つサンプラー（キーフレーム時刻リスト、補間形式、キーフレームに対応するアニメーションデータ）
		auto animSamplers = gltfAnimNode.samplers.Elements();

		//ボーン、サンプラー、パス（translation,rotation,scale）が割り当てられている
		for (const auto& animChannel : gltfAnimNode.channels.Elements())
		{
			const auto& sampler = animSamplers[gltfAnimNode.samplers.GetIndex(animChannel.samplerId)];
			const auto& boneNode = doc.nodes.Get(animChannel.target.nodeId);	//対応するボーンノード取得
			auto& boneAnim = modelAnim.boneAnim[boneNode.name];	//ボーン単位のアニメーション

			//アニメーション情報のターゲット（POS、ROTATE、SCALE）
			auto& path = animChannel.target.path;

			//補間方法（現状、LINERにしか対応しないので特に情報を格納しない）
			auto interpolation = sampler.interpolation;

			//開始 / 終了フレーム
			const auto& input = doc.accessors.Get(sampler.inputAccessorId);
			const int startFrame = static_cast<int>(input.min[0] * 60);	//単位が秒なので 60f / 1sec としてフレームに変換
			const int endFrame = static_cast<int>(input.max[0] * 60);	//単位が秒なので 60f / 1sec としてフレームに変換

			//キーフレーム情報
			auto keyFrames = resourceReader->ReadBinaryData<float>(doc, input);
			for (auto& keyFrame : keyFrames)keyFrame *= 60;	//単位が秒なので 60f / 1sec としてフレームに変換

			//具体的な値
			const auto& output = doc.accessors.Get(sampler.outputAccessorId);
			auto values = resourceReader->ReadBinaryData<float>(doc, output);

			ErrorMessage("LoadGLTFModel",
				path == Microsoft::glTF::TARGET_WEIGHTS || path == Microsoft::glTF::TARGET_UNKNOWN, "This anim's target path is unsupported.");

			//Rotation(Quaternion)
			if (path == Microsoft::glTF::TARGET_ROTATION)
			{
				boneAnim.rotateAnim.startFrame = startFrame;
				boneAnim.rotateAnim.endFrame = endFrame;

				for (int keyFrameIdx = 0; keyFrameIdx < keyFrames.size(); ++keyFrameIdx)
				{
					boneAnim.rotateAnim.keyFrames.emplace_back();
					auto& keyFrame = boneAnim.rotateAnim.keyFrames.back();
					keyFrame.frame = static_cast<int>(keyFrames[keyFrameIdx]);
					int offset = keyFrameIdx * 4;	//インデックスのオフセット
					keyFrame.value = XMVectorSet(values[offset], values[offset + 1], values[offset + 2], values[offset + 3]);
				}
			}
			//TranslationかScale
			else
			{
				Animation<Vec3<float>>* animPtr = &boneAnim.posAnim;
				if (path == Microsoft::glTF::TARGET_SCALE) { animPtr = &boneAnim.scaleAnim; }

				animPtr->startFrame = startFrame;
				animPtr->endFrame = endFrame;

				for (int keyFrameIdx = 0; keyFrameIdx < keyFrames.size(); ++keyFrameIdx)
				{
					animPtr->keyFrames.emplace_back();
					auto& keyFrame = animPtr->keyFrames.back();
					keyFrame.frame = static_cast<int>(keyFrames[keyFrameIdx]);
					int offset = keyFrameIdx * 3;	//インデックスのオフセット
					keyFrame.value = { values[offset], values[offset + 1], values[offset + 2] };
				}
			}
		}
	}

	skel.CreateBoneTree();
	result->m_skelton = std::make_shared<Skeleton>(skel);

	//マテリアル読み込み
	std::vector<std::shared_ptr<Material>>loadMaterials;
	for (auto& m : doc.materials.Elements())
	{
		auto material = std::make_shared<Material>();
		material->name = m.name;

		//PBR
		const auto baseColor = m.metallicRoughness.baseColorFactor;
		material->constData.pbr.baseColor.x = baseColor.r;
		material->constData.pbr.baseColor.y = baseColor.g;
		material->constData.pbr.baseColor.z = baseColor.b;
		material->constData.pbr.metalness = m.metallicRoughness.metallicFactor;
		material->constData.pbr.roughness = m.metallicRoughness.roughnessFactor;

		//カラーテクスチャ
		auto textureId = m.metallicRoughness.baseColorTexture.textureId;
		if (!textureId.empty())
		{
			auto& texture = doc.textures.Get(textureId);
			auto& image = doc.images.Get(texture.imageId);
			LoadGLTFMaterial(COLOR_TEX, material, image, Dir, *resourceReader, doc);
			material->constData.pbr.baseColor = { 0,0,0 };
		}

		//エミッシブ
		textureId = m.emissiveTexture.textureId;
		if (!textureId.empty())
		{
			auto& texture = doc.textures.Get(textureId);
			auto& image = doc.images.Get(texture.imageId);
			LoadGLTFMaterial(EMISSIVE_TEX, material, image, Dir, *resourceReader, doc);
		}
		else
		{
			material->constData.lambert.emissive.x = m.emissiveFactor.r;
			material->constData.lambert.emissive.y = m.emissiveFactor.g;
			material->constData.lambert.emissive.z = m.emissiveFactor.b;
		}

		//ノーマルマップ
		textureId = m.normalTexture.textureId;
		if (!textureId.empty())
		{
			auto& texture = doc.textures.Get(textureId);
			auto& image = doc.images.Get(texture.imageId);
			LoadGLTFMaterial(NORMAL_TEX, material, image, Dir, *resourceReader, doc);
		}
		//ラフネス
		textureId = m.metallicRoughness.metallicRoughnessTexture.textureId;
		if (!textureId.empty())
		{
			auto& texture = doc.textures.Get(textureId);
			auto& image = doc.images.Get(texture.imageId);
			LoadGLTFMaterial(ROUGHNESS_TEX, material, image, Dir, *resourceReader, doc);
		}

		material->CreateBuff();
		loadMaterials.emplace_back(material);
	}

	for (const auto& glTFMesh : doc.meshes.Elements())
	{
		//メッシュIDを比較して対応するスキンノードを取得
		const Microsoft::glTF::Skin* gltfSkin = nullptr;
		const auto& meshId = glTFMesh.id;
		const auto& skinNode = std::find_if(skinNodes.begin(), skinNodes.end(), [meshId](Microsoft::glTF::Node& node) 
			{
				return node.meshId == meshId;
			});
		if (skinNode != skinNodes.end())gltfSkin = &doc.skins.Get(skinNode->skinId);

		const auto& meshName = glTFMesh.name;	//メッシュ名取得
		int primitiveNum = 0;	//プリミティブの数記録用

		//マテリアル単位で分けられている？
		for (const auto& gltfMeshPrimitive : glTFMesh.primitives)
		{
			//モデル用メッシュ生成
			ModelMesh mesh;
			mesh.mesh = std::make_shared<Mesh<ModelMesh::Vertex>>();
			mesh.mesh->name = meshName + " - " + std::to_string(primitiveNum++);

			//頂点 & インデックス情報
			LoadGLTFPrimitive(mesh, gltfMeshPrimitive, gltfSkin, *resourceReader, doc);

			if (doc.materials.Has(gltfMeshPrimitive.materialId))
			{
				int materialIdx = int(doc.materials.GetIndex(gltfMeshPrimitive.materialId));
				mesh.material = loadMaterials[materialIdx];
			}
			else
			{
				mesh.material = std::make_shared<Material>();
			}

			mesh.mesh->CreateBuff();
			result->m_meshes.emplace_back(mesh);
		}
	}

	RegisterImportModel(Dir, FileName, result);

	return result;
}

std::shared_ptr<Model> Importer::LoadModel(const std::string& Dir, const std::string& FileName)
{
	//ファイルが存在しているか確認
	ErrorMessage("LoadModel", !KuroFunc::ExistFile(Dir + FileName), Dir + FileName + " wasn't found.\n");

	//既にロード済ならそのポインタを返す
	auto result = m_models.find(Dir + FileName);
	if (result != m_models.end())return m_models[Dir + FileName];

	//拡張子取得
	const auto ext = "." + KuroFunc::GetExtension(FileName);

	if (ext == ".glb" || ext == ".gltf")
	{
		return LoadGLTFModel(Dir, FileName, ext);
	}
	else
	{
		ErrorMessage("LoadModel", true, "This format is valid.");
	}
	return std::shared_ptr<Model>();
}