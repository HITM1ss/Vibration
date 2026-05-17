# OLED_Timer

基于 **STM32F103C8T6** 的 OLED 多功能定时器，支持 SSD1327 128×128 OLED 显示屏。

## 硬件

- MCU: STM32F103C8T6 (Cortex-M3)
- 显示: SSD1327 OLED 128×128 SPI 接口
- 传感器: MPU6050 (陀螺仪/加速度计)、AHT20 (温湿度)
- 存储: W25Qxx Flash

## 功能

- 定时器显示（倒计时/正计时）
- 温湿度监测
- 姿态显示（MPU6050）
- 数据存储 (W25Qxx)

## 构建

```bash
make
```

烧录使用 OpenOCD:
```bash
make flash
```

## 项目结构

```
Core/           # STM32 HAL 核心代码
Drivers/        # 外设驱动 (HAL, CMSIS)
user/           # 用户代码
  ├── oled/     # u8g2 OLED 驱动
  ├── mpu6050/  # MPU6050 传感器
  ├── aht20/    # AHT20 温湿度传感器
  └── w25qxx/   # W25Qxx Flash 存储
OLED_Timer.ioc  # STM32CubeMX 配置文件
Makefile        # 构建配置
```

## 文档

- `OLED_SSD1327_128x128_GUIDE.md` - OLED 驱动说明
- `BUILD_AND_FLASH.md` - 编译烧录指南