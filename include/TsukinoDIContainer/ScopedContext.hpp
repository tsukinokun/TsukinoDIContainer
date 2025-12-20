//-------------------------------------------------------------
//! @file   TsukinoDIContainer.hpp
//! @brief  依存性注入コンテナライブラリのスコープコンテキスト定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <shared_mutex>

namespace TsukinoDIContainer {
	class Container; // 前方宣言
	//-------------------------------------------------------------
	//! @class ScopedContext
	//! @brief Scoped ライフタイム管理用 RAII クラス
	//-------------------------------------------------------------
	class ScopedContext {
	public:
		//-------------------------------------------------------------
		// コンストラクタ
		//! @param  container  [in] 所属するコンテナ
		//-------------------------------------------------------------
		explicit ScopedContext(Container& container);

		//-------------------------------------------------------------
		// デストラクタ
		//-------------------------------------------------------------
		~ScopedContext();

		//-------------------------------------------------------------
		// コピー禁止
		//-------------------------------------------------------------
		ScopedContext(const ScopedContext&) = delete;

		//-------------------------------------------------------------
		// コピー禁止
		//-------------------------------------------------------------
		ScopedContext& operator=(const ScopedContext&) = delete;

		//-------------------------------------------------------------
		// ムーブコンストラクタ
		//! @param	 other  [in] 移動元オブジェクト
		//! @details ムーブ後、移動元のデストラクタではスコープ終了処理を行わない
		//! @details ムーブ元のオブジェクトを使用しないで下さい
		//-------------------------------------------------------------
		ScopedContext(ScopedContext&& other) noexcept;

		//-------------------------------------------------------------
		// 依存関係の解決
		//! @tparam  TInterface 抽象型インターフェース
		//! @return  解決されたインスタンス
		//! @throws	 ResolveException 未登録型、循環依存、未知ライフサイクルの場合
		//! @details コンテナの Resolve を呼び出す
		//-------------------------------------------------------------
		template<typename TInterface>
		std::shared_ptr<TInterface> resolve();

	private:
		Container& container_;	// 所属するコンテナへの参照
		std::unordered_map<std::type_index, std::shared_ptr<void>> scoped_instances_;	// スコープインスタンスマップ
		bool active_;			// スコープが有効かどうかのフラグ
	};
}