# 翻译文件（.ts）和Excel文件的相互转换工具(百度api版)

## 克隆和修改来源

源代码目录: https://github.com/Longxr/TSFileEditor/

## 运行环境
Win10 Qt5.12.4 Mingw32 && MSVC VS2017 (32位)，64位需要自行放置64位的libssl和libcrypto的库
linux 的qt版本应该随意

## 依赖库
[qtxlsx](https://qtxlsx.debao.me/)

## 主要功能
对Qt的ts翻译文件进行翻译输出到xlsx文件，再从xlsx文件翻译输出到原ts文件，需要有百度翻译的api
对于原来作者的修改:从翻译到第二列，默认改到了第三列(因为第三列才是输出到ts的位置，不再需要手动移植)，有道api换为了百度api，支持一键对28国家翻译文件进行翻译(不对已存在翻译进行修改)，批量处理升级，
翻译会默认输出到ts文件，请在使用前备份好之前的ts文件，以防万一
