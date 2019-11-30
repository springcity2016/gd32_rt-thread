
#include <rthw.h>
#include <drv_usart.h>
#include <rtthread.h>
#include "gd32f3x0.h"

#ifdef RT_USING_SERIAL

#define UART_ENABLE_IRQ(n)          NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)         NVIC_DisableIRQ((n))

#if !defined(RT_USING_USART0) && !defined(RT_USING_USART1)
#error "Please define at least one UARTx"
#endif


#include <rtdevice.h>

struct gd32_uart
{
    uint32_t uart_periph;
    IRQn_Type irqn;
    rcu_periph_enum per_clk;
    rcu_periph_enum tx_gpio_clk;
    rcu_periph_enum rx_gpio_clk;
    uint32_t tx_port;
    uint16_t tx_pin;
    uint32_t rx_port;
    uint16_t rx_pin;

    struct rt_serial_device *serial;
    char *device_name;
};

static void uart_isr(struct rt_serial_device *serial);

#if defined (RT_USING_USART0)
struct rt_serial_device serial0;
void USART0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    uart_isr(&serial0);
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif

#if defined(RT_USING_USART1)
struct rt_serial_device serial1;
void USART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    uart_isr(&serial1);
    /* leave interrupt */
    rt_interrupt_leave;
}
#endif

static const struct gd32_uart uarts[] = {
    #ifdef RT_USING_USART0
    {
        USART0,                             // uart peripheral index
        USART0_IRQn,                        // uart iqrn
        RCU_USART0, RCU_GPIOA, RCU_GPIOA,   // periph clock, tx gpio clock, rt gpio clock
        GPIOA, GPIO_PIN_9,                  // tx port, tx pin
        GPIOA, GPIO_PIN_10,                 // rx port, rx pin
        &serial0,
        "uart0"
    },
    #endif

    #ifdef RT_USING_USART1
    {
        USART1,                             // uart peripheral index
        USART1_IRQn,                        // uart iqrn
        RCU_USART1, RCU_GPIOA, RCU_GPIOB,   // periph clock, tx gpio clock, rt gpio clock
        GPIOA, GPIO_PIN_8,                  // tx port, tx pin
        GPIOB, GPIO_PIN_0,                  // rx port, rx pin
        &serial0,
        "uart0"
    },
    #endif
};

void gd32_uart_gpio_init(struct gd32_uart *uart)
{
    /* enable USART clock */
    rcu_periph_clock_enable(uart->tx_gpio_clk);
    rcu_periph_clock_enable(uart->rx_gpio_clk);
    rcu_periph_clock_enable(uart->per_clk);

    /* connect port to USARTx_Tx */
#ifdef RT_USING_USART0
    gpio_af_set(uart->tx_port, GPIO_AF_1, uart->tx_pin);
#endif
#ifdef RT_USING_USART1
    gpio_af_set(uart->tx_port, GPIO_AF_4, uart->tx_pin);
#endif
    gpio_mode_set(uart->tx_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, uart->tx_pin);
    gpio_output_options_set(uart->tx_port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, uart->tx_pin);

    /* connect port to USARTx_Rx */
#ifdef RT_USING_USART0
    gpio_af_set(uart->rx_port, GPIO_AF_1, uart->rx_pin);
#endif
#ifdef RT_USING_USART1
    gpio_af_set(uart->rx_port, GPIO_AF_4, uart->rx_pin);
#endif
    gpio_mode_set(uart->rx_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, uart->rx_pin);
    gpio_output_options_set(uart->rx_port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, uart->rx_pin);

    NVIC_SetPriority(uart->irqn, 0);
    NVIC_EnableIRQ(uart->irqn);
}

static rt_err_t gd32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;
    gd32_uart_gpio_init(uart);
    usart_baudrate_set(uart->uart_periph, cfg->baud_rate);
    
    switch (cfg->data_bits)
    {
    case DATA_BITS_9:
        usart_word_length_set(uart->uart_periph, USART_WL_9BIT);
        break;
    default:
        usart_word_length_set(uart->uart_periph, USART_WL_8BIT);
        break;
    }

    switch (cfg->stop_bits)
    {
    case STOP_BITS_2:
        usart_stop_bit_set(uart->uart_periph, USART_STB_2BIT);
        break;
    default:
        usart_stop_bit_set(uart->uart_periph, USART_STB_1BIT);
        break;
    }

    switch (cfg->parity)
    {
    case PARITY_ODD:
        usart_parity_config(uart->uart_periph, USART_PM_ODD);
        break;
    case PARITY_EVEN:
        usart_parity_config(uart->uart_periph, USART_PM_EVEN);
        break;
    default:
        usart_parity_config(uart->uart_periph, USART_PM_NONE);
        break;
    }

    usart_receive_config(uart->uart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart->uart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(uart->uart_periph);

    return RT_EOK;
}

static rt_err_t gd32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        NVIC_DisableIRQ(uart->irqn);
        usart_interrupt_disable(uart->uart_periph, USART_INT_RBNE);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        NVIC_EnableIRQ(uart->irqn);
        usart_interrupt_enable(uart->uart_periph, USART_INT_RBNE);
        break;
    }
    
    return RT_EOK;
}
static int gd32_putc(struct rt_serial_device *serial, char ch)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;

    usart_data_transmit(uart->uart_periph, ch);
    while((usart_flag_get(uart->uart_periph, USART_FLAG_TC) == RESET));

    return 1;
}
static int gd32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct gd32_uart *uart;
    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;
    ch = -1;
    if (usart_flag_get(uart->uart_periph, USART_FLAG_RBNE) != RESET)
        ch = usart_data_receive(uart->uart_periph);
    return ch;
}

static void uart_isr(struct rt_serial_device *serial)
{
    struct gd32_uart *uart = (struct gd32_uart *)serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);
    if ((usart_interrupt_flag_get(uart->uart_periph, USART_INT_FLAG_RBNE) != RESET)
     && (usart_flag_get(uart->uart_periph, USART_FLAG_RBNE) != RESET)) {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
        usart_flag_clear(uart->uart_periph, USART_FLAG_RBNE);
    }
}

static const struct rt_uart_ops gd32_uart_ops = 
{
    gd32_configure,
    gd32_control,
    gd32_putc,
    gd32_getc
};

int gd32_hw_usart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    int i;

    for (i = 0; i < sizeof(uarts) / sizeof(uarts[0]); i++) {
        uarts[i].serial->ops = &gd32_uart_ops;
        uarts[i].serial->config = config;

        rt_hw_serial_register(uarts[i].serial,
                              uarts[i].device_name,
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              (void *)&uarts[i]);
    }
    return 0;
}
INIT_BOARD_EXPORT(gd32_hw_usart_init);

#endif

