//-------------------------------------------------------------
//! @file   ResolveException.hpp
//! @brief  依存性注入コンテナライブラリの例外定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
#include <stdexcept> 
#include <string>
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer {
	//-------------------------------------------------------------
	//! @class  ResolveException
	//! @brief  解決エラー例外クラス
	//-------------------------------------------------------------
	class ResolveException : public std::runtime_error {
	public:
		//-------------------------------------------------------------
		// コンストラクタ
		//! @param  msg エラーメッセージ
		//-------------------------------------------------------------
		explicit ResolveException(const std::string& msg);
	};
}// namespace TsukinoDIContainer