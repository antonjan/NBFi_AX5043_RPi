#include <strings.h>
#include "wtimer.h"

typedef void (*handler_t)(struct wtimer_callback *desc);

static struct wtimer_fn_t _wtimer_fn;
struct wtimer_state wtimer_state[2];
struct wtimer_callback *wtimer_pending;

void wtimer_reg_func(struct wtimer_fn_t *fn)
{
	memcpy(&_wtimer_fn, fn, sizeof(struct wtimer_fn_t));
}

void wtimer_init(void)
{
	wtimer_pending = WTIMER_NULLCB;
	wtimer_state[0].queue = WTIMER_NULLDESC;
	wtimer0_update();
	wtimer0_schedq();
}

void wtimer_addcb_core(struct wtimer_callback *desc)
{
	struct wtimer_callback *d = WTIMER_CBPTR(wtimer_pending);
	while(1)
	{
		struct wtimer_callback *dn = d->next;
		if (dn == WTIMER_NULLCB)
			break;
		d = dn;
	}
	d->next = (struct wtimer_callback *)desc;
	desc->next = WTIMER_NULLCB;
}

void wtimer0_schedq(void)
{
	while (wtimer_state[0].queue != WTIMER_NULLDESC)
	{
		int32_t td = wtimer_state[0].time - wtimer_state[0].queue->time;
		if (td >= 0)
		{
			struct wtimer_callback * d = (struct wtimer_callback *)wtimer_state[0].queue;
			wtimer_state[0].queue = wtimer_state[0].queue->next;
			wtimer_addcb_core(d);
			continue;
		}
		return;
	}
}

void wtimer0_update(void)
{
	wtimer_state[0].time = _wtimer_fn.get_time();
}

void wtimer0_addcore(struct wtimer_desc *desc)
{
	struct wtimer_desc * d = WTIMER_PTR(wtimer_state[0].queue);
	while(1)
	{
		struct wtimer_desc * dn = d->next;
		int32_t td;
		if (dn == WTIMER_NULLDESC)
			break;
		td = desc->time - dn->time;
		if (td < 0)
			break;
		d = dn;
	}
	desc->next = d->next;
	d->next = desc;
}

static uint8_t wtimer_checkexpired(void)
{
	return 0;
/*
	uint16_t t;
	t = _wtimer_fn.cnt_get(0) - _wtimer_fn.cc_get(0);
	if (t < WTIMER0_MARGIN)
		return 1;

	if (_wtimer_fn.check_cc_irq(0) != 0)
		return 1;
	else
		return 0;
*/
}

uint8_t wtimer_runcallbacks(void)
{
	uint8_t ret = 0;
	while(1)
	{
		wtimer0_update();
		wtimer0_schedq();
		while(1)
		{
			struct wtimer_callback * d = wtimer_pending;
			if (d != WTIMER_NULLCB)
			{
				wtimer_pending = d->next;
				++ret;
				((handler_t)(d->handler))(d);
				continue;
			}

			uint8_t exp = wtimer_checkexpired();
			if (exp)
				break;
			return ret;
		}
	}
}

void ScheduleTask(struct wtimer_desc *desc, wtimer_desc_handler_t handler, uint8_t relative, uint32_t time)
{
	wtimer0_remove(desc);
	desc->time = time;
	if(relative)
		desc->time += wtimer0_curtime();
	if(handler)
		desc->handler = handler;
	wtimer0_addabsolute(desc);
}

uint8_t CheckTask(struct wtimer_desc *desc)
{
	uint8_t status = wtimer0_remove(desc);
	if (status)
		wtimer0_addabsolute(desc);
	return status;
}
