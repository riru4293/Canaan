/* -------------------------------------------------------------------------- */
/* Include                                                                    */
/* -------------------------------------------------------------------------- */
#include <bsp/board_api.h>
#include <tusb.h>

/* A combination of interfaces must have a unique product id, since PC will */
/* save device driver after the first plug. Same VID/PID with different     */
/* interface e.g MSC (first), then CDC (later) will possibly cause system   */
/* error on PC.                                                             */
/*                                                                          */
/* Auto ProductID layout's Bitmap:                                          */
/*     [MSB]                 HID | MSC | CDC                    [LSB]       */
/*                                                                          */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                 _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

#define USB_VID (0xCafe)
#define USB_BCD (0x0200)

/* -------------------------------------------------------------------------- */
/* Device Descriptors                                                         */
/* -------------------------------------------------------------------------- */
tusb_desc_device_t const desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = USB_BCD,

        /* Use Interface Association Descriptor (IAD) for CDC */
        /* As required by USB Specs IAD's subclass must be    */
        /* common class (2) and protocol must be IAD (1)      */
        .bDeviceClass = TUSB_CLASS_MISC,
        .bDeviceSubClass = MISC_SUBCLASS_COMMON,
        .bDeviceProtocol = MISC_PROTOCOL_IAD,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = USB_VID,
        .idProduct = USB_PID,
        .bcdDevice = 0x0100,

        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,

        .bNumConfigurations = 0x01};

/* Invoked when received GET DEVICE DESCRIPTOR */
/* Application return pointer to descriptor    */
uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&desc_device;
}

/* -------------------------------------------------------------------------- */
/* Configuration Descriptor                                                   */
/* -------------------------------------------------------------------------- */
enum
{
    ITF_NUM_CDC_0 = 0,
    ITF_NUM_CDC_0_DATA,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN)

#define EPNUM_CDC_0_NOTIF (0x81)
#define EPNUM_CDC_0_OUT (0x02)
#define EPNUM_CDC_0_IN (0x82)

uint8_t const desc_fs_configuration[] =
    {
        /* Config number, interface count, string index, total length, attribute, power in mA */
        TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

        /* 1st CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size. */
        TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_0, 4, EPNUM_CDC_0_NOTIF, 8, EPNUM_CDC_0_OUT, EPNUM_CDC_0_IN, 64),
};

/* Invoked when received GET CONFIGURATION DESCRIPTOR */
/* Application return pointer to descriptor */
/* Descriptor contents must exist long enough for transfer to complete */
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index; /* for multiple configurations */

    return desc_fs_configuration;
}

/* -------------------------------------------------------------------------- */
/* String Descriptors                                                         */
/* -------------------------------------------------------------------------- */

/* String Descriptor Index */
enum
{
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};

/* Array of pointer to string descriptors */
char const *string_desc_arr[] =
    {
        (const char[]){0x09, 0x04}, /* 0: is supported language is English (0x0409) */
        "TinyUSB",                  /* 1: Manufacturer                              */
        "TinyUSB Device",           /* 2: Product                                   */
        NULL,                       /* 3: Serials will use unique ID if possible    */
        "TinyUSB CDC",              /* 4: CDC Interface                             */
};

static uint16_t _desc_str[32 + 1];

/* Invoked when received GET STRING DESCRIPTOR request */
/* Application return pointer to descriptor, whose contents must exist long enough for transfer to complete */
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    size_t chr_count;

    switch (index)
    {
    case STRID_LANGID:
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
        break;

    case STRID_SERIAL:
        chr_count = board_usb_get_serial(_desc_str + 1, 32);
        break;

    default:
        /* Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors. */
        /* https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors */

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char *str = string_desc_arr[index];

        /* Cap at max char */
        chr_count = strlen(str);
        size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; /* -1 for string type */
        if (chr_count > max_count)
            chr_count = max_count;

        /* Convert ASCII string into UTF-16 */
        for (size_t i = 0; i < chr_count; i++)
        {
            _desc_str[1 + i] = str[i];
        }
        break;
    }

    /* First byte is length (including header), second byte is string type */
    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return _desc_str;
}
