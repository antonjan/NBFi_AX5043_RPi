#ifndef RADIO_H
#define RADIO_H

#define AX_PWR_PIN 5
#define AX_IRQ_PIN 6

#define BAND				UL868800_DL864000
#define HW_ID				255
#define HW_REV				0
#define TX_MAX_POWER		15
#define TX_MIN_POWER		0
#define SEND_INFO_PERIOD	(60 * 60 * 24 * 30)  //	one time per month nrx/drx

void radio_init(void);

#endif // RADIO_H
