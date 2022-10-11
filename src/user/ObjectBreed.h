#pragma once
#include<memory>
#include<vector>
#include"Collider.h"
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
	const int m_initCoinNum;
	//挙動制御
	const std::unique_ptr<ObjectController>m_controller;
	//コライダー（クローン元）
	const std::vector<std::unique_ptr<Collider>>m_originCollider;

	/// <summary>
	/// 敵の血統コンストラクタ
	/// </summary>
	/// <param name="arg_typeID">種別番号（敵の種類ごと）</param>
	/// <param name="arg_model">モデルデータ</param>
	/// <param name="arg_maxHp">最大HP</param>
	/// <param name="arg_initCoinNum">生成時の所持コイン数</param>
	/// <param name="arg_controller">挙動制御</param>
	/// <param name="arg_originCollider">クローン元コライダー</param>
	ObjectBreed(
		int arg_typeID,
		std::shared_ptr<Model>arg_model,
		int arg_maxHp,
		int arg_initCoinNum,
		std::unique_ptr<ObjectController>arg_controller, 
		std::vector<std::unique_ptr<Collider>>& arg_originCollider)
		:m_typeID(arg_typeID),
		m_model(arg_model),
		m_maxHp(arg_maxHp),
		m_initCoinNum(arg_initCoinNum),
		m_controller(std::move(arg_controller)),
		m_originCollider(std::move(arg_originCollider)) {}
};