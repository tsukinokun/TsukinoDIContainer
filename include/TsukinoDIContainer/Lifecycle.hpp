//-------------------------------------------------------------
//! @file   Lifecycle.hpp
//! @brief  依存性注入コンテナライブラリのライフサイクル定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer
{
	//-------------------------------------------------------------
	//! @enum   Lifecycle
	//! @brief  インスタンスのライフサイクル管理方法
	//-------------------------------------------------------------
	enum class Lifecycle {
		Transient,  //!< 毎回新しいインスタンス
		Singleton,  //!< 全体で共有
		Scoped      //!< スコープごとに共有
	};
}