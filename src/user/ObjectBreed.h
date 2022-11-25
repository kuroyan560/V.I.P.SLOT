#pragma once
#include<memory>
#include<vector>
#include"Collider.h"
#include"BasicDrawParameters.h"
class Model;
class ObjectController;

class ObjectBreed
{
public:
	//種別番号（種類ごと）
	const int m_typeID;
	//モデル
	const std::shared_ptr<Model>m_model;
	//最大HP
	const int m_maxHp;
	//所持コイン（倒したら得られるコインの数）
	const int m_killCoinNum;
	//挙動制御
	const std::unique_ptr<ObjectController>m_controller;
	//コライダー（クローン元）
	const std::vector<std::unique_ptr<Collider>>m_originCollider;
	//描画パラメータ
	const IndividualDrawParameter m_drawParam;

	/// <summary>
	/// 敵の血統コンストラクタ
	/// </summary>
	/// <param name="arg_typeID">種別番号（敵の種類ごと）</param>
	/// <param name="arg_model">モデルデータ</param>
	/// <param name="arg_maxHp">最大HP</param>
	/// <param name="arg_killCoinNum">倒すことで得られるコイン数</param>
	/// <param name="arg_controller">挙動制御</param>
	/// <param name="arg_originCollider">クローン元コライダー</param>
	ObjectBreed(
		int arg_typeID,
		std::shared_ptr<Model>arg_model,
		int arg_maxHp,
		int arg_killCoinNum,
		std::unique_ptr<ObjectController>arg_controller, 
		std::vector<std::unique_ptr<Collider>>& arg_originCollider,
		IndividualDrawParameter arg_drawParam = IndividualDrawParameter::GetDefault())
		:m_typeID(arg_typeID),
		m_model(arg_model),
		m_maxHp(arg_maxHp),
		m_killCoinNum(arg_killCoinNum),
		m_controller(std::move(arg_controller)),
		m_originCollider(std::move(arg_originCollider)),
		m_drawParam(arg_drawParam) {}
};