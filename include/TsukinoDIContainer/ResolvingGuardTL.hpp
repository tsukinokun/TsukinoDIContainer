//-------------------------------------------------------------
//! @file   ResolvingGuardTL.hpp
//! @brief  依存性注入コンテナライブラリの循環依存検出用ガード定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
#include <typeindex>
namespace TsukinoDIContainer {
	//---------------------------------------------------------
	//! @struct  ResolvingGuardTL
	//! @brief   循環依存検出用ガードクラス（スレッドローカル）
	//! @details コンストラクタでスタックに型をプッシュ、デストラクタでポップ
	//---------------------------------------------------------
	struct ResolvingGuardTL {
		std::type_index type_;	// ガード対象の型
		bool active_{ false };	// ガードが有効かどうかのフラグ

		//---------------------------------------------------------
		//! @brief  コンストラクタ
		//! @param  t  [in] ガード対象の型
		//! @throws ResolveException 循環依存が検出された場合
		//---------------------------------------------------------
		explicit ResolvingGuardTL(std::type_index t);

		//---------------------------------------------------------
		//! @brief  デストラクタ
		//! @details スタックから型をポップ
		//---------------------------------------------------------
		~ResolvingGuardTL();
	};
} // namespace TsukinoDIContainer