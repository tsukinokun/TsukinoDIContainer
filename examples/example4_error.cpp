//-------------------------------------------------------------
//! @file   example4_error.cpp
//! @brief  TsukinoDIContainer エラー処理使用例
//! @author 山﨑愛 ( Qiita:tsukino_   github:tsukino)
//-------------------------------------------------------------
#include <iostream>
#include <TsukinoDIContainer/TsukinoDIContainer.hpp>

//-------------------------------------------------------------
//! @class   IA
//! @brief   循環依存テスト用インターフェース A
//-------------------------------------------------------------
struct IA { virtual ~IA() = default; };

//-------------------------------------------------------------
//! @class   IB
//! @brief   循環依存テスト用インターフェース B
//-------------------------------------------------------------
struct IB { virtual ~IB() = default; };

//-------------------------------------------------------------
//! @class   AImpl
//! @brief   IA の実装（IB に依存）
//-------------------------------------------------------------
struct AImpl : public IA {
    explicit AImpl(std::shared_ptr<IB>) {}
};

//-------------------------------------------------------------
//! @class   BImpl
//! @brief   IB の実装（IA に依存）
//-------------------------------------------------------------
struct BImpl : public IB {
    explicit BImpl(std::shared_ptr<IA>) {}
};

//-------------------------------------------------------------
//! @class   DummyService
//! @brief   依存なしのダミーサービス（再登録テスト用）
//-------------------------------------------------------------
struct DummyService {
    DummyService() {} // デフォルトコンストラクタあり
};

// エントリポイント
int main() {
	// コンテナ生成
    TsukinoDIContainer::Container container;

    // 1. 未登録型を解決しようとした場合
    try {
        auto unknown = container.resolve<int>();
    }
    catch (const TsukinoDIContainer::ResolveException& ex) {
        std::cerr << "[Error] " << ex.what() << std::endl;
    }

    // 2. 既に登録済みの型を再登録しようとした場合
    try {
        container.registerType<DummyService, DummyService>();
        container.registerType<DummyService, DummyService>(); // 2回目はエラー
    }
    catch (const TsukinoDIContainer::ResolveException& ex) {
        std::cerr << "[Error] " << ex.what() << std::endl;
    }

    // 3. 循環依存を解決しようとした場合
    try {
        container.replaceCtor<IA, AImpl, IB>();
        container.replaceCtor<IB, BImpl, IA>();
        auto a = container.resolve<IA>(); // 循環依存でエラー
    }
    catch (const TsukinoDIContainer::ResolveException& ex) {
        std::cerr << "[Error] " << ex.what() << std::endl;
    }
	// プログラムの終了
    return 0;
}
