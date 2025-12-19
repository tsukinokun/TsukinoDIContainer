//-------------------------------------------------------------
//! @file   Container.cpp
//! @brief  依存性注入コンテナライブラリのコンテナ実装
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include "TsukinoDIContainer/Container.hpp"
#include "TsukinoDIContainer/ResolveException.hpp"
#include "TsukinoDIContainer/ScopedContext.hpp"
#include "TsukinoDIContainer/Lifecycle.hpp"
#include "TsukinoDIContainer/ResolvingGuardTL.hpp"
#include "TsukinoDIContainer/ResolvingStack.hpp"
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer
{
	//-------------------------------------------------------------
	//! @brief 型登録
	//-------------------------------------------------------------
	template<typename TInterface, typename TImplementation>
	void Container::registerType(Lifecycle cycle_) {
		std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
		// 登録済み確認
		const auto type = std::type_index(typeid(TInterface));
		if (registrations_.find(type) != registrations_.end()) {
			// 既に登録済みなら例外、上書きしたい場合はReplaceを使用させる。
			throw ResolveException("Type already registered: " + std::string(type.name()));
		}
		// 登録処理
		registrations_[type] = {
			cycle_,
			{}, // 依存なし
			[](const std::vector<std::shared_ptr<void>>&) {
				return std::make_shared<TImplementation>();
			}
		};
	}

	//-------------------------------------------------------------
	//! @brief 型登録上書き
	//-------------------------------------------------------------
	template<typename TInterface, typename TImplementation>
	void Container::replaceType(Lifecycle cycle_) {
		std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
		//型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		// 登録処理
		registrations_[type] = {
			cycle_,
			{}, // 依存なし
			[](const std::vector<std::shared_ptr<void>>&) {
				return std::make_shared<TImplementation>();
			}
		};
		// 旧 Singleton を必ず破棄（次回解決で新規生成） 
		singletons_.erase(type);
	}

	//-------------------------------------------------------------
	//! @brief コンストラクタ呼び出し補助
	//! @tparam TImplementation 具象型実装
	//! @tparam TDeps         依存型リスト
	//! @param  args          [in] 依存インスタンスリスト
	//! @param  I             インデックスシーケンス
	//! @return 生成されたインスタンス
	//-------------------------------------------------------------
	template<typename TImplementation, typename... TDeps, std::size_t... I>
	static std::shared_ptr<void> callCtorImpl(
		const std::vector<std::shared_ptr<void>>& args,
		std::index_sequence<I...>)
	{
		// 依存インスタンスを静的ポインタキャストしてコンストラクタに渡す
		return std::make_shared<TImplementation>(
			std::static_pointer_cast<TDeps>(args[I])...
		);
	}

	//-------------------------------------------------------------
	//! @brief コンストラクタ呼び出し補助
	//-------------------------------------------------------------
	template<typename TInterface, typename TImplementation, typename... TDeps>
	void Container::registerCtor(Lifecycle cycle) {
		std::unique_lock<std::shared_mutex> lock(mutex_);	// スレッドセーフ
		// 型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		//登録済みなら例外
		if (registrations_.find(type) != registrations_.end()) {
			// 既に登録済みなら例外、上書きしたい場合はReplaceCtorを使用させる。
			throw ResolveException("Type already registered: " + std::string(type.name()));
		}
		// 登録処理
		registrations_[type] = Registration{
			cycle,								   // ライフサイクル	
			{ std::type_index(typeid(TDeps))... }, // 依存型リスト
			[](const std::vector<std::shared_ptr<void>>& args) {
				return callCtorImpl<TImplementation, TDeps...>(
					args, std::index_sequence_for<TDeps...>{});
			}
		};
		// Singleton の場合は古いインスタンスを消去
		if (cycle == Lifecycle::Singleton) {
			singletons_.erase(type);
		}
	}

	//-------------------------------------------------------------
	//! @brief コンストラクタ呼び出し補助上書き
	//-------------------------------------------------------------
	template<typename TInterface, typename TImplementation, typename... TDeps>
	void Container::replaceCtor(Lifecycle cycle_) {
		std::unique_lock<std::shared_mutex> lock(mutex_);
		const auto type = std::type_index(typeid(TInterface));
		registrations_[type] = Registration{
			cycle_,
			{ std::type_index(typeid(TDeps))... },
			[](const std::vector<std::shared_ptr<void>>& args) {
				return callCtorImpl<TImplementation, TDeps...>(
					args, std::index_sequence_for<TDeps...>{});
			}
		};
		// 旧 Singleton を必ず破棄
		singletons_.erase(type);
	}

	//-------------------------------------------------------------
	//! @brief インスタンス登録
	//-------------------------------------------------------------
	template<typename TInterface>
	void Container::registerInstance(std::shared_ptr<TInterface> instance) {
		std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
		// 型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		// 登録済み確認
		if (registrations_.find(type) != registrations_.end()) {
			// 既に登録済みなら例外、上書きしたい場合はReplaceInstanceを使用させる。
			throw ResolveException("Type already registered: " + std::string(type.name()));
		}
		// 登録処理
		registrations_[type] = {
			Lifecycle::Singleton,														// ライフサイクル
			{},																			// 依存なし
			[instance](const std::vector<std::shared_ptr<void>>&) { return instance; }	// ファクトリ関数
		};
		singletons_[typeid(TInterface)] = instance;  // シングルトンインスタンスとして保存
	}

	//-------------------------------------------------------------
	//! @brief インスタンス登録上書き
	//-------------------------------------------------------------
	template<typename TInterface>
	void Container::replaceInstance(std::shared_ptr<TInterface> instance) {
		std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
		// 型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		// 登録処理
		registrations_[type] = {
			Lifecycle::Singleton,														// ライフサイクル
			{},																			// 依存なし
			[instance](const std::vector<std::shared_ptr<void>>&) { return instance; }	// ファクトリ関数
		};
		// シングルトンインスタンスとして保存
		singletons_[type] = instance;
	}

	//-------------------------------------------------------------
	//! @brief 型解決（循環依存検出付き）
	//-------------------------------------------------------------
	template<typename TInterface>
	std::shared_ptr<TInterface> Container::resolve() {
		const auto key = std::type_index(typeid(TInterface));
		return std::static_pointer_cast<TInterface>(resolveByKey(key));
	}

	//-------------------------------------------------------------
	//! @brief 型が登録済みか確認
	//-------------------------------------------------------------
	template<typename TInterface>
	bool Container::isRegistered() const {
		std::shared_lock<std::shared_mutex> lock(mutex_); // 読み取り専用ロック
		// 型をハッシュキーとして取得
		const auto type = std::type_index(typeid(TInterface));
		// 登録情報マップに存在するか確認
		return registrations_.find(type) != registrations_.end();
	}

	//-------------------------------------------------------------
	//! @brief 型消去での内部解決ヘルパー
	//-------------------------------------------------------------
	std::shared_ptr<void> Container::resolveByKey(const std::type_index& key) {
		// 登録確認
		{
			std::shared_lock<std::shared_mutex> lock(mutex_);
			if (registrations_.find(key) == registrations_.end()) {
				throw ResolveException("Type not registered: " + std::string(key.name()));
			}
		}

		// RAII ガードで push/pop を自動化
		ResolvingGuardTL guard(key);

		// 登録情報スナップショット
		Registration reg_copy;
		{
			std::shared_lock<std::shared_mutex> lock(mutex_);
			reg_copy = registrations_.at(key);
		}

		// 既存 Singleton があれば返す
		if (reg_copy.cycle_ == Lifecycle::Singleton) {
			std::shared_lock<std::shared_mutex> lock(mutex_);
			auto it = singletons_.find(key);
			if (it != singletons_.end() && it->second) {
				return it->second; // guard が自動で pop
			}
		}

		// 依存を再帰的に解決
		std::vector<std::shared_ptr<void>> args;
		args.reserve(reg_copy.deps_.size());
		for (auto& depKey : reg_copy.deps_) {
			args.push_back(resolveByKey(depKey));
		}

		// 構築
		std::shared_ptr<void> created = reg_copy.ctor_(args);

		// Singleton の場合はキャッシュに保存
		if (reg_copy.cycle_ == Lifecycle::Singleton) {
			std::unique_lock<std::shared_mutex> lock(mutex_);
			auto& slot = singletons_[key];
			if (!slot) slot = created;
			return slot; // guard が自動で pop
		}

		// Scoped / Transient はそのまま返す
		return created; // guard が自動で pop
	}


	//-------------------------------------------------------------
	//! @brief ScopedContextを生成
	//-------------------------------------------------------------
	ScopedContext Container::createScope() {
		return ScopedContext(*this);	// ScopedContext を返す
	}
} // namespace TsukinoDIContainer