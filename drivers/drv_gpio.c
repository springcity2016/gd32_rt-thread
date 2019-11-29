
#include "drv_gpio.h"
#include <rtdevice.h>
#include <rthw.h>
#include "gd32f3x0.h"
#include "gd32f3x0_exti.h"


// #ifdef RT_USIING_PIN     // 文件写完后打开

#define __GD32_PIN(index, port, pin) { index, RCU_GPIO##port, GPIO##port, \
        GPIO_PIN_##pin, GPIO_PORT_SOURCE_GPIO##port, GPIO_PIN_SOURCE_##pin }
#define __GD32_PIN_DEFAULT  { -1, (rcu_periph_enum)0, 0, 0, 0, 0 }

// #define __GD32_PIN(index, port, pin) {index, RCU_GPIO##port, GPIO##port, \
//         GPIO_PIN_##pin, GPIO_PORT_SOURCE_GPIO##port, GPIO_PIN_SOURCE_##pin}
// #define __GD32_PIN_DEFAULT {-1, (rcu_periph_enum)0, 0, 0, 0, 0}


/* GD32 GPIO driver */
struct pin_index
{
    rt_int16_t index;
    rcu_periph_enum clk;
    rt_uint32_t gpio_periph;
    rt_uint32_t pin;
    rt_uint8_t port_src;
    rt_uint8_t pin_src;
};

static const struct pin_index pins[] = 
{
    __GD32_PIN_DEFAULT,
    __GD32_PIN(1, C, 13),
    __GD32_PIN(2, C, 14),
    __GD32_PIN(3, C, 15),
    __GD32_PIN(4, F, 0),
    __GD32_PIN(5, F, 1),
    __GD32_PIN_DEFAULT,
    __GD32_PIN(7, C, 0),
    __GD32_PIN(8, C, 1),
    __GD32_PIN(9, C, 2),
    __GD32_PIN(10, C, 3),
    __GD32_PIN_DEFAULT,
    __GD32_PIN_DEFAULT,
    __GD32_PIN(13, A, 0),
    __GD32_PIN(14, A, 1),
    __GD32_PIN(15, A, 2),
    __GD32_PIN(16, A, 3),
    __GD32_PIN(17, F, 4),
    __GD32_PIN(18, F, 5),
    __GD32_PIN(19, A, 4),
    __GD32_PIN(20, A, 5),
    __GD32_PIN(21, A, 6),
    __GD32_PIN(22, A, 7),
    __GD32_PIN(23, C, 4),
    __GD32_PIN(24, C, 5),
    __GD32_PIN(25, B, 0),
    __GD32_PIN(26, B, 1),
    __GD32_PIN(27, B, 2),
    __GD32_PIN(28, B, 10),
    __GD32_PIN(29, B, 11),
    __GD32_PIN_DEFAULT,
    __GD32_PIN_DEFAULT,
    __GD32_PIN(32, B, 12),
    __GD32_PIN(33, B, 13),
    __GD32_PIN(34, B, 14),
    __GD32_PIN(35, B, 15),
    __GD32_PIN(36, C, 6),
    __GD32_PIN(37, C, 7),
    __GD32_PIN(38, C, 8),
    __GD32_PIN(39, C, 9),
    __GD32_PIN(40, A, 8),
    __GD32_PIN(41, A, 9),
    __GD32_PIN(42, A, 10),
    __GD32_PIN(43, A, 11),
    __GD32_PIN(44, A, 12),
    __GD32_PIN(45, A, 13),
    __GD32_PIN(46, F, 6),
    __GD32_PIN(47, F, 7),
    __GD32_PIN(48, A, 14),
    __GD32_PIN(49, A, 15),
    __GD32_PIN(50, C, 10),
    __GD32_PIN(51, C, 11),
    __GD32_PIN(52, C, 12),
    __GD32_PIN(53, D, 2),
    __GD32_PIN(54, B, 3),
    __GD32_PIN(55, B, 4),
    __GD32_PIN(56, B, 5),
    __GD32_PIN(57, B, 6),
    __GD32_PIN(58, B, 7),
    __GD32_PIN_DEFAULT,
    __GD32_PIN(60, B, 8),
    __GD32_PIN(61, B, 9),
    __GD32_PIN_DEFAULT,
    __GD32_PIN_DEFAULT,
};

struct pin_irq_map
{
    rt_uint16_t pinbit;
    IRQn_Type irqno;
};
static const struct pin_irq_map pin_irq_map[] = 
{
    { GPIO_PIN_0, EXTI0_1_IRQn },
    { GPIO_PIN_1, EXTI0_1_IRQn },
    { GPIO_PIN_2, EXTI2_3_IRQn },
    { GPIO_PIN_3, EXTI2_3_IRQn },
    { GPIO_PIN_4, EXTI4_15_IRQn },
    { GPIO_PIN_5, EXTI4_15_IRQn },
    { GPIO_PIN_6, EXTI4_15_IRQn },
    { GPIO_PIN_7, EXTI4_15_IRQn },
    { GPIO_PIN_8, EXTI4_15_IRQn },
    { GPIO_PIN_9, EXTI4_15_IRQn },
    { GPIO_PIN_10, EXTI4_15_IRQn },
    { GPIO_PIN_11, EXTI4_15_IRQn },
    { GPIO_PIN_12, EXTI4_15_IRQn },
    { GPIO_PIN_13, EXTI4_15_IRQn },
    { GPIO_PIN_14, EXTI4_15_IRQn },
    { GPIO_PIN_15, EXTI4_15_IRQn },
};
struct rt_pin_irq_hdr pin_irq_hdr_tab[] = 
{
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
    { -1, 0, RT_NULL, RT_NULL },
};

#define ITEM_NUM(items) (sizeof(items) / sizeof(items[0]))
const struct pin_index *get_pin(rt_uint8_t pin)
{
    const struct pin_index *index;
    if (pin < ITEM_NUM(pins)) {
        index = &pins[pin];
        if (index->index == -1) { index = RT_NULL; }
    } else {
        index = NULL;
    }
    return index;
};

void gd32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    rt_uint32_t pin_mode;
    rt_uint32_t pin_pull_up_down;
    rt_uint32_t pin_otype;
    index = get_pin(pin);
    if (index == RT_NULL) { return; }


    /* GPIO Periph clock enable */
    rcu_periph_clock_enable(index->clk);
    pin_mode = GPIO_MODE_OUTPUT;
    pin_pull_up_down = GPIO_PUPD_PULLUP;
    pin_otype = GPIO_OTYPE_PP;

    switch(mode)
    {
   case PIN_MODE_OUTPUT:
        /* output setting */
        pin_mode = GPIO_MODE_OUTPUT;
        pin_pull_up_down = GPIO_PUPD_PULLUP;
        pin_otype = GPIO_OTYPE_PP;
        break;
   case PIN_MODE_OUTPUT_OD:
        /* output setting: od. */
        pin_mode = GPIO_MODE_OUTPUT;
        pin_pull_up_down = GPIO_PUPD_PULLUP;
        pin_otype = GPIO_OTYPE_OD;
        break;
   case PIN_MODE_INPUT:
        /* input setting: not pull. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pull_up_down = GPIO_PUPD_NONE;
        break;
   case PIN_MODE_INPUT_PULLUP:
        /* input setting: pull up. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pull_up_down = GPIO_PUPD_PULLUP;
        break;
   case PIN_MODE_INPUT_PULLDOWN:
        /* input setting: pull down. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pull_up_down = GPIO_PUPD_PULLDOWN;
        break;
   default:
        break;
    }

    /* set GPIO mode */
    gpio_mode_set(index->gpio_periph, pin_mode, pin_pull_up_down, index->pin);
    /* set GPIO output type and speed */
    gpio_output_options_set(index->gpio_periph, pin_otype, GPIO_OSPEED_50MHZ, index->pin);

}

void gd32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;
    index = get_pin(pin);
    if (index == RT_NULL) { return; }
    gpio_bit_write(index->gpio_periph, index->pin, (bit_status)value);
}

int gd32_pin_read(rt_device_t dev, rt_base_t pin) 
{
    int value;
    const struct pin_index *index;
    value = PIN_LOW;
    index = get_pin(pin);
    if (index == RT_NULL) {
        return value;
    }
    value = gpio_input_bit_get(index->gpio_periph, index->pin);
    return value;
}

rt_inline rt_int32_t bit2bitno(rt_uint32_t bit)
{
    rt_uint8_t i;
    for (i = 0; i < 32; i++) {
        if ((0x01 << i) == bit) { return i; }
    }
    return -1;
}
rt_inline const struct pin_irq_map *get_pin_irq_map(rt_uint32_t pinbit)
{
    rt_int32_t mapindex = bit2bitno(pinbit);
    if (mapindex < 0 || mapindex >= ITEM_NUM(pin_irq_map)) { return RT_NULL; }
    return &pin_irq_map[mapindex];
};

rt_err_t gd32_pin_attach_irq(struct rt_device *device, rt_int32_t pin, rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t hdr_index = -1;

    index = get_pin(pin);
    if (index == RT_NULL) {
        return RT_EINVAL;
    }
    hdr_index = bit2bitno(index->pin);
    if ((hdr_index < 0) || (hdr_index >= ITEM_NUM(pin_irq_map))) {
        return RT_EINVAL;
    } 
    level = rt_hw_interrupt_disable();
    if ((pin_irq_hdr_tab[hdr_index].pin == pin)
     && (pin_irq_hdr_tab[hdr_index].hdr == hdr)
     && (pin_irq_hdr_tab[hdr_index].mode == mode)
     && (pin_irq_hdr_tab[hdr_index].args == args)) {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    if (pin_irq_hdr_tab[hdr_index].pin != -1) {
        rt_hw_interrupt_enable(level);
        return RT_EFULL;
    }
    pin_irq_hdr_tab[hdr_index].pin = pin;
    pin_irq_hdr_tab[hdr_index].hdr = hdr;
    pin_irq_hdr_tab[hdr_index].mode = mode;
    pin_irq_hdr_tab[hdr_index].args = args;
    rt_hw_interrupt_enable(level);
    return RT_EOK;
}
rt_err_t gd32_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t hdr_index = -1;

    index = get_pin(pin);
    if (index == RT_NULL) {
        return RT_EINVAL;
    }
    hdr_index = bit2bitno(index->pin);
    if ((hdr_index < 0) || (hdr_index >= ITEM_NUM(pin_irq_map))) {
        return RT_EINVAL;
    }
    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[hdr_index].pin == -1) {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    pin_irq_hdr_tab[hdr_index].pin = -1;
    pin_irq_hdr_tab[hdr_index].hdr = RT_NULL;
    pin_irq_hdr_tab[hdr_index].mode = 0;
    pin_irq_hdr_tab[hdr_index].args = RT_NULL;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
rt_err_t gd32_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enable)
{
    const struct pin_index *index;
    const struct pin_irq_map *irqmap;
    rt_base_t level;
    rt_int32_t hdr_index = -1;
    exti_trig_type_enum trigger_mode;
    
    return RT_EOK;
}
const static struct rt_pin_ops _gd32_pin_ops = 
{
    gd32_pin_mode,
    gd32_pin_write,
    gd32_pin_read,
    gd32_pin_attach_irq,
    gd32_pin_detach_irq,
    gd32_pin_irq_enable,
};
int rt_hw_pin_init(void)
{
    int result;
    result = rt_device_pin_register("pin", &_gd32_pin_ops, RT_NULL);
    return result;
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

// #endif                   // 文件写完后打开

