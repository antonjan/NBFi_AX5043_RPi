/*
 * ax5043.c
 *
 *  Created on: 28 ???. 2018 ?.
 *      Author: lema
 */

#include "ax5043.h"
#include "string.h"
#include "easyax5043.h"

struct axradio_fn_t _axradio_fn;

void ax5043_reg_func(struct axradio_fn_t * axradio_fn)
{
	memcpy(&_axradio_fn, axradio_fn, sizeof(struct axradio_fn_t));
}

void ax5043_enter_deepsleep(void)
{
	ax5043_spi_write(AX5043_PWRMODE, AX5043_PWRSTATE_POWERDOWN);
	ax5043_spi_write(AX5043_PWRMODE, AX5043_PWRSTATE_DEEPSLEEP);
}

uint8_t ax5043_wakeup_deepsleep_core(void)
{
	return 0;
}

uint8_t ax5043_wakeup_deepsleep(void)
{
	{
		uint8_t i = ax5043_wakeup_deepsleep_core();
		if (i)
			return i;
	}
	if (ax5043_probeirq())
		return RADIO_ERR_IRQ;
	return 0;
}

uint8_t ax5043_reset(void)
{
	uint8_t i;
	// Reset Device
	ax5043_spi_write(AX5043_PWRMODE, 0x80);
	ax5043_spi_write(AX5043_PWRMODE, AX5043_PWRSTATE_POWERDOWN);
	// Check Scratch
	i = ax5043_spi_read(AX5043_REVISION);
	if (i != SILICONREV1)
		return RADIO_ERR_REVISION;
	ax5043_spi_write(AX5043_SCRATCH, 0x55);
	i = ax5043_spi_read(AX5043_SCRATCH);
	if (i != 0x55)
		return RADIO_ERR_COMM;
	ax5043_spi_write(AX5043_SCRATCH, 0xAA);
	i = ax5043_spi_read(AX5043_SCRATCH);
	if (i != 0xAA)
		return RADIO_ERR_COMM;
	if (ax5043_probeirq())
		return RADIO_ERR_IRQ;
	ax5043_spi_write(AX5043_PINFUNCIRQ, 0x03);
	return RADIO_OK;
}

uint8_t ax5043_probeirq(void)
{
	uint8_t i[2] = {0, 0};
	_axradio_fn.ax5043_disable_pin_irq();
	ax5043_spi_write(AX5043_PINFUNCIRQ, 0x01);
	i[0] = _axradio_fn.ax5043_get_irq_pin_state();
	ax5043_spi_write(AX5043_PINFUNCIRQ, 0x00);
	i[1] = _axradio_fn.ax5043_get_irq_pin_state();
	ax5043_spi_write(AX5043_PINFUNCIRQ, 0x03);

	if (!i[0] || i[1])
	{
		/* Error */
		ax5043_spi_write(AX5043_PINFUNCIRQ, 0x02);
		_axradio_fn.ax5043_enable_pin_irq();
		return 1;
	}

	_axradio_fn.ax5043_enable_pin_irq();
	return 0;
}

void ax5043_spi_read_fifo(uint8_t *ptr, uint8_t len)
{
	uint8_t spi_tx_buf[UINT8_MAX], spi_rx_buf[UINT8_MAX];
	//_axradio_fn.ax5043_disable_pin_irq();
	_axradio_fn.spi_cs_set(0);

	memset(spi_tx_buf, 0 ,UINT8_MAX);
	spi_tx_buf[0] = AX5043_FIFODATA;
	_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, len + 1);
	memcpy(ptr, spi_rx_buf + 1, len);

	_axradio_fn.spi_cs_set(1);
	//_axradio_fn.ax5043_enable_pin_irq();
}


void ax5043_spi_write_fifo(uint8_t *ptr, uint8_t len)
{
	uint8_t spi_tx_buf[UINT8_MAX];
	//_axradio_fn.ax5043_disable_pin_irq();
	_axradio_fn.spi_cs_set(0);

	spi_tx_buf[0] = AX5043_FIFODATA | 0x80;
	memcpy(spi_tx_buf + 1, ptr, len);
	_axradio_fn.spi_tx(spi_tx_buf, len + 1);

	_axradio_fn.spi_cs_set(1);
	//_axradio_fn.ax5043_enable_pin_irq();
}


uint8_t ax5043_spi_write(uint32_t add, uint8_t data)
{
	uint8_t spi_tx_buf[10], spi_rx_buf[10];
	//_axradio_fn.ax5043_disable_pin_irq();

	if (add >= 0x70)
	{
		add |= 0x7000;
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = (add >> 8) | 0x80;
		spi_tx_buf[1] = add & 0xFF;
		spi_tx_buf[2] = data;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 3);
		_axradio_fn.spi_cs_set(1);
	}
	else
	{
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add | 0x80;
		spi_tx_buf[1] = data;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 2);
		_axradio_fn.spi_cs_set(1);
	}
	//_axradio_fn.ax5043_enable_pin_irq();
	return 1;
}

uint8_t ax5043_spi_read(uint32_t add)
{
	uint8_t spi_tx_buf[10], spi_rx_buf[10];
	//_axradio_fn.ax5043_disable_pin_irq();
	if (add >= 0x70)
	{
		add |= 0x7000;
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add >> 8;
		spi_tx_buf[1] = add & 0xFF;
		spi_tx_buf[2] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 3);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return spi_rx_buf[2];
	}
	else
	{
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add;
		spi_tx_buf[1] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 2);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return spi_rx_buf[1];
	}
}


uint16_t ax5043_spi_read16(uint32_t add)
{
	uint8_t spi_tx_buf[10], spi_rx_buf[10];
	//_axradio_fn.ax5043_disable_pin_irq();
	if (add >= 0x70)
	{
		add |= 0x7000;
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add >> 8;
		spi_tx_buf[1] = add & 0xFF;
		spi_tx_buf[2] = 0x00;
		spi_tx_buf[3] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 4);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return (uint16_t)spi_rx_buf[2] << 8 | (uint16_t)spi_rx_buf[3];
	}
	else
	{
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add;
		spi_tx_buf[1] = 0x00;
		spi_tx_buf[2] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 3);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return (uint16_t)spi_rx_buf[1] << 8 | (uint16_t)spi_rx_buf[2];
	}
}

uint32_t ax5043_spi_read24(uint32_t add)
{
	uint8_t spi_tx_buf[10], spi_rx_buf[10];
	//_axradio_fn.ax5043_disable_pin_irq();
	if (add >= 0x70)
	{
		add |= 0x7000;
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add >> 8;
		spi_tx_buf[1] = add & 0xFF;
		spi_tx_buf[2] = 0x00;
		spi_tx_buf[3] = 0x00;
		spi_tx_buf[4] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 5);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return (uint32_t)spi_rx_buf[2] << 16 | (uint32_t)spi_rx_buf[3]  << 8 | (uint32_t)spi_rx_buf[4];
	}
	else
	{
		_axradio_fn.spi_cs_set(0);
		spi_tx_buf[0] = add;
		spi_tx_buf[1] = 0x00;
		spi_tx_buf[2] = 0x00;
		spi_tx_buf[3] = 0x00;
		_axradio_fn.spi_tx_rx(spi_tx_buf, spi_rx_buf, 4);
		_axradio_fn.spi_cs_set(1);
		//_axradio_fn.ax5043_enable_pin_irq();
		return (uint32_t)spi_rx_buf[1] << 16 | (uint32_t)spi_rx_buf[2]  << 8 | (uint32_t)spi_rx_buf[3];
	}
}


void ax5043_tcxo_set_reset(uint8_t set)
{
	ax5043_spi_write(AX5043_PINFUNCPWRAMP, set);
}

void ax5043_hard_reset()
{
	if(_axradio_fn.ax5043_on_off_pwr)
	{
		_axradio_fn.ax5043_on_off_pwr(0);
		_axradio_fn.ax5043_on_off_pwr(1);
	}
}
