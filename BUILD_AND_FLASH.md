
# STM32F103C8T6 编译与烧录指南

## 环境要求

### 1. ARM GCC 工具链
- 版本要求：GCC 10.x 或更高版本
- 下载地址：https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-arm-embedded
- 安装后需将工具链 `bin` 目录添加到系统环境变量 `PATH`

### 2. MinGW Make
- 用于执行 Makefile
- 通常随 MinGW 安装包提供

### 3. OpenOCD（用于烧录）
- 用于通过 ST-Link 烧录程序
- 下载地址：https://openocd.org/

## 编译步骤

### 1. 打开命令行终端
- 进入工程根目录：
```bash
cd e:\Github\STM32F103C8T6
```

### 2. 执行编译
```bash
mingw32-make
```

### 3. 编译成功标志
编译成功后会在 `build` 目录生成以下文件：
- `STM32f103c8t6.elf` - 可执行文件
- `STM32f103c8t6.hex` - Intel HEX 格式（用于烧录）
- `STM32f103c8t6.bin` - 二进制格式（用于烧录）

### 4. 清理编译产物
```bash
mingw32-make clean
```

## 烧录步骤

### 1. 硬件连接
- 将 ST-Link 调试器连接到电脑 USB 口
- 将 ST-Link 的 SWD 接口连接到 STM32F103C8T6：
  - ST-Link SWDIO → STM32 PA13
  - ST-Link SWCLK → STM32 PA14
  - ST-Link GND → STM32 GND
  - ST-Link 3.3V → STM32 3.3V（可选，若目标板有独立电源则无需连接）

### 2. 执行烧录
```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/STM32f103c8t6.elf verify reset exit"
```

### 3. 烧录成功标志
```
** Programming Started **
Info : device id = 0x00006410
Info : flash size = 64 KiB
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
```

## 常见问题

### 1. 编译失败 - READONLY 关键字错误
**现象**：
```
ld.exe: STM32F103XX_FLASH.ld:105: non constant or forward reference address expression for section .ARM.extab
```

**原因**：链接脚本中使用了 `READONLY` 关键字，该特性仅在 GCC 11+ 支持。

**解决方案**：
编辑 `STM32F103XX_FLASH.ld` 文件，移除所有段定义中的 `(READONLY)` 关键字（共5处）。

### 2. 烧录失败 - 无法连接目标
**检查项**：
- ST-Link 驱动是否正确安装
- SWD 接线是否正确
- 目标板电源是否正常
- 尝试降低烧录速度：在命令中添加 `-c "adapter speed 100"`

### 3. 工具未找到
**现象**：
```
'xxx' 不是内部或外部命令，也不是可运行的程序或批处理文件。
```

**解决方案**：
确保工具链的 `bin` 目录已添加到系统环境变量 `PATH` 中。

## 快捷命令汇总

> **注意**：本工程使用 PowerShell 环境，**不支持使用 `&&` 连接多个命令**，必须分开执行。

```bash
# 编译
mingw32-make

# 清理
mingw32-make clean

# 烧录（分两步执行）
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init" -c "reset halt" -c "program build/STM32f103c8t6.elf verify reset exit"
```
