//-------------------------------------------------------------
//! @file   example2_singleton.cpp
//! @brief  TsukinoDIContainer Singletonライフサイクル使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>
//-------------------------------------------------------------
//! @class   ICounterService
//! @brief   カウンターサービスインターフェース
//-------------------------------------------------------------
struct ICounterService {
    //-------------------------------------------------------------
	//! @brief   カウンターをインクリメントする関数
	//! @note    純粋仮想関数
    //-------------------------------------------------------------
    virtual void increment() = 0;

	//-------------------------------------------------------------
	//! @brief   カウンターの現在値を取得する関数
	//! @note    純粋仮想関数
	//! @return  カウンターの現在値
	//! @note    純粋仮想関数
	//-------------------------------------------------------------
    virtual int getValue() const = 0;

	//-------------------------------------------------------------
	//! @brief   仮想デストラクタ
	//-------------------------------------------------------------
    virtual ~ICounterService() = default;
};

//-------------------------------------------------------------
//! @class   CounterService
//! @brief   カウンターサービス実装
//-------------------------------------------------------------
struct CounterService : public ICounterService {
	int value_ = 0;  // カウンターの内部状態

	//-------------------------------------------------------------
	//! @brief   カウンターをインクリメントする関数実装
	//-------------------------------------------------------------
    void increment() override { ++value_; }

	//-------------------------------------------------------------
	//! @brief   カウンターの現在値を取得する関数実装
	// @return  カウンターの現在値
	//-------------------------------------------------------------
    int getValue() const override { return value_; }
};

// エントリポイント
int main() {
	// コンテナ生成
    TsukinoDIContainer::Container container;

    // Singleton ライフサイクルで登録
    container.registerType<ICounterService, CounterService>(TsukinoDIContainer::Lifecycle::Singleton);

    // 1回目の解決
    auto counter1 = container.resolve<ICounterService>();
    counter1->increment();
    std::cout << "Counter1 value: " << counter1->getValue() << std::endl;

    // 2回目の解決（同じインスタンスが返る）
    auto counter2 = container.resolve<ICounterService>();
    counter2->increment();
    std::cout << "Counter2 value: " << counter2->getValue() << std::endl;

    // counter1とcounter2は同じインスタンスを指しているので値は共有される
    std::cout << "Counter1 value (after counter2 increment): " << counter1->getValue() << std::endl;
	// プログラムの終了
    return 0;
}
