//-------------------------------------------------------------
//! @file   ResolvingGuardTL.cpp
//! @brief  依存性注入コンテナライブラリの循環依存検出用ガード実装
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include "TsukinoDIContainer/ResolvingStack.hpp"
// // 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer
{
	thread_local std::vector<std::type_index> g_resolving_stack;
}// namespace TsukinoDIContainer