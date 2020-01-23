#include "radio.h"

#include <string.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

#include "axradio/ax5043.h"
#include "axradio/axradio.h"
#include "bcm2835/bcm2835.h"
#include "libmf/libmfwtimer.h"
#include "nbfi/nbfi_config.h"
#include "nbfi/nbfi.h"
#include "nbfi/rf.h"

#define MODEM_ID 0x77CCD8
const uint32_t KEY[8] = {0x1DF3F6B5, 0x447AC18D, 0x64ED540B, 0x66FE147E, 0xB670BA41, 0x8916C02D, 0x68237B49, 0xF9A7BA62};

#if BAND == UL868800_DL446000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL868800_DL864000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       864000000
#elif BAND == UL868800_DL446000_DL864000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       864000000
#elif BAND == UL867950_DL446000
#define NBFI_UL_FREQ_BASE       (867950000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL868500_DL446000
#define NBFI_UL_FREQ_BASE       (868500000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL868100_DL446000
#define NBFI_UL_FREQ_BASE       (868100000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL864000_DL446000
#define NBFI_UL_FREQ_BASE       (864000000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL863175_DL446000
#define NBFI_UL_FREQ_BASE       (863175000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND == UL864000_DL875000
#define NBFI_UL_FREQ_BASE       (864000000 - 25000)
#define NBFI_DL_FREQ_BASE       875000000
#elif BAND == UL458550_DL453750
#define NBFI_UL_FREQ_BASE       (458550000 - 25000)
#define NBFI_DL_FREQ_BASE       453750000
#elif BAND == UL916500_DL902900
#define NBFI_UL_FREQ_BASE       (916500000 - 25000)
#define NBFI_DL_FREQ_BASE       902900000
#endif

const nbfi_settings_t nbfi_set_default =
{
	CRX,				//	mode;
	UL_DBPSK_3200_PROT_D,	//	tx_phy_channel;
	DL_PSK_500,			//	rx_phy_channel;
	HANDSHAKE_SIMPLE,
	MACK_1,				//	mack_mode
	1,					//	num_of_retries;
	8,					//	max_payload_len;
	{0},				//	dl_ID[3];
	{0},				//	temp_ID[3];
	{0xFF,0,0},			//	broadcast_ID[3];
	{0},				//	full_ID[6];
	0,					//	tx_freq;
	0,					//	rx_freq;
	PCB,				//	tx_antenna;
	PCB,				//	rx_antenna;
	TX_MAX_POWER,		//	tx_pwr;
	10,//60 * 24 * 7,	//	heartbeat_interval
	255,				//	heartbeat_num
	NBFI_FLG_FIXED_BAUD_RATE,					//	additional_flags
	NBFI_UL_FREQ_BASE,
	NBFI_DL_FREQ_BASE
};

std::mutex spi_lock;
static char spi_buf[UINT8_MAX];

void delay_ms(uint16_t ms)
{
	usleep(ms * 1000);
}

uint64_t get_time_chrono(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void dummy(void){}
void dummy_uint8(uint8_t some){}

uint8_t get_irq_pin_state(void)
{
	std::cout << "getted irq level " << (int)bcm2835_gpio_lev(AX_IRQ_PIN) << std::endl;
	return bcm2835_gpio_lev(AX_IRQ_PIN);
}

void spi_rx(uint8_t *rx_buf, uint16_t len)
{
	std::lock_guard<std::mutex> lock(spi_lock);
	bcm2835_spi_transfernb(spi_buf, (char *)rx_buf, len);
}

void spi_tx(uint8_t *tx_buf, uint16_t len)
{
	std::lock_guard<std::mutex> lock(spi_lock);
	bcm2835_spi_transfernb((char *)tx_buf, spi_buf, len);
}

void spi_tx_rx(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t len)
{
	std::lock_guard<std::mutex> lock(spi_lock);
	bcm2835_spi_transfernb((char *)tx_buf, (char *)rx_buf, len);
}

void on_off_pwr(uint8_t state)
{
	if (state)
	{
		bcm2835_gpio_write(AX_PWR_PIN, LOW);
		bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
	}
	else
	{
		bcm2835_gpio_write(AX_PWR_PIN, HIGH);
		bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, HIGH);
	}
	usleep(100 * 1000);
}

void cb_gpioISR(int gpio, int level, uint32_t tick)
{
	char str[UINT8_MAX];
	snprintf(str, UINT8_MAX, "GPIO %d interrupt detected: level %d, rpi system tick %.6fs 0x", gpio, level, (double)tick / 1000000);
	std::cout << str << std::hex << std::this_thread::get_id() << std::endl;
	axradio_isr();
}

void enable_pin_irq(void)
{
	std::cout << "enable irq pin 0x" << std::hex << std::this_thread::get_id() << std::endl;
	bcm2835_gpioSetISRFunc(AX_IRQ_PIN, RISING_EDGE, -1, cb_gpioISR);
	std::cout << "enabled" << std::endl;
}

void disable_pin_irq(void)
{
	std::cout << "disable irq pin 0x" << std::hex << std::this_thread::get_id() << std::endl;
	bcm2835_gpioSetISRFunc(AX_IRQ_PIN, RISING_EDGE, -1, NULL);
	std::cout << "disabled" << std::endl;
}

void nbfi_read_default_settings(nbfi_settings_t* settings)
{
	memcpy((void *)settings, (const void *)&nbfi_set_default, sizeof(nbfi_settings_t));
}

void nbfi_before_tx(NBFi_ax5043_pins_s * pins)
{
	pins->cfga = PA_DIFFERENTIAL;
}

void nbfi_before_rx(NBFi_ax5043_pins_s * pins)
{
	pins->cfga = PA_DIFFERENTIAL;
}

void nbfi_before_off(NBFi_ax5043_pins_s * pins){}

void nbfi_read_flash_settings(nbfi_settings_t* settings)
{
	memset(settings, 0, sizeof(nbfi_settings_t));
}

void nbfi_write_flash_settings(nbfi_settings_t* settings){}

uint32_t nbfi_measure_voltage_or_temperature(uint8_t val)
{
	return 0;
}

uint32_t nbfi_update_rtc()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void nbfi_rtc_synchronized(uint32_t time){}

void nbfi_receive_complete(uint8_t * data, uint16_t length){}

void nbfi_reset(){}

static struct wtimer_fn_t wtimer_fn
{
	.get_time = get_time_chrono,
};

static struct axradio_fn_t axradio_fn
{
	.ax5043_enable_global_irq = dummy,
	.ax5043_disable_global_irq = dummy,
	.ax5043_enable_pin_irq = enable_pin_irq,
	.ax5043_disable_pin_irq = disable_pin_irq,
	.ax5043_get_irq_pin_state = get_irq_pin_state,
	.spi_rx = spi_rx,
	.spi_tx = spi_tx,
	.spi_tx_rx = spi_tx_rx,
	.spi_cs_set = dummy_uint8,
	.axradio_statuschange = axradio_statuschange,
	.ax5043_on_off_pwr = on_off_pwr,
};

static struct nbfi_func_t nbfi_fn
{
	.rx_handler = nbfi_receive_complete,
	.before_tx = nbfi_before_tx,
	.before_rx = nbfi_before_rx,
	.before_off = nbfi_before_off,
	.read_default_settings = nbfi_read_default_settings,
	.read_flash_settings = nbfi_read_flash_settings,
	.write_flash_settings = nbfi_write_flash_settings,
	.measure_voltage_or_temperature = nbfi_measure_voltage_or_temperature,
	.update_rtc = nbfi_update_rtc,
	.rtc_synchronized = nbfi_rtc_synchronized,
	.lock_unlock_nbfi_irq = dummy_uint8,
	.reset = nbfi_reset,
};

void radio_init(void)
{
	if (bcm2835_init())
	{
		if (bcm2835_spi_begin())
		{
			bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
			bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
			bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);
			bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
			bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);

			bcm2835_gpio_fsel(AX_PWR_PIN, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(AX_PWR_PIN, LOW);

			bcm2835_gpioSetISRFunc(AX_IRQ_PIN, RISING_EDGE, -1, cb_gpioISR);
		}
		else
			std::cout << "bcm2835_spi_begin failed. Are you running as root?" << std::endl;
	}
	else
		std::cout << "bcm2835_spi_begin failed. Are you running as root?" << std::endl;

	std::cout << "check point" << std::endl;

	NBFI_reg_func(&nbfi_fn);
	wtimer_reg_func(&wtimer_fn);
	ax5043_reg_func(&axradio_fn);

	wtimer_init();

	nbfi_dev_info_t info = {MODEM_ID, (uint32_t*)KEY, TX_MIN_POWER, TX_MAX_POWER, HW_ID, HW_REV, BAND, SEND_INFO_PERIOD};

	NBFi_Config_Set_Device_Info(&info);
//	NBFi_Clear_Saved_Configuration();		//	if you need to clear previously saved nbfi configuration
	NBFI_Init();
}
