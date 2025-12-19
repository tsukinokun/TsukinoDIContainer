//-------------------------------------------------------------
//! @file   TsukinoDIContainer.hpp
//! @brief  依存性注入コンテナライブラリ
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#pragma once
#include "Lifecycle.hpp"
#include "ResolveException.hpp"
#include "Container.hpp"
#include "ScopedContext.hpp"
#include "ResolvingGuardTL.hpp"
//#include <memory>
//#include <unordered_map>
//#include <unordered_set>
//#include <functional>
//#include <typeindex>
//#include <stdexcept>
//#include <string>
//#include <utility>
//#include <algorithm>
//#include <mutex>
//#include <shared_mutex>
////宣言部
//namespace TsukinoDIContainer {
//	////-------------------------------------------------------------
//	////! @enum   Lifecycle
//	////! @brief  インスタンスのライフサイクル管理方法
//	////-------------------------------------------------------------
//	//enum class Lifecycle {
//	//	Transient,  //!< 毎回新しいインスタンス
//	//	Singleton,  //!< 全体で共有
//	//	Scoped      //!< スコープごとに共有
//	//};
//
//	////-------------------------------------------------------------
//	////! @class  ResolveException
//	////! @brief  解決エラー例外クラス
//	////-------------------------------------------------------------
//	//class ResolveException : public std::runtime_error {
//	//public:
//	//	//-------------------------------------------------------------
//	//	// コンストラクタ
//	//	//! @param  msg エラーメッセージ
//	//	//-------------------------------------------------------------
//	//	explicit ResolveException(const std::string& msg);
//	//};
//
//	class Container; // 前方宣言
//
//	////-------------------------------------------------------------
//	////! @class ScopedContext
//	////! @brief Scoped ライフタイム管理用 RAII クラス
//	////-------------------------------------------------------------
//	//class ScopedContext {
//	//public:
//	//	//-------------------------------------------------------------
//	//	// コンストラクタ
//	//	//! @param  container  [in] 所属するコンテナ
//	//	//-------------------------------------------------------------
//	//	explicit ScopedContext(Container& container);
//
//	//	//-------------------------------------------------------------
//	//	// デストラクタ
//	//	//-------------------------------------------------------------
//	//	~ScopedContext();
//
//	//	//-------------------------------------------------------------
//	//	// コピー禁止
//	//	//-------------------------------------------------------------
//	//	ScopedContext(const ScopedContext&) = delete;
//
//	//	//-------------------------------------------------------------
//	//	// コピー禁止
//	//	//-------------------------------------------------------------
//	//	ScopedContext& operator=(const ScopedContext&) = delete;
//
//	//	//-------------------------------------------------------------
//	//	// ムーブコンストラクタ
//	//	//! @param	 other  [in] 移動元オブジェクト
//	//	//! @details ムーブ後、移動元のデストラクタではスコープ終了処理を行わない
//	//	//! @details ムーブ元のオブジェクトを使用しないで下さい
//	//	//-------------------------------------------------------------
//	//	ScopedContext(ScopedContext&& other) noexcept;
//
//	//	//-------------------------------------------------------------
//	//	// 依存関係の解決
//	//	//! @tparam  TInterface 抽象型インターフェース
//	//	//! @return  解決されたインスタンス
//	//	//! @throws	 ResolveException 未登録型、循環依存、未知ライフサイクルの場合
//	//	//! @details コンテナの Resolve を呼び出す
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface>
//	//	std::shared_ptr<TInterface> resolve();
//
//	//private:
//	//	Container& container_;	// 所属するコンテナへの参照
//	//	std::unordered_map<std::type_index, std::shared_ptr<void>> scoped_instances_;	// スコープインスタンスマップ
//	//	bool active_;			// スコープが有効かどうかのフラグ
//	//};
//
//	////-------------------------------------------------------------
//	////! @class   Container
//	////! @brief   依存性注入コンテナクラス
//	////! @details 型登録、解決、スコープ管理を提供
//	////-------------------------------------------------------------
//	//class Container {
//	//	friend class ScopedContext; // ScopedContext に内部アクセスを許可
//	//public:
//	//	//-------------------------------------------------------------
//	//	// 型登録
//	//	//! @tparam  TInterface			 抽象型インターフェース
//	//	//! @tparam  TImplementation	 具象型実装
//	//	//! @param   cycle_  [in]        ライフサイクル管理方法（デフォルトはTransient）
//	//	//! @throws	 ResolveException 既に登録済みの場合
//	//	//! @details 登録済みの場合は例外を投げる。上書きしたい場合はReplaceを使用してください。
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface, typename TImplementation>
//	//	void registerType(Lifecycle cycle_ = Lifecycle::Transient);
//
//	//	//-------------------------------------------------------------
//	//	// 型登録上書き
//	//	//! @tparam  TInterface			 抽象型インターフェース
//	//	//! @tparam  TImplementation	 具象型実装
//	//	//! @param   cycle_  [in]        ライフサイクル管理方法（デフォルトはTransient）
//	//	//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface, typename TImplementation>
//	//	void replaceType(Lifecycle cycle_ = Lifecycle::Transient);
//
//	//	//-------------------------------------------------------------
//	//	// コンストラクタ呼び出し補助
//	//	//! @tparam TImplementation 具象型実装
//	//	//! @tparam TDeps         依存型リスト
//	//	//! @param  args          [in] 依存インスタンスリスト
//	//	//! @param  I             インデックスシーケンス
//	//	//! @return 生成されたインスタンス
//	//	//! @details 依存インスタンスを静的ポインタキャストしてコンストラクタに渡す
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface, typename TImplementation, typename... TDeps>
//	//	 void registerCtor(Lifecycle cycle);
//
//	//	//-------------------------------------------------------------
//	//	// コンストラクタ呼び出し補助上書き
//	//	//! @tparam TImplementation 具象型実装
//	//	//! @tparam TDeps         依存型リスト
//	//	//! @param  args          [in] 依存インスタンスリスト
//	//	//! @return 生成されたインスタンス
//	//	//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface, typename TImplementation, typename... TDeps>
//	//	void replaceCtor(Lifecycle cycle_ = Lifecycle::Transient);
//
//	//	//-------------------------------------------------------------
//	//	// インスタンス登録
//	//	//! @tparam TInterface 抽象型インターフェース
//	//	//! @param  instance   [in] 具象型インスタンス
//	//	//! @throws	 ResolveException 既に登録済みの場合
//	//	//! @details シングルトンとして登録される
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface>
//	//	void registerInstance(std::shared_ptr<TInterface> instance);
//
//	//	//-------------------------------------------------------------
//	//	// インスタンス登録上書き
//	//	//! @tparam TInterface 抽象型インターフェース
//	//	//! @param  instance   [in] 具象型インスタンス
//	//	//! @details 既に登録済みの場合でも上書きする。未登録の場合は新規登録となる。
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface>
//	//	void replaceInstance(std::shared_ptr<TInterface> instance);
//
//	//	//-------------------------------------------------------------
//	//	// 型解決（循環依存検出付き）
//	//	//! @tparam TInterface 抽象型インターフェース
//	//	//! @return 解決されたインスタンス
//	//	//! @throws ResolveException 未登録型、循環依存、未知ライフサイクルの場合
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface>
//	//	std::shared_ptr<TInterface> resolve();
//
//	//	//-------------------------------------------------------------
//	//	// 型が登録済みか確認
//	//	//! @tparam TInterface 抽象型インターフェース
//	//	//! @return 登録済みなら true、未登録なら false
//	//	//-------------------------------------------------------------
//	//	template<typename TInterface>
//	//	bool isRegistered() const;
//
//	//	//-------------------------------------------------------------
//	//	// キーによる型解決（循環依存検出付き）
//	//	//! @param  key  [in] 型のキー（type_index）
//	//	//! @return  解決されたインスタンス
//	//	//! @throws ResolveException 未登録型、循環依存、未知ライフサイクルの場合
//	//	//-------------------------------------------------------------
//	//	std::shared_ptr<void> resolveByKey(const std::type_index& key);
//
//	//	//-------------------------------------------------------------
//	//	// ScopedContext を生成
//	//	//! @return ScopedContext RAII スコープオブジェクト
//	//	//-------------------------------------------------------------
//	//	ScopedContext createScope();
//
//	//private:
//
//	//	//---------------------------------------------------------
//	//	//! @struct Registration
//	//	//! @brief  登録情報構造体
//	//	//! @details ライフサイクルとファクトリ関数を保持
//	//	//---------------------------------------------------------
//	//	struct Registration {
//	//		Lifecycle cycle_ = Lifecycle::Transient;												// ライフサイクル(デフォルトはTransient)
//	//		std::vector<std::type_index> deps_;														// 依存型リスト（空なら引数なし）
//	//		std::function<std::shared_ptr<void>(const std::vector<std::shared_ptr<void>>&)> ctor_;	// ファクトリ関数
//	//	};
//
//	//	std::unordered_map<std::type_index, Registration> registrations_;				// 登録情報マップ
//	//	std::unordered_map<std::type_index, std::shared_ptr<void>> singletons_;			// シングルトンインスタンスマップ
//	//	mutable std::shared_mutex mutex_;												// スレッドセーフ用ミューテックス
//	//};
//
//	////---------------------------------------------------------
//	////! @struct  ResolvingGuardTL
//	////! @brief   循環依存検出用ガードクラス（スレッドローカル）
//	////! @details コンストラクタでスタックに型をプッシュ、デストラクタでポップ
//	////---------------------------------------------------------
//	//struct ResolvingGuardTL {
//	//	std::type_index type_;	// ガード対象の型
//	//	bool active_{ false };	// ガードが有効かどうかのフラグ
//
//	//	//---------------------------------------------------------
//	//	//! @brief  コンストラクタ
//	//	//! @param  t  [in] ガード対象の型
//	//	//! @throws ResolveException 循環依存が検出された場合
//	//	//---------------------------------------------------------
//	//	explicit ResolvingGuardTL(std::type_index t);
//
//	//	//---------------------------------------------------------
//	//	//! @brief  デストラクタ
//	//	//! @details スタックから型をポップ
//	//	//---------------------------------------------------------
//	//	~ResolvingGuardTL();
//	//};
//
//} // namespace TsukinoDIContainer
//
////実装部
//namespace TsukinoDIContainer
//{
//	//-------------------------------------------------------------
//	// ResolveExceptionクラスの実装
//	//-------------------------------------------------------------
//
//	////-------------------------------------------------------------
//	////! @brief  コンストラクタ
//	////-------------------------------------------------------------
//	// ResolveException::ResolveException(const std::string& msg)
//	//	: std::runtime_error("TsukinoDIContainer Resolve Error: " + msg) {
//	//}
//
//	//-------------------------------------------------------------
//	// ScopedContextクラスの実装
//	//-------------------------------------------------------------
//
//	////-------------------------------------------------------------
//	////! @brief  コンストラクタ
//	////-------------------------------------------------------------
//	// ScopedContext::ScopedContext(Container& container)
//	//	: container_(container), active_(true) {
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief  デストラクタ
//	////-------------------------------------------------------------
//	// ScopedContext::~ScopedContext() {
//	//	scoped_instances_.clear(); // 自分専用のマップを破棄
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief  ムーブコンストラクタ
//	////-------------------------------------------------------------
//	// ScopedContext::ScopedContext(ScopedContext&& other) noexcept
//	//	: container_(other.container_), active_(std::exchange(other.active_, false)) {
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief   依存関係の解決
//	////-------------------------------------------------------------
//	//template<typename TInterface>
//	//std::shared_ptr<TInterface> ScopedContext::resolve() {
//	//	const auto type = std::type_index(typeid(TInterface));
//
//	//	// 既にスコープ内に存在する場合はそれを返す
//	//	auto it = scoped_instances_.find(type);
//	//	if (it != scoped_instances_.end()) {
//	//		return std::static_pointer_cast<TInterface>(it->second);
//	//	}
//	//	// コンテナに登録されているか確認
//	//	if (!container_.isRegistered<TInterface>()) {
//	//		throw ResolveException("Type not registered in container: " + std::string(type.name()));
//	//	}
//	//	// 登録情報のスナップショットを取得（ロック付き）
//	//	Container::Registration reg_copy;
//	//	{
//	//		std::shared_lock<std::shared_mutex> lock(container_.mutex_);
//	//		reg_copy = container_.registrations_.at(type);
//	//	}
//	//	// 依存を再帰的に解決
//	//	std::vector<std::shared_ptr<void>> args;
//	//	args.reserve(reg_copy.deps_.size());
//	//	for (auto& depKey : reg_copy.deps_) {
//	//		args.push_back(container_.resolveByKey(depKey));
//	//	}
//	//	// インスタンス生成（ロック外）
//	//	auto instance = std::static_pointer_cast<TInterface>(reg_copy.ctor_(args));
//	//	// スコープ内に保存
//	//	scoped_instances_[type] = instance;
//	//	// 生成されたインスタンスを返す
//	//	return instance;
//	//}
//
//	////-------------------------------------------------------------
//	//// Containerクラスの実装
//	////-------------------------------------------------------------
//	//static thread_local std::vector<std::type_index> g_resolving_stack; // 循環依存検出用スタック
//	////-------------------------------------------------------------
//	////! @brief 型登録
//	////-------------------------------------------------------------
//	//template<typename TInterface, typename TImplementation>
//	// void Container::registerType(Lifecycle cycle_) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
//	//	// 登録済み確認
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	if (registrations_.find(type) != registrations_.end()) {
//	//		// 既に登録済みなら例外、上書きしたい場合はReplaceを使用させる。
//	//		throw ResolveException("Type already registered: " + std::string(type.name()));
//	//	}
//	//	// 登録処理
//	//	registrations_[type] = {
//	//		cycle_,
//	//		{}, // 依存なし
//	//		[](const std::vector<std::shared_ptr<void>>&) {
//	//			return std::make_shared<TImplementation>();
//	//		}
//	//	};
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief 型登録上書き
//	////-------------------------------------------------------------
//	//template<typename TInterface, typename TImplementation>
//	// void Container::replaceType(Lifecycle cycle_) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
//	//	//型をハッシュキーとして取得
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	// 登録処理
//	//	registrations_[type] = {
//	//		cycle_,
//	//		{}, // 依存なし
//	//		[](const std::vector<std::shared_ptr<void>>&) {
//	//			return std::make_shared<TImplementation>();
//	//		}
//	//	};
//	//	// 旧 Singleton を必ず破棄（次回解決で新規生成） 
//	//	singletons_.erase(type);
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief コンストラクタ呼び出し補助
//	////! @tparam TImplementation 具象型実装
//	////! @tparam TDeps         依存型リスト
//	////! @param  args          [in] 依存インスタンスリスト
//	////! @param  I             インデックスシーケンス
//	////! @return 生成されたインスタンス
//	////-------------------------------------------------------------
//	//template<typename TImplementation, typename... TDeps, std::size_t... I>
//	//static std::shared_ptr<void> callCtorImpl(
//	//	const std::vector<std::shared_ptr<void>>& args,
//	//	std::index_sequence<I...>)
//	//{
//	//	// 依存インスタンスを静的ポインタキャストしてコンストラクタに渡す
//	//	return std::make_shared<TImplementation>(
//	//		std::static_pointer_cast<TDeps>(args[I])...
//	//	);
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief コンストラクタ呼び出し補助
//	////-------------------------------------------------------------
//	//template<typename TInterface, typename TImplementation, typename... TDeps>
//	// void Container::registerCtor(Lifecycle cycle) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_);	// スレッドセーフ
//	//	// 型をハッシュキーとして取得
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	//登録済みなら例外
//	//	if (registrations_.find(type) != registrations_.end()) {
//	//		// 既に登録済みなら例外、上書きしたい場合はReplaceCtorを使用させる。
//	//		throw ResolveException("Type already registered: " + std::string(type.name()));
//	//	}
//	//	// 登録処理
//	//	registrations_[type] = Registration{
//	//		cycle,								   // ライフサイクル	
//	//		{ std::type_index(typeid(TDeps))... }, // 依存型リスト
//	//		[](const std::vector<std::shared_ptr<void>>& args) {
//	//			return callCtorImpl<TImplementation, TDeps...>(
//	//				args, std::index_sequence_for<TDeps...>{});
//	//		}
//	//	};
//	//	// Singleton の場合は古いインスタンスを消去
//	//	if (cycle == Lifecycle::Singleton) {
//	//		singletons_.erase(type);
//	//	}
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief コンストラクタ呼び出し補助上書き
//	////-------------------------------------------------------------
//	//template<typename TInterface, typename TImplementation, typename... TDeps>
//	// void Container::replaceCtor(Lifecycle cycle_) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_);
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	registrations_[type] = Registration{
//	//		cycle_,
//	//		{ std::type_index(typeid(TDeps))... },
//	//		[](const std::vector<std::shared_ptr<void>>& args) {
//	//			return callCtorImpl<TImplementation, TDeps...>(
//	//				args, std::index_sequence_for<TDeps...>{});
//	//		}
//	//	};
//	//	// 旧 Singleton を必ず破棄
//	//	singletons_.erase(type);
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief インスタンス登録
//	////-------------------------------------------------------------
//	//template<typename TInterface>
//	// void Container::registerInstance(std::shared_ptr<TInterface> instance) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
//	//	// 型をハッシュキーとして取得
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	// 登録済み確認
//	//	if (registrations_.find(type) != registrations_.end()) {
//	//		// 既に登録済みなら例外、上書きしたい場合はReplaceInstanceを使用させる。
//	//		throw ResolveException("Type already registered: " + std::string(type.name()));
//	//	}
//	//	// 登録処理
//	//	registrations_[type] = {
//	//		Lifecycle::Singleton,														// ライフサイクル
//	//		{},																			// 依存なし
//	//		[instance](const std::vector<std::shared_ptr<void>>&) { return instance; }	// ファクトリ関数
//	//	};
//	//	singletons_[typeid(TInterface)] = instance;  // シングルトンインスタンスとして保存
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief インスタンス登録上書き
//	////-------------------------------------------------------------
//	//template<typename TInterface>
//	// void Container::replaceInstance(std::shared_ptr<TInterface> instance) {
//	//	std::unique_lock<std::shared_mutex> lock(mutex_); // スレッドセーフ
//	//	// 型をハッシュキーとして取得
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	// 登録処理
//	//	registrations_[type] = {
//	//		Lifecycle::Singleton,														// ライフサイクル
//	//		{},																			// 依存なし
//	//		[instance](const std::vector<std::shared_ptr<void>>&) { return instance; }	// ファクトリ関数
//	//	};
//	//	// シングルトンインスタンスとして保存
//	//	singletons_[type] = instance;
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief 型解決（循環依存検出付き）
//	////-------------------------------------------------------------
//	//template<typename TInterface>
//	// std::shared_ptr<TInterface> Container::resolve() {
//	//	const auto key = std::type_index(typeid(TInterface));
//	//	return std::static_pointer_cast<TInterface>(resolveByKey(key));
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief 型が登録済みか確認
//	////-------------------------------------------------------------
//	//template<typename TInterface>
//	// bool Container::isRegistered() const {
//	//	std::shared_lock<std::shared_mutex> lock(mutex_); // 読み取り専用ロック
//	//	// 型をハッシュキーとして取得
//	//	const auto type = std::type_index(typeid(TInterface));
//	//	// 登録情報マップに存在するか確認
//	//	return registrations_.find(type) != registrations_.end();
//	//}
//
//	////-------------------------------------------------------------
//	////! @brief 型消去での内部解決ヘルパー
//	////-------------------------------------------------------------
//	// std::shared_ptr<void> Container::resolveByKey(const std::type_index& key) {
//	//	// 登録確認
//	//	{
//	//		std::shared_lock<std::shared_mutex> lock(mutex_);
//	//		if (registrations_.find(key) == registrations_.end()) {
//	//			throw ResolveException("Type not registered: " + std::string(key.name()));
//	//		}
//	//	}
//
//	//	// RAII ガードで push/pop を自動化
//	//	ResolvingGuardTL guard(key);
//
//	//	// 登録情報スナップショット
//	//	Registration reg_copy;
//	//	{
//	//		std::shared_lock<std::shared_mutex> lock(mutex_);
//	//		reg_copy = registrations_.at(key);
//	//	}
//
//	//	// 既存 Singleton があれば返す
//	//	if (reg_copy.cycle_ == Lifecycle::Singleton) {
//	//		std::shared_lock<std::shared_mutex> lock(mutex_);
//	//		auto it = singletons_.find(key);
//	//		if (it != singletons_.end() && it->second) {
//	//			return it->second; // guard が自動で pop
//	//		}
//	//	}
//
//	//	// 依存を再帰的に解決
//	//	std::vector<std::shared_ptr<void>> args;
//	//	args.reserve(reg_copy.deps_.size());
//	//	for (auto& depKey : reg_copy.deps_) {
//	//		args.push_back(resolveByKey(depKey));
//	//	}
//
//	//	// 構築
//	//	std::shared_ptr<void> created = reg_copy.ctor_(args);
//
//	//	// Singleton の場合はキャッシュに保存
//	//	if (reg_copy.cycle_ == Lifecycle::Singleton) {
//	//		std::unique_lock<std::shared_mutex> lock(mutex_);
//	//		auto& slot = singletons_[key];
//	//		if (!slot) slot = created;
//	//		return slot; // guard が自動で pop
//	//	}
//
//	//	// Scoped / Transient はそのまま返す
//	//	return created; // guard が自動で pop
//	//}
//
//
//	////-------------------------------------------------------------
//	////! @brief ScopedContextを生成
//	////-------------------------------------------------------------
//	// ScopedContext Container::createScope() {
//	//	return ScopedContext(*this);	// ScopedContext を返す
//	//}
//
//	//---------------------------------------------------------
//	// ResolvingGuardTLクラスの実装
//	//---------------------------------------------------------
//
//	////---------------------------------------------------------
//	////! @brief  コンストラクタ
//	////---------------------------------------------------------
//	//ResolvingGuardTL::ResolvingGuardTL(std::type_index t)
//	//	: type_(t) {
//	//	// 循環依存検出
//	//	if (std::find(g_resolving_stack.begin(), g_resolving_stack.end(), type_) != g_resolving_stack.end()) {
//	//		std::string chain;
//	//		for (auto& x : g_resolving_stack) {
//	//			chain += x.name();
//	//			chain += " -> ";
//	//		}
//	//		chain += type_.name();
//	//		throw ResolveException("Circular dependency detected: " + chain);
//	//	}
//	//	g_resolving_stack.push_back(type_);
//	//	active_ = true;
//	//}
//
//	////---------------------------------------------------------
//	////! @brief  デストラクタ
//	////! @details スタックから型をポップ
//	////---------------------------------------------------------
//	//ResolvingGuardTL::~ResolvingGuardTL() {
//	//	if (active_) {
//	//		g_resolving_stack.pop_back();
//	//	}
//	//}
//}// namespace TsukinoDIContainer