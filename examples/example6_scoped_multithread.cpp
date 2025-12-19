//-------------------------------------------------------------
//! @file   example6_scoped_multithread.cpp
//! @brief  TsukinoDIContainer Scoped ライフサイクル + マルチスレッド使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <thread>
#include <vector>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   ISessionService
//! @brief   セッションサービスインターフェース
//-------------------------------------------------------------
struct ISessionService {
	//-------------------------------------------------------------
	//! @brief   値を設定する関数
	//! @param   v [in] 設定する値
	//! @note    純粋仮想関数
	//-------------------------------------------------------------
	virtual void setValue(int v) = 0;

	//-------------------------------------------------------------
	//! @brief   値を取得する関数
	//! @return  設定された値
	//! @note    純粋仮想関数
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
	int value_ = 0;	// 内部状態

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

//-------------------------------------------------------------
//! @brief エントリポイント
//-------------------------------------------------------------
int main() {
	// コンテナ生成
	TsukinoDIContainer::Container container;

	// Scoped ライフサイクルで登録
	container.registerType<ISessionService, SessionService>(TsukinoDIContainer::Lifecycle::Scoped);

	// マルチスレッドでスコープを生成して利用
	const int threadCount = 3;
	std::vector<std::thread> threads;

	// 出力用の排他制御 
	static std::mutex cout_mutex;

	// 各スレッドで独自のスコープを生成
	for (int i = 0; i < threadCount; ++i) {
		// スレッド生成
		threads.emplace_back([&, i]() {
			auto scope = container.createScope(); // スレッドごとにスコープ生成
			// スコープ内で同じ型を2回解決
			auto s1 = scope.resolve<ISessionService>();
			s1->setValue(i * 10);
			// 同じスコープ内で再度解決
			auto s2 = scope.resolve<ISessionService>();
			{
				std::lock_guard<std::mutex> lock(cout_mutex); // 出力を排他制御
				// 同じスコープ内で共有されていることを確認
				std::cout << "Thread " << i
					<< " values: " << s1->getValue()
					<< ", " << s2->getValue()
					<< " (shared in same scope)" << std::endl;
			}
			});
	}
	// スレッド終了待機
	for (auto& t : threads) {
		t.join();
	}
	// プログラムの終了
	return 0;
}
