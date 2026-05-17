# SSD1327 128x128 OLED 驱动指南

## 一、硬件连接

### 1.1 OLED模块引脚说明

| OLED引脚 | 引脚名称 | 功能说明 |
|---------|---------|---------|
| 1 | VCC | 电源正（3.3V） |
| 2 | GND | 电源地 |
| 3 | SCK | SPI时钟线 |
| 4 | SDA | SPI数据线（MOSI） |
| 5 | DC | 数据/命令选择 |
| 6 | CS | 片选（低电平有效） |
| 7 | RST | 复位（低电平有效） |

### 1.2 STM32F103C8T6 引脚连接

| OLED引脚 | STM32引脚 | 引脚功能 |
|---------|----------|---------|
| VCC | 3.3V | 电源 |
| GND | GND | 地 |
| SCK | PA5 | SPI1_SCK |
| SDA | PA7 | SPI1_MOSI |
| DC | PA4 | GPIO输出 |
| CS | PB0 | GPIO输出 |
| RST | PB1 | GPIO输出 |

### 1.3 接线示意图

```
OLED模块          STM32F103C8T6
┌─────────┐       ┌─────────────┐
│ VCC     │───────│ 3.3V        │
│ GND     │───────│ GND         │
│ SCK     │───────│ PA5         │
│ SDA     │───────│ PA7         │
│ DC      │───────│ PA4         │
│ CS      │───────│ PB0         │
│ RST     │───────│ PB1         │
└─────────┘       └─────────────┘
```

## 二、驱动过程

### 2.1 驱动架构

本项目使用 **U8G2 库** 作为 OLED 显示驱动，采用 **软件SPI** 方式通信。

### 2.2 关键文件说明

| 文件路径 | 功能说明 |
|---------|---------|
| `user/oled/u8g2_hal.c` | U8G2 HAL适配层，实现GPIO控制和延时回调 |
| `user/oled/u8x8_byte.c` | 软件SPI字节传输实现 |
| `user/oled/u8x8_d_ssd1327.c` | SSD1327显示控制器驱动 |
| `Core/Src/main.c` | 主程序，包含初始化和显示逻辑 |

### 2.3 初始化流程

```c
// 1. 初始化GPIO（普通IO配置）
MX_GPIO_Init();

// 2. 初始化SPI（会将PA5/PA7配置为复用功能）
MX_SPI1_Init();

// 3. 重新配置PA5/PA7为普通GPIO（软件SPI需要）
GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// 4. OLED硬件复位
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_Delay(10);
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
HAL_Delay(10);

// 5. U8G2初始化
u8g2_Setup_ssd1327_ws_128x128_f(&u8g2, U8G2_R0, 
                                  u8x8_byte_4wire_sw_spi,  // 软件SPI传输
                                  u8x8_gpio_and_delay_stm32f1); // GPIO控制
u8g2_InitDisplay(&u8g2);
u8g2_SetPowerSave(&u8g2, 0);
u8g2_ClearBuffer(&u8g2);

// 6. 显示内容
u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
u8g2_DrawStr(&u8g2, 0, 20, "SSD1327 OLED");
u8g2_SendBuffer(&u8g2);
```

### 2.4 U8G2 HAL适配层实现

```c
// GPIO映射定义
#define OLED_CS_PIN    GPIO_PIN_0
#define OLED_CS_PORT   GPIOB
#define OLED_DC_PIN    GPIO_PIN_4
#define OLED_DC_PORT   GPIOA
#define OLED_RST_PIN   GPIO_PIN_1
#define OLED_RST_PORT  GPIOB
#define OLED_SCK_PIN   GPIO_PIN_5
#define OLED_SCK_PORT  GPIOA
#define OLED_SDA_PIN   GPIO_PIN_7
#define OLED_SDA_PORT  GPIOA

// GPIO控制回调函数
uint8_t u8x8_gpio_and_delay_stm32f1(u8x8_t *u8x8, uint8_t msg, 
                                     uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_CS:
            HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, 
                             arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_DC:
            HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, 
                             arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_RESET:
            HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, 
                             arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_SPI_CLOCK:
            HAL_GPIO_WritePin(OLED_SCK_PORT, OLED_SCK_PIN, 
                             arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_SPI_DATA:
            HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, 
                             arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        // ...其他case
    }
    return 1;
}
```

## 三、常见问题排查

### 3.1 OLED不显示

**可能原因及解决方法：**

| 现象 | 可能原因 | 解决方法 |
|-----|---------|---------|
| 屏幕完全不亮 | 电源未接或电压不对 | 确认VCC接3.3V，GND接地 |
| 屏幕不显示但有微弱亮度 | 复位时序不正确 | 检查RST引脚连接和复位代码 |
| 屏幕不显示 | CS电平逻辑错误 | 确认CS为低电平有效，检查`u8g2_hal.c`中CS配置 |
| 屏幕不显示 | SPI引脚配置错误 | 确认PA5/PA7在SPI初始化后重新配置为GPIO输出 |
| 显示乱码 | SPI时序不匹配 | 使用软件SPI方式，确保时序正确 |

### 3.2 关键注意事项

1. **SPI引脚复用问题**：`MX_SPI1_Init()`会将PA5/PA7配置为SPI复用功能，软件SPI需要在其后重新配置为普通GPIO输出。

2. **CS电平逻辑**：OLED的CS引脚为低电平有效，`u8x8_gpio_and_delay_stm32f1`中`U8X8_MSG_GPIO_CS`的处理应为：
   - `arg_int=1`（chip_disable）→ CS输出高电平（GPIO_PIN_SET）
   - `arg_int=0`（chip_enable）→ CS输出低电平（GPIO_PIN_RESET）

3. **复位时序**：OLED上电后需要至少10ms的复位时间，确保RST引脚先拉低再拉高。

## 四、编译与烧录

### 4.1 编译命令

```bash
make clean && make
```

### 4.2 烧录命令

```bash
make flash
```

### 4.3 OpenOCD配置

确保`stm32f103c8t6_opencd.cfg`配置正确：

```
source [find interface/stlink.cfg]
source [find target/stm32f1x.cfg]

reset_config srst_only srst_nogate
```

## 五、附录

### 5.1 U8G2库文件列表

项目中使用的U8G2核心文件：

- `u8g2.h` - U8G2主头文件
- `u8g2_setup.c` - U8G2初始化函数
- `u8g2_d_setup.c` - 显示设备初始化
- `u8g2_buffer.c` - 显示缓冲区管理
- `u8g2_font.c` - 字体处理
- `u8x8.h` - u8x8底层驱动接口
- `u8x8_setup.c` - u8x8初始化
- `u8x8_byte.c` - 字节传输（软件SPI）
- `u8x8_cad.c` - 命令/数据传输
- `u8x8_d_ssd1327.c` - SSD1327控制器驱动
- `u8x8_gpio.c` - GPIO抽象层

### 5.2 参考链接

- U8G2官方仓库：https://github.com/olikraus/u8g2
- SSD1327数据手册：https://www.solomon-systech.com/en/product/oled-display-driver-ics/ssd1327/
