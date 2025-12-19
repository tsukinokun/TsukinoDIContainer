//-------------------------------------------------------------
//! @file   ResolvingGuardTL.cpp
//! @brief  依存性注入コンテナライブラリの循環依存検出用ガード実装
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include "TsukinoDIContainer/ResolvingGuardTL.hpp"
#include "TsukinoDIContainer/ResolveException.hpp"
#include "TsukinoDIContainer/ResolvingStack.hpp"
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer {
	//---------------------------------------------------------
	//! @brief  コンストラクタ
	//---------------------------------------------------------
	ResolvingGuardTL::ResolvingGuardTL(std::type_index t)
		: type_(t) {
		// 循環依存検出
		if (std::find(g_resolving_stack.begin(), g_resolving_stack.end(), type_) != g_resolving_stack.end()) {
			std::string chain;
			for (auto& x : g_resolving_stack) {
				chain += x.name();
				chain += " -> ";
			}
			chain += type_.name();
			throw ResolveException("Circular dependency detected: " + chain);
		}
		g_resolving_stack.push_back(type_);
		active_ = true;
	}

	//---------------------------------------------------------
	//! @brief  デストラクタ
	//! @details スタックから型をポップ
	//---------------------------------------------------------
	ResolvingGuardTL::~ResolvingGuardTL() {
		if (active_) {
			g_resolving_stack.pop_back();
		}
	}
}// namespace TsukinoDIContainer