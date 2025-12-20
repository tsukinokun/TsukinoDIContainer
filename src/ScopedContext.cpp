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
}