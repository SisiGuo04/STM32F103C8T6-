# 驱动介绍

## 1.OLED模块概述

0.96寸OLED显示屏驱动模块，基于STM32 HAL库开发，采用I2C通信协议。

### 硬件配置

#### 引脚连接

| OLED 引脚 | STM32 引脚 | 功能说明         |
|-----------|------------|------------------|
| VCC       | 3.3V       | 电源正极         |
| GND       | GND        | 电源地           |
| SCL       | PB6        | I2C1 时钟线      |
| SDA       | PB7        | I2C1 数据线      |

#### 技术参数

- **显示屏尺寸**：0.96 英寸  
- **分辨率**：128×64 像素  
- **通信接口**：I2C  
- **工作电压**：3.3V  

### 文件结构
```
oled_driver/
├── oled.c       // 驱动核心实现
├── oled.h       // 驱动接口声明
└── font.h       // 字库数据
```

### 依赖关系

- STM32 HAL 库  
- I2C1 外设初始化配置  

### 接口说明

#### 显示功能

- 字符显示  
- 中文显示  
- 数字各进制显示  
- 图像显示  

### 使用注意事项

1. 使用前需确保 I2C1 外设已正确初始化  
2. PB6/PB7 引脚应配置为复用功能模式  
3. 建议系统时钟不低于 8MHz

## 2.DHT11 温湿度传感器驱动说明

### 概述
本驱动提供了基于STM32 HAL库的DHT11温湿度传感器接口实现，包含传感器初始化、数据读取等功能，适用于嵌入式环境下的温湿度监测。

### 文件结构
- **dht11.h**  
  头文件定义硬件接口宏、数据类型及函数声明。
- **dht11.c**  
  源文件实现DHT11通信协议的核心功能。

### 硬件连接
DHT11传感器共有4个引脚：
| 引脚名称 | 连接说明                     |
|----------|------------------------------|
| VCC (3V3)| 接3.3V电源（支持3-5.5V范围） |
| GND      | 接地                         |
| DATA     | 接MCU的GPIO引脚（如PB12）    |
| NC       | 空引脚，无需连接             |

**注意事项**：
- DATA引脚需接**4.7KΩ上拉电阻**至VCC（若硬件未集成，需外接）
- 建议线长不超过20米以提高稳定性

### 主要功能
1. **初始化与检测**  
   - `DHT11_Init()`: 初始化GPIO并检测传感器是否存在。
   - `DHT11_Check()`: 验证传感器响应信号。

2. **数据读取**  
   - `DHT11_Read_Data(u8 *temp, u8 *humi)`: 读取温度（℃）和湿度（%RH），通过校验和验证数据有效性。
   - 支持逐位(`DHT11_Read_Bit`)和逐字节(`DHT11_Read_Byte`)读取原始数据。

3. **底层控制**  
   - `DHT11_Rst()`: 发送复位信号启动通信。
   - `DHT11_IO_OUT()`/`DHT11_IO_IN()`: 动态切换GPIO输入输出模式。

### 使用示例
```c
u8 temperature, humidity;
if (DHT11_Read_Data(&temperature, &humidity) == 0) {
    printf("Temperature: %d°C, Humidity: %d%%\n", temperature, humidity);
} else {
    printf("Sensor read error!\n");
}
