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