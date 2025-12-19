//-------------------------------------------------------------
//! @file   example8_mock.cpp
//! @brief  TsukinoDIContainer モック注入使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   ILogger
//! @brief   ロガーインターフェース
//-------------------------------------------------------------
struct ILogger {
	//-------------------------------------------------------------
	//! @brief   ログ出力関数
	//! @param   msg  [in] ログメッセージ
	//! @note    純粋仮想関数
	//-------------------------------------------------------------
	virtual void log(const std::string& msg) = 0;

	//-------------------------------------------------------------
	//! @brief   仮想デストラクタ
	//-------------------------------------------------------------
	virtual ~ILogger() = default;
};

//-------------------------------------------------------------
//! @class   ConsoleLogger
//! @brief   本番用コンソールロガー
//-------------------------------------------------------------
struct ConsoleLogger : public ILogger {
	//-------------------------------------------------------------
	//! @brief   ログ出力関数実装
	//! @param   msg  [in] ログメッセージ
	//! @details 本番環境で使用するロガー実装
	//-------------------------------------------------------------
	void log(const std::string& msg) override {
		std::cout << "[ConsoleLogger] " << msg << std::endl;
	}
};

//-------------------------------------------------------------
//! @class   MockLogger
//! @brief   テスト用モックロガー
//-------------------------------------------------------------
struct MockLogger : public ILogger {
	//-------------------------------------------------------------
	//! @brief   ログ出力関数実装
	//! @param   msg  [in] ログメッセージ
	//! @details テスト環境で使用するモックロガー実装
	//-------------------------------------------------------------
	void log(const std::string& msg) override {
		std::cout << "[MockLogger] captured message: " << msg << std::endl;
	}
};

//-------------------------------------------------------------
//! @class   Service
//! @brief   ロガーに依存するサービス
//-------------------------------------------------------------
struct Service {
	//-------------------------------------------------------------
	//! @brief   コンストラクタ（ILogger に依存）
	//! @param   logger  [in] ロガーインスタンス
	//! @details registerCtor を使って依存関係付きで登録
	//-------------------------------------------------------------
	explicit Service(std::shared_ptr<ILogger> logger) : logger_(std::move(logger)) {}

	//-------------------------------------------------------------
	//! @brief   サービス実行関数
	//! @details 依存するロガーを使ってログ出力
	//! @details run 関数内でログを出力
	//-------------------------------------------------------------
	void run() {
		logger_->log("Service is running");
	}
private:
	std::shared_ptr<ILogger> logger_;
};

// エントリポイント
int main() {
	// コンテナ生成
	TsukinoDIContainer::Container container;

	// 1. 本番用 ConsoleLogger を登録
	container.registerType<ILogger, ConsoleLogger>(TsukinoDIContainer::Lifecycle::Singleton);
	container.registerCtor<Service, Service, ILogger>(TsukinoDIContainer::Lifecycle::Transient);

	auto service1 = container.resolve<Service>();
	service1->run(); // ConsoleLogger が使われる

	// 2. テスト用 MockLogger に差し替え
	container.replaceType<ILogger, MockLogger>(TsukinoDIContainer::Lifecycle::Singleton);
	// Service の登録はそのまま再利用可能
	auto service2 = container.resolve<Service>();
	service2->run(); // MockLogger が使われる
	// プログラムの終了
	return 0;
}
