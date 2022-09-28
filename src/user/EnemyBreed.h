#pragma once
#include<memory>
class Model;
class EnemyController;

class EnemyBreed
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
	const std::unique_ptr<EnemyController>m_controller;

	EnemyBreed(
		int arg_typeID,
		std::shared_ptr<Model>arg_model,
		int arg_maxHp,
		int arg_initCoinNum,
		std::unique_ptr<EnemyController>arg_controller)
		:m_typeID(arg_typeID),
		m_model(arg_model),
		m_maxHp(arg_maxHp),
		m_initCoinNum(arg_initCoinNum),
		m_controller(std::move(arg_controller)) {}
};