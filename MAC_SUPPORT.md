# UltralightUE Mac 平台支持说明

## 概述

UltralightUE 插件现已支持 macOS 平台。Mac 版本的 Ultralight 1.4.0 库文件已包含在插件中。

## 文件位置

Mac 版本的 Ultralight 库文件位于：
```
Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/Mac/Release/
```

包含以下文件：
- `libAppCore.dylib` - AppCore 库
- `libWebCore.dylib` - WebCore 库
- `libUltralightCore.dylib` - UltralightCore 库
- `libUltralight.dylib` - Ultralight 主库

## 代码适配说明

### 已完成的适配工作

1. **库路径映射修正** (`UltralightUE.cpp`)
   - 修正了 Mac 平台的库文件路径映射
   - 添加了多路径查找逻辑（插件目录、模块目录、可执行文件目录）
   - 支持编辑器模式和打包模式

2. **AppCore 库支持**
   - 添加了 `AppCoreLibraryPath` 和 `AppCoreHandle` 成员变量
   - 实现了 AppCore 库的加载和卸载逻辑
   - AppCore 在 Mac/Linux 平台上是可选的，但推荐使用

3. **构建配置** (`UltralightUELibrary.Build.cs`)
   - Mac 平台的库链接配置已就绪
   - 运行时依赖项已正确配置
   - 资源文件（`cacert.pem`, `icudt67l.dat`）已包含；`inspector_resources.pak` 为可选（取决于 SDK 分发内容）

### 库加载顺序

插件会按以下顺序查找库文件：

1. **编辑器模式**：
   - `Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/Mac/Release/`

2. **打包模式**：
   - 模块目录（`FPlatformProcess::GetModulesDirectory()`）
   - 可执行文件目录（`FPlatformProcess::ExecutablePath()`）

## 使用方法

### 1. 启用插件

在 Unreal Editor 中：
1. 打开 **Edit → Plugins**
2. 搜索 "Ultralight"
3. 启用 **UltralightUE** 插件
4. 重启编辑器

### 2. 验证安装

启动项目后，检查 Output Log 中的 `LogUltralightUE` 类别，确认库文件已成功加载。

### 3. 使用 API

Mac 平台的 API 使用方式与 Windows 平台完全相同：

```cpp
// 获取子系统
UUltralightSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UUltralightSubsystem>();

// 创建视图
UUltralightView* View = Subsystem->CreateView(
    1920,                           // Width
    1080,                           // Height
    true,                           // Transparent
    TEXT("file:///index.html"),     // Initial URL
    nullptr                         // Auto-create render target
);
```

## 注意事项

1. **代码签名**：如果您的应用需要代码签名，请确保所有 dylib 文件都已正确签名。

2. **权限设置**：macOS 可能需要授予应用访问网络或文件系统的权限。

3. **架构支持**：当前支持 Apple Silicon (`arm64`) 架构。如果需要在 Intel Mac (`x86_64`) 上运行，需要提供对应的 `x86_64` 或 `universal2` 版本 Ultralight 库。

4. **资源文件**：确保以下资源文件可用：
   - `cacert.pem`
   - `icudt67l.dat` (在 resources 目录中)

## 故障排除

### 库文件未找到

如果遇到库文件加载失败：

1. 检查文件是否存在：
   ```bash
   ls -la Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/Mac/Release/
   ```

2. 检查文件权限：
   ```bash
   chmod +x Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/Mac/Release/*.dylib
   ```

3. 检查依赖关系：
   ```bash
   otool -L Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/Mac/Release/libWebCore.dylib
   ```

### 运行时错误

如果遇到运行时错误：

1. 查看 Output Log 中的详细错误信息
2. 检查 dylib 文件的架构是否匹配（x86_64 vs ARM64）
3. 确保所有依赖的库都已正确加载

## 版本信息

- **Ultralight 版本**: 1.4.0
- **插件版本**: 1.0
- **支持的 Unreal Engine 版本**: UE 5.3, UE 5.5+

## 相关链接

- [Ultralight 官方网站](https://ultralig.ht)
- [Ultralight 文档](https://docs.ultralig.ht)
- [UltralightUE GitHub](https://github.com/JailbreakPapa/UltralightUE)
