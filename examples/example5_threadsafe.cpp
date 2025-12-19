//-------------------------------------------------------------
//! @file   example5_threadsafe.cpp
//! @brief  TsukinoDIContainer スレッドセーフ使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <thread>
#include <vector>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   IWorkerService
//! @brief   ワーカーサービスインターフェース
//-------------------------------------------------------------
struct IWorkerService {
	//-------------------------------------------------------------
	//! @brief   並列処理をする関数
	//! @param   id  [in] ワーカーID
	//-------------------------------------------------------------
	virtual void doWork(int id) = 0;

	//-------------------------------------------------------------
	//! @brief   仮想デストラクタ
	//-------------------------------------------------------------
	virtual ~IWorkerService() = default;
};

//-------------------------------------------------------------
//! @class   WorkerService
//! @brief   ワーカーサービス実装
//-------------------------------------------------------------
struct WorkerService : public IWorkerService {
	//-------------------------------------------------------------
	//! @brief   並列処理をする関数実装
	//! @param   id  [in] ワーカーID
	//! @details ワーカーIDとスレッドIDを表示
	//-------------------------------------------------------------
	void doWork(int id) override {
		std::cout << "Worker " << id << " is working in thread "
			<< std::this_thread::get_id() << std::endl;
	}
};

// エントリポイント
int main() {
	// コンテナ生成
	TsukinoDIContainer::Container container;

	// Transientライフサイクルで登録
	container.registerType<IWorkerService, WorkerService>(TsukinoDIContainer::Lifecycle::Transient);

	// 複数スレッドで Resolve を呼び出す
	const int threadCount = 5;
	std::vector<std::thread> threads;

	// スレッド生成
	for (int i = 0; i < threadCount; ++i) {
		threads.emplace_back([&, i]() {
			auto worker = container.resolve<IWorkerService>();	// スレッドセーフに解決
			worker->doWork(i);									// ワーク実行
			});
	}

	// 全スレッド終了待ち
	for (auto& t : threads) {
		t.join();	// スレッドの終了を待つ
	}
	// プログラムの終了
	return 0;
}
