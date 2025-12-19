# TsukinoDIContainer
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![C++17](https://img.shields.io/badge/C++-17-blue.svg)
初心者安心設計の C++ 用 **依存性注入 (DI) コンテナ** ライブラリです。 
ヘッダーオンリーで導入が簡便で軽量です。
「安全」「わかりやすい」ことを重視して設計されています。

---

## ✨ 特徴
- **ヘッダオンリー**で簡単導入
- **ライフサイクル管理**  
  - Transient / Singleton / Scoped をサポート
- **スレッドセーフ設計**  
  - `std::shared_mutex` による安全な並列解決
- **エラー処理が明示的**  
  - 未登録型 / 再登録 / 循環依存を例外で検出
- **初心者安心設計**  
  - チュートリアル用のサンプルコードを多数用意

---

## 📦 インストール

### CMakeでのビルド方法
プロジェクトに以下を追加してください：
```cmake
add_subdirectory(TsukinoDIContainer)
target_link_libraries(your_project PRIVATE TsukinoDIContainer)
```
### 🔨 g++ でのビルド方法

CMakeを使わずに直接コンパイルする場合は、以下のように `g++` を利用できます。

```bash
g++ -std=c++17 -Iinclude examples/example1_basic.cpp -o example1_basic
./example1_basic
```

### 🖥 Visual Studioでのビルド方法
- `TsukinoDIContainer.sln` を開くか、Visual Studio で「Open Folder」でプロジェクトルートを開く  
- [プロパティ] → [C/C++ → 全般 → 追加のインクルードディレクトリ] に `include/` を追加  
- スタートアッププロジェクトに `examples/example1_basic.cpp` を設定して `F5` 実行

### ヘッダオンリー利用
TsukinoDIContainer はヘッダオンリーライブラリなので、CMake を使わなくても  
`#include "TsukinoDIContainer.hpp"` を追加するだけで簡単に利用できます。

# 🚀使い方
**基本例 (example1_basic.cpp)**
```cpp
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
```
```bash
[Console] Hello, TsukinoDIContainer!
```
# 📚 サンプル一覧
``examples/``ディレクトリにチュートリアルコードを用意しています。

| ファイル名                  | 内容説明                                   |
|-----------------------------|--------------------------------------------|
| example1_basic.cpp          | Transient の最小例                         |
| example2_singleton.cpp      | Singleton の挙動                           |
| example3_scoped.cpp         | Scoped の挙動                              |
| example4_error.cpp          | 未登録 / 再登録 / 循環依存のエラー体験     |
| example5_threadsafe.cpp     | マルチスレッドでの安全性                   |
| example6_scoped_multithread.cpp | Scoped + マルチスレッド                |
| example7_registerCtor.cpp   | 依存関係付き登録のチュートリアル           |
| example8_mock.cpp           | テスト用モック注入の例                     |

# 🛡 ライセンス
[MIT License](./LICENSE)
自由に利用・改変・再配布可能です。

# 👩‍💻 作者
**山﨑 愛**
- [Qiita: tsukino_](https://qiita.com/tsukino_) 
- [GitHub: tsukino](https://github.com/tsukinokun)