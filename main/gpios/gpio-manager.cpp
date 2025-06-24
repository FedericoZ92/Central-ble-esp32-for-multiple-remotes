
#include "gpios/gpio-manager.h"
#include "debug.h"

//gpio
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/rtc_io.h>
#include "gpios/input-gpio.h"
#include "gpios/output-gpio.h"
//adc
#include "driver/adc.h"

GpioBase* GpioManager::get(gpio_num_t number)
{
    for (auto* gpio : gpios){
        if (gpio->getNumber() == number){
            return gpio;    
        }
    }
    return nullptr;
}

void GpioManager::configure()
{
    add(&gpioEna); 
    V(add(&gpioPosi));    
    V(add(&gpioDispSda));    
    V(add(&gpioDispScl));
    V(add(&gpioRst));    
    V(add(&gpioCs));    
    V(add(&gpioBkl));   
    add(&gpioBatt);   
    add(&gpioNtc);      
    V(add(&gpioScroll));
    add(&gpioOnSw);  
    add(&gpioRgbLedGreen);   
    add(&gpioRgbLedRed);   
    add(&gpioRgbLedBlue); 
    add(&gpioLedWhite); 
    V(add(&gpioLedGreen));    
    add(&gpioFlashChipSelect);   
    add(&gpioBuzzerPwm); 

    ESP_LOGI(GPIO_TAG, "GpioManager init");
    configure_ON_ENA();
    enable_ON_ENA();
    V(configure_posifa_i2c());
    V(configure_ON_POSIFA());
    V(enable_ON_POSIFA());
    
    V(configure_display());
    
    V(configure_SCROLL_BUTTON_GPIO()); 
    configure_ON_SW_BUTTON();
    
    configure_vinCheck();
    configure_ainOrNtcCheck();  
    unallowAdcReading();   
    configure_battery();
    configure_ntc();

    disable_rgb_green_led();  
    configure_rgb_green_led();  
    disable_rgb_red_led(); 
    configure_rgb_red_led(); 
    disable_rgb_blue_led(); 
    configure_rgb_blue_led(); 
    disable_white_led();
    configure_white_led();
    disable_green_led();
    V(configure_green_led());  

    configure_flash(); 
    enable_flash();

    initOthers();
}

void GpioManager::configure_ON_ENA() const
{
    ESP_LOGI(GPIO_TAG, "Enable ON_ENA");     
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;      // Disable interrupts
    io_conf.mode = GPIO_MODE_OUTPUT;            // Set as output mode
    io_conf.pin_bit_mask = (1ULL << ON_ENA);    // Specify the pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  // Disable pull-down resistor
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      // Disable pull-up resistor
    gpio_config(&io_conf);                         // Apply the configuration
}

void GpioManager::enable_ON_ENA()
{
    ESP_LOGI(GPIO_TAG, "Enable ON_ENA");    
    static_cast<GpioOutput*>(get(ON_ENA))->setOutputLevel(1);  
}

void GpioManager::shutdown()
{
    ESP_LOGI(GPIO_TAG, "Disable ON_ENA");    
    static_cast<GpioOutput*>(get(ON_ENA))->setOutputLevel(0);  
}

void GpioManager::configure_posifa_i2c() const
{
    ESP_LOGI(GPIO_TAG, "Init I2C_MASTER_SDA_IO and I2C_MASTER_SDA_IO");    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT; // Set as output initially
    io_conf.pin_bit_mask = (1ULL << I2C_MASTER_SDA_IO) | (1ULL << I2C_MASTER_SCL_IO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);   
}

void GpioManager::configure_ON_POSIFA() const
{
    ESP_LOGI(GPIO_TAG, "Configure ON_POSIFA");     
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;        // Disable interrupts
    io_conf.mode = GPIO_MODE_OUTPUT;              // Set as output mode
    io_conf.pin_bit_mask = (1ULL << ON_POSIFA);   // Specify the pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Disable pull-down resistor
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Disable pull-up resistor
    gpio_config(&io_conf);                        // Apply the configuration
    esp_rom_gpio_pad_select_gpio(ON_POSIFA);
}

void GpioManager::enable_ON_POSIFA() 
{
    ESP_LOGI(GPIO_TAG, "Enable ON_POSIFA");     
    static_cast<GpioOutput*>(get(ON_POSIFA))->setOutputLevel(1); 
}

void GpioManager::disable_ON_POSIFA() 
{
    ESP_LOGI(GPIO_TAG, "Disable ON_POSIFA");    
    static_cast<GpioOutput*>(get(ON_POSIFA))->setOutputLevel(0); 
}

void GpioManager::configure_display() const
{
    configure_display_i2c();
    configure_display_backlight();
    configure_display_reset();
    configure_display_chip_select();
}

void GpioManager::enable_display() 
{
    enable_display_backlight();
    enable_display_reset();
    enable_display_chip_select();
}

void GpioManager::disable_display() 
{
    disable_display_backlight();
    disable_display_reset();
    disable_display_chip_select();
}

void GpioManager::configure_display_i2c() const
{
    ESP_LOGI(GPIO_TAG, "Configure I2C_DISP_SDA and I2C_DISP_SCL");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << I2C_DISP_SDA) | (1ULL << I2C_DISP_SCL);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);           
}

void GpioManager::configure_display_chip_select() const
{
    ESP_LOGI(GPIO_TAG, "Configure DISP_CS");     
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;    
    io_conf.mode = GPIO_MODE_OUTPUT;          
    io_conf.pin_bit_mask = (1ULL << DISP_CS);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void GpioManager::enable_display_chip_select()
{
    ESP_LOGI(GPIO_TAG, "Enable DISP_CS"); 
    static_cast<GpioOutput*>(get(DISP_CS))->setOutputLevel(0); //chip select basso 
}

void GpioManager::disable_display_chip_select()
{
    ESP_LOGI(GPIO_TAG, "Disable DISP_CS");
    // unused
}

void GpioManager::configure_display_backlight() const
{
    ESP_LOGI(GPIO_TAG, "Configure DISP_BKL_EN"); 
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << DISP_BKL_EN); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
}

void GpioManager::enable_display_backlight() 
{
    ESP_LOGI(GPIO_TAG, "Enable DISP_BKL_EN");
    static_cast<GpioOutput*>(get(DISP_BKL_EN))->setOutputLevel(1);
}

void GpioManager::disable_display_backlight() 
{
    ESP_LOGI(GPIO_TAG, "Disable DISP_BKL_EN");
    static_cast<GpioOutput*>(get(DISP_BKL_EN))->setOutputLevel(0); 
}

void GpioManager::configure_display_reset() const
{
    ESP_LOGI(GPIO_TAG, "Configure DISP_RST");  
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;    
    io_conf.mode = GPIO_MODE_OUTPUT;          
    io_conf.pin_bit_mask = (1ULL << DISP_RST);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void GpioManager::enable_display_reset()
{
    ESP_LOGI(GPIO_TAG, "Enable DISP_RST");      
    static_cast<GpioOutput*>(get(DISP_RST))->setOutputLevel(1);
}

void GpioManager::disable_display_reset()
{
    ESP_LOGI(GPIO_TAG, "Disable DISP_RST");     
    static_cast<GpioOutput*>(get(DISP_RST))->setOutputLevel(1);
}

void GpioManager::configure_SCROLL_BUTTON_GPIO() const
{
    ESP_LOGI(GPIO_TAG, "Enable SCROLL_BUTTON_GPIO");      
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;      
    io_conf.mode = GPIO_MODE_INPUT;            
    io_conf.pin_bit_mask = (1ULL << SCROLL_BUTTON_GPIO); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
    // Clean and validate interrupt status before enabling
    //gpio_set_intr_type(SCROLL_BUTTON_GPIO, GPIO_INTR_DISABLE);  // Disable interrupts (safe equivalent)
    //gpio_set_intr_type(SCROLL_BUTTON_GPIO, GPIO_INTR_ANYEDGE);  // Re-enable with desired mode
}

void GpioManager::configure_ON_SW_BUTTON() const
{
    ESP_LOGI(GPIO_TAG, "Enable ON_SW_BUTTON"); 
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;      
    io_conf.mode = GPIO_MODE_INPUT;            
    io_conf.pin_bit_mask = (1ULL << ON_SW_BUTTON); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
    // Clean and validate interrupt status before enabling
    //gpio_set_intr_type(ON_SW_BUTTON, GPIO_INTR_DISABLE);  // Disable interrupts (safe equivalent)
    //gpio_set_intr_type(ON_SW_BUTTON, GPIO_INTR_ANYEDGE);  // Re-enable with desired mode
}

void GpioManager::configure_vinCheck()
{
    ESP_LOGI(GPIO_TAG, "Init VIN_CHECK");    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << VIN_CHECK); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
}

void GpioManager::configure_ainOrNtcCheck()
{
    ESP_LOGI(GPIO_TAG, VT("Init AIN_CHECK", "Init NTC_CHECK"));    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << VT(AIN_CHECK, NTC_CHECK)); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
}

void GpioManager::allowAdcReading()
{
    ESP_LOGI(GPIO_TAG, VT("Enable VIN_CHECK and AIN_check", "Enable VIN_CHECK and NTC_CHECK"));   
    static_cast<GpioOutput*>(get(VIN_CHECK))->setOutputLevel(1); 
    static_cast<GpioOutput*>(get(VT(AIN_CHECK, NTC_CHECK)))->setOutputLevel(1); 
}

void GpioManager::unallowAdcReading()
{
    ESP_LOGI(GPIO_TAG, VT("Disable VIN_CHECK and AIN_check", "Disable VIN_CHECK and NTC_CHECK"));    
    static_cast<GpioOutput*>(get(VIN_CHECK))->setOutputLevel(0); 
    static_cast<GpioOutput*>(get(VT(AIN_CHECK, NTC_CHECK)))->setOutputLevel(0); 
}

void GpioManager::configure_battery() const
{
    ESP_LOGI(GPIO_TAG, "Configure battery");  
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(VIN_MEAS, VT(ADC_ATTEN, ADC_ATTEN_BATT_TEMP_SENSOR));
}

void GpioManager::configure_ntc() const
{
    ESP_LOGI(GPIO_TAG, "Configure ntc");
    VT(;, adc1_config_width(ADC_WIDTH));
    VT(;, adc1_config_channel_atten(VREF_REAL, ADC_ATTEN_VREF));  
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(VT(NTC_BOARD, NTC_EXT), ADC_ATTEN);  
}

void GpioManager::configure_rgb_green_led() const
{
    ESP_LOGI(GPIO_TAG, "Configure rgb green led");    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pin_bit_mask = (1ULL << LED_RGB_G);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_LOGI(GPIO_TAG, "Configured rgb green led - 2");
    gpio_config(&io_conf);  
    ESP_LOGI(GPIO_TAG, "Configured rgb green led");  
}

void GpioManager::configure_rgb_red_led() const
{
    ESP_LOGI(GPIO_TAG, "Configure rgb red led");    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pin_bit_mask = (1ULL << LED_RGB_R);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);  
}

void GpioManager::configure_rgb_blue_led() const
{
    ESP_LOGI(GPIO_TAG, "Configure rgb blue led");        
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pin_bit_mask = (1ULL << LED_RGB_B);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);      
}

void GpioManager::configure_white_led() const
{
    ESP_LOGI(GPIO_TAG, "Configure white led");        
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pin_bit_mask = (1ULL << LED_W);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);           
}

void GpioManager::configure_green_led() const
{
    ESP_LOGI(GPIO_TAG, "Configure green led");        
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pin_bit_mask = (1ULL << LED_G);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);  
           
}

void GpioManager::disable_rgb_green_led()
{
    gpio_set_level(LED_RGB_G, 1);
}

void GpioManager::disable_rgb_red_led()
{
    gpio_set_level(LED_RGB_R, 1);    
}

void GpioManager::disable_rgb_blue_led()
{
    gpio_set_level(LED_RGB_B, 1); 
}

void GpioManager::disable_white_led()
{
    gpio_set_level(LED_W, 1); 
}

void GpioManager::disable_green_led()
{
    gpio_set_level(LED_G, 1); 
}

void GpioManager::configure_flash() const
{
    ESP_LOGI(GPIO_TAG, "Configure FLASH_SPI_CS"); 
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_SPI_CS); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
}

void GpioManager::enable_flash()
{
    ESP_LOGI(GPIO_TAG, "Enable FLASH_SPI_CS"); 
    static_cast<GpioOutput*>(get(FLASH_SPI_CS))->setOutputLevel(0);  
}

void GpioManager::disable_flash()
{
    ESP_LOGI(GPIO_TAG, "Disable FLASH_SPI_CS");     
    static_cast<GpioOutput*>(get(FLASH_SPI_CS))->setOutputLevel(1);
}

void GpioManager::initOthers()
{
    gpio_config_t io_conf;

    ESP_LOGI(GPIO_TAG, "Configure FLASH_SPI_CS"); 
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_SPI_HOLD); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
     
    ESP_LOGI(GPIO_TAG, "Configure FLASH_SPI_DI");      
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_SPI_DI); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 

    ESP_LOGI(GPIO_TAG, "Configure FLASH_SPI_CLK");     
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_SPI_CLK); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 

    ESP_LOGI(GPIO_TAG, "Configure FLASH_DO"); 
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_DO); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 
    
    ESP_LOGI(GPIO_TAG, "Configure FLASH_WP");    
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << FLASH_WP); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 

    ESP_LOGI(GPIO_TAG, "Configure IO0");       
    io_conf.intr_type = GPIO_INTR_ANYEDGE;      
    io_conf.mode = GPIO_MODE_INPUT;            
    io_conf.pin_bit_mask = (1ULL << IO0); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      
    gpio_config(&io_conf); 
    
    ESP_LOGI(GPIO_TAG, "Configure ADC_DRDY");      
    io_conf.intr_type = GPIO_INTR_ANYEDGE;      
    io_conf.mode = GPIO_MODE_INPUT;            
    io_conf.pin_bit_mask = (1ULL << ADC_DRDY); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      
    gpio_config(&io_conf); 
     
    ESP_LOGI(GPIO_TAG, "Configure FLASH_SPI_CS"); 
    io_conf.intr_type = GPIO_INTR_DISABLE;      
    io_conf.mode = GPIO_MODE_OUTPUT;            
    io_conf.pin_bit_mask = (1ULL << BUZZER_PWM); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      
    gpio_config(&io_conf); 

    ESP_LOGI(GPIO_TAG, "Set FLASH_SPI_CS"); 
    static_cast<GpioOutput*>(get(BUZZER_PWM))->setOutputLevel(0); 
}

void GpioManager::add(GpioBase* gpio)
{
    gpios.push_back(gpio);        
}
