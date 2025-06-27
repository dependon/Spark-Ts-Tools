# 多平台编译说明

本项目已配置GitHub Actions自动化构建，支持以下平台：

- **Windows** (windows-latest)
- **Linux** (ubuntu-latest) 
- **macOS** (macos-latest)

## 自动构建

当代码推送到 `main` 或 `master` 分支，或者创建Pull Request时，GitHub Actions会自动触发构建。

### 构建产物

构建完成后，可以在GitHub Actions页面下载对应平台的可执行文件：

- `spark-ts-tools-windows` - Windows可执行文件及依赖
- `spark-ts-tools-linux` - Linux可执行文件
- `spark-ts-tools-macos` - macOS可执行文件

## 本地构建

### 前置要求

- Qt 5.15.2 或更高版本
- 对应平台的C++编译器
  - Windows: MSVC 2019或更新版本
  - Linux: GCC
  - macOS: Xcode Command Line Tools

### 构建步骤

1. 克隆仓库：
```bash
git clone <repository-url>
cd Spark-Ts-Tools
```

2. 生成Makefile：
```bash
qmake Spark-Ts-Tools.pro CONFIG+=release
```

3. 编译：

**Linux/macOS:**
```bash
make -j$(nproc)
```

**Windows:**
```cmd
nmake
```

4. 可执行文件将生成在 `bin/` 目录下。

## 项目结构

- `Libraries/` - 项目依赖库
  - `LibXlsxRW/` - Excel读写库
- `MainApps/Spark-Ts-Tools/` - 主应用程序
- `ssl/` - SSL库文件（Windows）
- `.github/workflows/build.yml` - GitHub Actions配置

## 注意事项

- Windows版本会自动包含Qt依赖和SSL库
- Linux和macOS版本需要目标系统安装Qt运行时
- 所有平台都使用Release配置进行优化编译