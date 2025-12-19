//-------------------------------------------------------------
//! @file   TsukinoDIContainer.cpp
//! @brief  依存性注入コンテナライブラリのスコープコンテキスト実装
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include "TsukinoDIContainer/Container.hpp"
#include "TsukinoDIContainer/ScopedContext.hpp"
#include "TsukinoDIContainer/ResolveException.hpp"
#include "TsukinoDIContainer/ResolvingGuardTL.hpp"
namespace TsukinoDIContainer {
	//-------------------------------------------------------------
	//! @brief  コンストラクタ
	//-------------------------------------------------------------
	 ScopedContext::ScopedContext(Container& container)
		: container_(container), active_(true) {
	}

	//-------------------------------------------------------------
	//! @brief  デストラクタ
	//-------------------------------------------------------------
	 ScopedContext::~ScopedContext() {
		scoped_instances_.clear(); // 自分専用のマップを破棄
	}

	//-------------------------------------------------------------
	//! @brief  ムーブコンストラクタ
	//-------------------------------------------------------------
	 ScopedContext::ScopedContext(ScopedContext&& other) noexcept
		: container_(other.container_), active_(std::exchange(other.active_, false)) {
	}

	//-------------------------------------------------------------
	//! @brief   依存関係の解決
	//-------------------------------------------------------------
	template<typename TInterface>
	std::shared_ptr<TInterface> ScopedContext::resolve() {
		const auto type = std::type_index(typeid(TInterface));

		// 既にスコープ内に存在する場合はそれを返す
		auto it = scoped_instances_.find(type);
		if (it != scoped_instances_.end()) {
			return std::static_pointer_cast<TInterface>(it->second);
		}
		// コンテナに登録されているか確認
		if (!container_.isRegistered<TInterface>()) {
			throw ResolveException("Type not registered in container: " + std::string(type.name()));
		}
		// 登録情報のスナップショットを取得（ロック付き）
		Container::Registration reg_copy;
		{
			std::shared_lock<std::shared_mutex> lock(container_.mutex_);
			reg_copy = container_.registrations_.at(type);
		}
		// 依存を再帰的に解決
		std::vector<std::shared_ptr<void>> args;
		args.reserve(reg_copy.deps_.size());
		for (auto& depKey : reg_copy.deps_) {
			args.push_back(container_.resolveByKey(depKey));
		}
		// インスタンス生成（ロック外）
		auto instance = std::static_pointer_cast<TInterface>(reg_copy.ctor_(args));
		// スコープ内に保存
		scoped_instances_[type] = instance;
		// 生成されたインスタンスを返す
		return instance;
	}
}