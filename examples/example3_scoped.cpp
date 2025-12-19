//-------------------------------------------------------------
//! @file   example3_scoped.cpp
//! @brief  TsukinoDIContainer Scopedライフサイクル使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   ISessionService
//! @brief   セッションサービスインターフェース
//-------------------------------------------------------------
struct ISessionService {
	//-------------------------------------------------------------
	//! @brief   値を設定する関数
	//-------------------------------------------------------------
	virtual void setValue(int v) = 0;

	//-------------------------------------------------------------
	//! @brief   値を取得する関数
	//-------------------------------------------------------------
	virtual int getValue() const = 0;

	//-------------------------------------------------------------
	//! @brief   仮想デストラクタ
	//-------------------------------------------------------------
	virtual ~ISessionService() = default;
};

//-------------------------------------------------------------
//! @class   SessionService
//! @brief   セッションサービス実装
//-------------------------------------------------------------
struct SessionService : public ISessionService {
	int value_ = 0;  // 内部状態

	//-------------------------------------------------------------
	//! @brief   値を設定する関数実装
	//! @param   v [in] 設定する値
	//-------------------------------------------------------------
	void setValue(int v) override { value_ = v; }

	//-------------------------------------------------------------
	//! @brief   値を取得する関数実装
	//! @return  設定された値
	//-------------------------------------------------------------
	int getValue() const override { return value_; }
};

// エントリポイント
int main() {
	// コンテナ生成
	TsukinoDIContainer::Container container;

	// Scoped ライフサイクルで登録
	container.registerType<ISessionService, SessionService>(TsukinoDIContainer::Lifecycle::Scoped);

	// スコープ1開始
	{
		auto scope1 = container.createScope();          // スコープ開始
		auto s1a = scope1.resolve<ISessionService>();   // スコープ内で解決
		s1a->setValue(42);                              // 値を設定
		// 同じスコープ内で再度解決
		auto s1b = scope1.resolve<ISessionService>();
		// s1aとs1bは同じインスタンス
		std::cout << "Scope1 values: " << s1a->getValue() << ", " << s1b->getValue() << std::endl;
		// s1a と s1b は同じインスタンスを共有
	}// スコープ1終了（RAIIなのでインスタンスは破棄される）

	// スコープ2開始（新しいインスタンスが生成される）
	{
		auto scope2 = container.createScope();          // スコープ開始
		auto s2a = scope2.resolve<ISessionService>();   // スコープ内で解決
		// 以前のスコープとは別インスタンス
		std::cout << "Scope2 initial value: " << s2a->getValue() << std::endl;

		s2a->setValue(99);  // 値を設定
		// 同じスコープ内で再度解決
		auto s2b = scope2.resolve<ISessionService>();
		// s2aとs2bは同じインスタンス
		std::cout << "Scope2 values: " << s2a->getValue() << ", " << s2b->getValue() << std::endl;
		// s2a と s2b は同じインスタンスを共有
	}

	return 0;
}
