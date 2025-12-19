//-------------------------------------------------------------
//! @file   Container.hpp
//! @brief  依存性注入コンテナライブラリのコンテナ定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
#include <memory>        // std::shared_ptr
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector
#include <functional>    // std::function
#include <typeindex>     // std::type_index
#include <shared_mutex>  // std::shared_mutex
#include "ResolveException.hpp" // 独自例外
#include "ScopedContext.hpp"    // ScopedContext クラス
#include "Lifecycle.hpp"        // ライフサイクル enum
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer {
	//-------------------------------------------------------------
	//! @class   Container
	//! @brief   依存性注入コンテナクラス
	//! @details 型登録、解決、スコープ管理を提供
	//-------------------------------------------------------------
	class Container {
		friend class ScopedContext; // ScopedContext に内部アクセスを許可
	public:
		//-------------------------------------------------------------
		// 型登録
		//! @tparam  TInterface			 抽象型インターフェース
		//! @tparam  TImplementation	 具象型実装
		//! @param   cycle_  [in]        ライフサイクル管理方法（デフォルトはTransient）
		//! @throws	 ResolveException 既に登録済みの場合
		//! @details 登録済みの場合は例外を投げる。上書きしたい場合はReplaceを使用してください。
		//-------------------------------------------------------------
		template<typename TInterface, typename TImplementation>
		void registerType(Lifecycle cycle_ = Lifecycle::Transient);

		//-------------------------------------------------------------
		// 型登録上書き
		//! @tparam  TInterface			 抽象型インターフェース
		//! @tparam  TImplementation	 具象型実装
		//! @param   cycle_  [in]        ライフサイクル管理方法（デフォルトはTransient）
		//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
		//-------------------------------------------------------------
		template<typename TInterface, typename TImplementation>
		void replaceType(Lifecycle cycle_ = Lifecycle::Transient);

		//-------------------------------------------------------------
		// コンストラクタ呼び出し補助
		//! @tparam TImplementation 具象型実装
		//! @tparam TDeps         依存型リスト
		//! @param  args          [in] 依存インスタンスリスト
		//! @param  I             インデックスシーケンス
		//! @return 生成されたインスタンス
		//! @details 依存インスタンスを静的ポインタキャストしてコンストラクタに渡す
		//-------------------------------------------------------------
		template<typename TInterface, typename TImplementation, typename... TDeps>
		void registerCtor(Lifecycle cycle);

		//-------------------------------------------------------------
		// コンストラクタ呼び出し補助上書き
		//! @tparam TImplementation 具象型実装
		//! @tparam TDeps         依存型リスト
		//! @param  args          [in] 依存インスタンスリスト
		//! @return 生成されたインスタンス
		//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
		//-------------------------------------------------------------
		template<typename TInterface, typename TImplementation, typename... TDeps>
		void replaceCtor(Lifecycle cycle_ = Lifecycle::Transient);

		//-------------------------------------------------------------
		// インスタンス登録
		//! @tparam TInterface 抽象型インターフェース
		//! @param  instance   [in] 具象型インスタンス
		//! @throws	 ResolveException 既に登録済みの場合
		//! @details シングルトンとして登録される
		//-------------------------------------------------------------
		template<typename TInterface>
		void registerInstance(std::shared_ptr<TInterface> instance);

		//-------------------------------------------------------------
		// インスタンス登録上書き
		//! @tparam TInterface 抽象型インターフェース
		//! @param  instance   [in] 具象型インスタンス
		//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
		//-------------------------------------------------------------
		template<typename TInterface>
		void replaceInstance(std::shared_ptr<TInterface> instance);

		//-------------------------------------------------------------
		// 型解決（循環依存検出付き）
		//! @tparam TInterface 抽象型インターフェース
		//! @return 解決されたインスタンス
		//! @throws ResolveException 未登録型、循環依存、未知ライフサイクルの場合
		//-------------------------------------------------------------
		template<typename TInterface>
		std::shared_ptr<TInterface> resolve();

		//-------------------------------------------------------------
		// 型が登録済みか確認
		//! @tparam TInterface 抽象型インターフェース
		//! @return 登録済みなら true、未登録なら false
		//-------------------------------------------------------------
		template<typename TInterface>
		bool isRegistered() const;

		//-------------------------------------------------------------
		// キーによる型解決（循環依存検出付き）
		//! @param  key  [in] 型のキー（type_index）
		//! @return  解決されたインスタンス
		//! @throws ResolveException 未登録型、循環依存、未知ライフサイクルの場合
		//-------------------------------------------------------------
		std::shared_ptr<void> resolveByKey(const std::type_index& key);

		//-------------------------------------------------------------
		// ScopedContext を生成
		//! @return ScopedContext RAII スコープオブジェクト
		//-------------------------------------------------------------
		ScopedContext createScope();

	private:

		//---------------------------------------------------------
		//! @struct Registration
		//! @brief  登録情報構造体
		//! @details ライフサイクルとファクトリ関数を保持
		//---------------------------------------------------------
		struct Registration {
			Lifecycle cycle_ = Lifecycle::Transient;												// ライフサイクル(デフォルトはTransient)
			std::vector<std::type_index> deps_;														// 依存型リスト（空なら引数なし）
			std::function<std::shared_ptr<void>(const std::vector<std::shared_ptr<void>>&)> ctor_;	// ファクトリ関数
		};

		std::unordered_map<std::type_index, Registration> registrations_;				// 登録情報マップ
		std::unordered_map<std::type_index, std::shared_ptr<void>> singletons_;			// シングルトンインスタンスマップ
		mutable std::shared_mutex mutex_;												// スレッドセーフ用ミューテックス
	};
}// namespace TsukinoDIContainer
namespace TsukinoDIContainer {
	// テンプレート関数の実装
	template<typename TInterface, typename TImplementation>
	void Container::registerType(Lifecycle cycle_) {
		std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
		// 型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		// 既に登録済みか確認
		if (registrations_.find(type) != registrations_.end()) {
			throw ResolveException("Type already registered: " + std::string(type.name()));
		}
		// 登録処理
		registerCtor<TInterface, TImplementation>(cycle_);
	}
}