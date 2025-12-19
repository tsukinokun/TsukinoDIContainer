//-------------------------------------------------------------
//! @file	example1_basic.cpp
//! @brief	TsukinoDIContainer 基本的な使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   IMessageService
//! @brief   メッセージサービスインターフェース
//! @details メッセージ送信の抽象インターフェース
//-------------------------------------------------------------
struct IMessageService {
	//-------------------------------------------------------------
	//! @brief   メッセージ送信
	//! @param   msg  [in] 送信するメッセージ
	//! @note    純粋仮想関数
	//-------------------------------------------------------------
	virtual void send(const std::string& msg) = 0;

	//-------------------------------------------------------------
	//! @brief   仮想デストラクタ
	//-------------------------------------------------------------
	virtual ~IMessageService() = default;
};

//-------------------------------------------------------------
//! @class   ConsoleMessageService
//! @brief   コンソールメッセージサービス実装
//! @details IMessageService のコンソール出力実装
//-------------------------------------------------------------
struct ConsoleMessageService : public IMessageService {
	//-------------------------------------------------------------
	//! @brief   メッセージ送信実装
	//! @param   msg  [in] 送信するメッセージ
	//-------------------------------------------------------------
	void send(const std::string& msg) override {
		std::cout << "[Console] " << msg << std::endl;
	}
};

// エントリポイント
int main() {
	// コンテナ生成
	TsukinoDIContainer::Container container;

	// 型登録(抽象型インターフェース : IMessageService 具体型 : ConsoleMessageService)
	// ライフサイクルはTransient（毎回新しいインスタンスを生成する）
	container.registerType<IMessageService, ConsoleMessageService>(TsukinoDIContainer::Lifecycle::Transient);

	// 依存関係を解決して利用
	auto service = container.resolve<IMessageService>();
	// メッセージ送信
	service->send("Hello, TsukinoDIContainer!");
	// プログラムの終了
	return 0;
}
