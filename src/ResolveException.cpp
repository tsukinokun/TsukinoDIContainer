//-------------------------------------------------------------
//! @file   ResolveException.cpp
//! @brief  依存性注入コンテナライブラリの例外実装
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include "TsukinoDIContainer/ResolveException.hpp"
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer
{
	//-------------------------------------------------------------
	//! @brief  コンストラクタ
	//-------------------------------------------------------------
	ResolveException::ResolveException(const std::string& msg)
		: std::runtime_error("TsukinoDIContainer Resolve Error: " + msg) {
	}
}