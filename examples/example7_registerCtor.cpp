//-------------------------------------------------------------
//! @file   example7_registerCtor.cpp
//! @brief  TsukinoDIContainer registerCtor 使用例
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
//! @brief   コンソールロガー実装
//-------------------------------------------------------------
struct ConsoleLogger : public ILogger {
    //-------------------------------------------------------------
	//! @brief   ログ出力関数実装
	//! @param   msg  [in] ログメッセージ
    //-------------------------------------------------------------
    void log(const std::string& msg) override {
        std::cout << "[ConsoleLogger] " << msg << std::endl;
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
    //-------------------------------------------------------------
    void run() { logger_->log("Service is running"); }
private:
	std::shared_ptr<ILogger> logger_;   // 依存するロガーインスタンス
};

// エントリポイント
int main() {
	// コンテナ生成
    TsukinoDIContainer::Container container;

    // 1) 依存先 ILogger を先に登録（Singleton で共有）
    container.registerType<ILogger, ConsoleLogger>(TsukinoDIContainer::Lifecycle::Singleton);

    // 2) 依存関係付きで Service を新規登録（ILogger に依存）
    //    既に Service が登録されていたら例外で安全に失敗します
    container.registerCtor<Service, Service, ILogger>(TsukinoDIContainer::Lifecycle::Transient);

    // 3) 解決して実行
    auto service = container.resolve<Service>();
    service->run();

    // 4) 重複登録の誤用を示す（例外）
    try {
        container.registerCtor<Service, Service, ILogger>(TsukinoDIContainer::Lifecycle::Transient);
    }
    catch (const TsukinoDIContainer::ResolveException& ex) {
        std::cerr << "[Error] " << ex.what() << std::endl;
    }

    return 0;
}
