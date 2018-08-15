//=====================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��GPIO�ײ���������Դ�ļ�
//=====================================================================
#include "gpio.h"   //����������ͷ�ļ�

//���˿ڻ���ַ���볣��������PORT_ARR[0]~PORT_ARR[4]��
const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                      PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//GPIO�ڻ���ַ���볣��������GPIO_ARR[0]~GPIO_ARR[4]��
const GPIO_MemMapPtr GPIO_ARR[]={PTA_BASE_PTR,PTB_BASE_PTR,
                       PTC_BASE_PTR,PTD_BASE_PTR,PTE_BASE_PTR};

//�ڲ���������
void gpio_get_port_pin(uint_16 port_pin,uint_8* port,uint_8* pin);

//=====================================================================
//�������ƣ�gpio_init
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//          dir�����ŷ���0=���룬1=���,�������ŷ���궨�壩
//          state���˿����ų�ʼ״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ����ʼ��ָ���˿�������ΪGPIO���Ź��ܣ�������Ϊ�������������������
//          ��ָ����ʼ״̬�ǵ͵�ƽ��ߵ�ƽ
//=====================================================================
void gpio_init(uint_16 port_pin, uint_8 dir, uint_8 state)
{
    //�ֲ���������
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ�� 
    GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ��
    uint_8 port,pin;            //�����˿�port������pin����
    //���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
    gpio_get_port_pin(port_pin , &port , &pin);
    //����port�����ֲ�����port_ptr��gpio_ptr��ֵ�������������ַ��
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    gpio_ptr = GPIO_ARR[port];  //���GPIOģ����Ӧ�ڻ���ַ

    //�趨��Ӧ�˿ڵ���Ӧ���Ź���ΪGPIO���������ſ��ƼĴ�����MUX=0b001��
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK; //��D10-D8=000
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(1);    //��D10-D8=001

    //���ݴ������dir����������Ϊ�����������
    if (1 == dir)   //ϣ��Ϊ���
    {
         BSET(pin,GPIO_PDDR_REG(gpio_ptr));//���ݷ���Ĵ�����pinλ=1������Ϊ���
         gpio_set(port_pin, state);  //����gpio_set�������趨���ų�ʼ״̬
    }
    else           //ϣ��Ϊ����
    {
        BCLR(pin,GPIO_PDDR_REG(gpio_ptr));//���ݷ���Ĵ�����pinλ=0������Ϊ����
    }
}

//=====================================================================
//�������ƣ�gpio_set
//�������أ���
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       state�����ų�ʼ״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ���趨����״̬Ϊ�͵�ƽ��ߵ�ƽ
//=====================================================================
void gpio_set(uint_16 port_pin, uint_8 state)
{
    //�ֲ���������
    GPIO_MemMapPtr  gpio_ptr;  //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
    uint_8 port,pin;           //�����˿�port������pin����
    //���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
    gpio_ptr = GPIO_ARR[port]; 

    //���ݴ������state����������Ϊ���1����0
    if (1==state)
    {
        BSET(pin,GPIO_PDOR_REG(gpio_ptr));
    }
    else
    {
        BCLR(pin,GPIO_PDOR_REG(gpio_ptr));
    }
}

//=====================================================================
//�������ƣ�gpio_get
//�������أ�ָ�����ŵ�״̬��1��0��
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ȡָ�����ŵ�״̬��1��0��
//=====================================================================
uint_8 gpio_get(uint_16 port_pin)
{
    //�ֲ���������
    GPIO_MemMapPtr  gpio_ptr;  //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
    uint_8 port,pin;           //�����˿�port������pin����
    //���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
    gpio_ptr = GPIO_ARR[port];

    //�������ŵ�״̬
    return ((BGET(pin,GPIO_PDIR_REG(gpio_ptr)))>=1 ? 1:0);
}

//=====================================================================
//�������ƣ�gpio_reverse
//�������أ���
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����תָ���������״̬��
//=====================================================================
void gpio_reverse(uint_16 port_pin)
{
    //�ֲ���������
    GPIO_MemMapPtr  gpio_ptr;  //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
    uint_8 port,pin;           //�����˿�port������pin����
    //���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
    gpio_ptr = GPIO_ARR[port]; 

    //��תָ���������״̬
    BSET(pin,GPIO_PTOR_REG(gpio_ptr));
}

//=====================================================================
//�������ƣ�gpio_pull
//�������أ���
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//        pullselect���������ߵ͵�ƽ�� 0=���͵�ƽ��1=���ߵ�ƽ��
//���ܸ�Ҫ��ʹָ�����������ߵ�ƽ�������͵�ƽ
//=====================================================================
void gpio_pull(uint_16 port_pin, uint_8 pullselect)
{
    //�ֲ���������
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port,pin;           //�����˿�port������pin����
    //���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //����port�����ֲ�����port_ptr��ֵ����û���ַ��
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

    //�趨��Ӧ�˿ڵ���Ӧ���Ź���ΪGPIO���������ſ��ƼĴ�����MUX=0b001��
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK; //��D10-D8=000
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(1);    //��D10-D8=001

    //�趨����������������������������5mA��
    BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    //������������ʹ��
    BSET(PORT_PCR_PE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    //���ݴ������pullselect�������������߻�������
    if (1==pullselect) 
    {
        BSET(PORT_PCR_PS_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
    else
    {
        BCLR(PORT_PCR_PS_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
}

//=====================================================================
//�������ƣ�gpio_enable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       irqtype�������ж����ͣ��ɺ궨��������ٴ��о����£�
//                LOW_LEVEL    8      //�͵�ƽ����
//                HIGH_LEVEL   12     //�ߵ�ƽ����
//                RISING_EDGE  9      //�����ش���
//                FALLING_EDGE 10     //�½��ش���
//                DOUBLE_EDGE  11     //˫���ش���
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ�����������������жϣ���
//        �����жϴ���������
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//          KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//=====================================================================
void gpio_enable_int(uint_16 port_pin,uint_8 irqtype)
{
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_ISF_MASK;  //��������жϱ�־
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_IRQC(irqtype);//ʹ�������ж�

    switch(port)
    {
    case 0:   //PTA
        NVIC_EnableIRQ(PORTA_IRQn);   //���жϿ�����IRQ�ж�
        break;
    case 3:   //PTD
        NVIC_EnableIRQ(PORTD_IRQn);  //���жϿ�����IRQ�ж�
        break;
    default:;
    }
}

//=====================================================================
//�������ƣ�gpio_disable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ���������ر������ж�
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//           KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//=====================================================================
void gpio_disable_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin, &port, &pin);

    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_IRQC_MASK;//���������ж�
    
    switch(port)
    {
    case 0://PTA
        NVIC_DisableIRQ(PORTA_IRQn);   //���жϿ�����IRQ�ж�
        break;
    case 3://PTD
        NVIC_DisableIRQ(PORTD_IRQn);   //���жϿ�����IRQ�ж�
        break;
    default:;
    }
}

//=====================================================================
//�������ƣ�gpio_get_int
//�������أ�����GPIO�жϱ�־��1��0��1��ʾ������GPIO�жϣ�0��ʾ����û��GPIO�жϡ�
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ,��ȡ�жϱ�־��
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//           KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//=====================================================================
uint_8 gpio_get_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

    return ((BGET(PORT_PCR_ISF_SHIFT,PORT_PCR_REG(port_ptr, pin)))>=1 ? 1:0);
}

//=====================================================================
//�������ƣ�gpio_clear_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ,����жϱ�־��
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//           KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//=====================================================================
void gpio_clear_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_ISF_MASK;  //��������жϱ�־
}

//=====================================================================
//�������ƣ�gpio_clear_allint
//�������أ���
//����˵������
//���ܸ�Ҫ��������ж˿ڵ�GPIO�ж�
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//           KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//=====================================================================
void gpio_clear_allint(void)
{
    uint_8 i;
    for(i=0;i<5;i++)
    {
        PORT_ISFR_REG(PORT_ARR[i])=PORT_ISFR_ISF_MASK;
    }
}

//=====================================================================
//�������ƣ�gpio_drive_strength
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       control���������ŵ�����������1=��������0=��������
//         �����ű�����Ϊ�������ʱ�����ſ��ƼĴ�����DSE=1���������� DSE=0������������
//���ܸ�Ҫ������������������ָ�����������������ĳ�������һ����mA��λ����
//            ��������������5mA,����������18mA���������ű�����Ϊ�������ʱ,
//            �����ŵ����������������ã�ֻ��PTB0,PTB1,PTD6,PTD7ͬʱ���и���
//            ��������������������,��Щ���ſ�����ֱ������LED���MOSFET������
//            �볡Ч����ܣ����磬�ú���ֻ����������4�����š�
//=====================================================================
void gpio_drive_strength(uint_16 port_pin, uint_8 control)
{
    //�ֲ���������
    PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    //���ݴ������port�����ֲ�����port_ptr��ֵ
    port_ptr = PORT_ARR[port];

    //���ݴ������control����������Ϊ����ߵ���������������
    BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    if (1 == control)
    {
        BSET(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
    else
    {
        BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
}

//----------------------����Ϊ�ڲ�������Ŵ�-----------------
//=====================================================================
//�������ƣ�gpio_get_port_pin
//�������أ���
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       port���˿ںţ���ָ����������
//       pin:���źţ�0~31��ʵ��ȡֵ��оƬ���������ž���������ָ����������
//���ܸ�Ҫ������������port_pin���н������ó�����˿ں������źţ��ֱ�ֵ��port��pin,���ء�
//       ������(PTB_NUM)|(9)����ΪPORTB��9��������ֱ�ֵ��port��pin����
//=====================================================================
void gpio_get_port_pin(uint_16 port_pin,uint_8* port,uint_8* pin)
{
    *port = (port_pin>>8);
    *pin = port_pin;
}
//----------------------------�ڲ���������--------------------

