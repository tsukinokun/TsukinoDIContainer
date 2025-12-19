//-------------------------------------------------------------
//! @file   ResolvingStack.hpp
//! @brief  解決中スタック（スレッドローカル）定義
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
#include <vector>
#include <typeindex>
// 名前空間 : TsukinoDIContainer
namespace TsukinoDIContainer
{
	extern thread_local std::vector<std::type_index> g_resolving_stack;
}