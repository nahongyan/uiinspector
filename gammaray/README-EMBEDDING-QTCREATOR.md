# 在 Qt Creator 插件中嵌入最小 GammaRay 客户端

本仓库已裁剪为“client + ui（无 feedback）”最小形态，并仅保留必需的 UI 插件白名单（modelinspector, widgetinspector, guisupport, eventmonitor, sysinfo, mimetypes, network, localeinspector）。Windows/MSVC 构建已验证可通过并生成运行时产物。

## 构建开关

- GAMMARAY_CLIENT_ONLY_BUILD=ON
- GAMMARAY_BUILD_UI=ON
- GAMMARAY_DISABLE_FEEDBACK=ON
- 可选：GAMMARAY_UI_PLUGIN_WHITELIST="modelinspector;widgetinspector;guisupport;eventmonitor;sysinfo;mimetypes;network;localeinspector"

## 运行时布局（Windows）

- bin/Release/
  - gammaray_client.dll
  - gammaray_ui-`[qt]`-MSVC-`[toolset]`-`[arch]`.dll
- plugins/gammaray/3.3/`[qt]`-MSVC-`[toolset]`-`[arch]`d/Release/
  - 各 UI 插件 dll（如 gammaray_modelinspector_ui.dll 等）

建议在 Qt Creator 插件安装目录中以一个固定根目录（如 `PluginInstallRoot/gammaray/`）打包上述结构，并在初始化时设置该根路径。

## 初始化与连接（示例）

下面示例展示在 Qt Creator 插件启动后启动并连接 GammaRay 客户端，使用内置 MainWindow（最省事）。若你希望将各工具嵌入 Qt Creator 面板，可在 ready() 之后改为创建自定义容器，而非调用 createMainWindow()。

```cpp
#include <ui/paths.h>
#include <client/clientconnectionmanager.h>

using namespace GammaRay;

void MyQtCreatorPlugin::startGammaRayClient()
{
    // 1) 设置运行时根路径，指向你随插件部署的 gammaray 目录
    //    例如：<PluginInstallRoot>/gammaray
    const QString gammaRayRoot = myResolvedGammaRayRoot();
    Paths::setRootPath(gammaRayRoot);

    // 2) 初始化一次注册项
    ClientConnectionManager::init();

    // 3) 创建连接管理器（不自动显示 Splash）
    auto *mgr = new ClientConnectionManager(this, /*showSplashScreenOnStartUp=*/false);

    // 4) 连接信号：就绪即创建内置主窗口（最简单的集成方式）
    connect(mgr, &ClientConnectionManager::ready, mgr, [mgr](){
        mgr->createMainWindow();
    });

    // 5) 连接错误与断开回调，根据 Qt Creator 的 UX 处理
    connect(mgr, &ClientConnectionManager::persistentConnectionError,
            this, [](const QString &msg){ /* 展示错误并记录日志 */ });
    connect(mgr, &ClientConnectionManager::disconnected,
            this, [](){ /* 根据需要清理 UI/状态 */ });

    // 6) 连接到目标探针（Probe）
    //    这里假定探针已在被调应用中运行并监听 TCP 端口
    // 将端口替换为实际探针监听端口
    const QUrl targetUrl = QUrl("tcp://127.0.0.1:PORT");
    mgr->connectToHost(targetUrl);
}
```

说明：

- Paths::setRootPath 必须在第一次使用前设置，以便发现 UI 插件等运行时资源。
- 若你已有探针发现机制（如外部启动器或工程设置记录端口），在这里传入对应的 URL 即可。
- 不使用内置 MainWindow 时，可以在 ready() 后构建自己的窗口/面板，核心是利用 ToolManager 暴露的工具模型与视图工厂（ToolUiFactory）。

## 在 Qt Creator 插件中链接

通常建议将 GammaRay 作为独立运行时随插件部署，而不是直接链接其库到 Qt Creator 插件中（以降低 ABI 风险与耦合）。如果你确需链接：

- 包含目录：
  - `GammaRayRoot/client`
  - `GammaRayRoot/common`
  - `GammaRayRoot/ui`
  - `GammaRayRoot/core`（仅含极简 shim 头，如 metaenum.h）
- 链接库（以构建产物为准）：
  - gammaray_client.dll
  - gammaray_ui-...dll
- 运行时需确保 plugins/gammaray/... 目录在 Paths::rootPath() 下可见。

## 常见问题

- 启动后无工具项：多半是 Paths::setRootPath 未正确指到包含 plugins/gammaray 的目录。
- MSVC 下链接不到 UI 符号：本仓库已通过 WHOLEARCHIVE 方式将 ui_internal 静态库强制链接到 gammaray_client，外部集成无需重复处理。
- 缺失 core 头：本最小化版本仅保留了必要的 shim（如 core/metaenum.h）。若你在自定义扩展里引用了被裁剪的头，请移除或补充最小 shim。

## 版本与 ABI

- 运行时代码会在 plugins/gammaray/3.3/`[abi]`/ 下查找 UI 插件，确保部署时目录结构一致。
- 若使用不同 Qt/编译器版本，请重新构建以匹配 ABI。
