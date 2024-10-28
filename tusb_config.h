#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

/* -------------------------------------------------------------------------- */
/* Board specific configuration                                               */
/* -------------------------------------------------------------------------- */

/* Root hub port number. */
#define BOARD_TUD_RHPORT (0)

/* Root hub port max operational speed. */
#define BOARD_TUD_MAX_SPEED (OPT_MODE_DEFAULT_SPEED)

/* -------------------------------------------------------------------------- */
/* Common configuration                                                       */
/* -------------------------------------------------------------------------- */

/* Set to 1 to enable tinyusb device functionality. */
#define CFG_TUD_ENABLED (1)

/* Max speed that hardware controller could support with on-chip PHY */
#define CFG_TUD_MAX_SPEED (BOARD_TUD_MAX_SPEED)

/* Tinyusb use follows macros to declare transferring memory */
/* so that they can be put into those specific section.      */
#define CFG_TUSB_MEM_SECTION __attribute__((section(".usb_ram")))
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))

/* -------------------------------------------------------------------------- */
/* Device configuration                                                       */
/* -------------------------------------------------------------------------- */

/* Defines the maximum packet size for endpoint 0. */
#define CFG_TUD_ENDPOINT0_SIZE (64)

/* Set to 1 to enable USB class. */
#define CFG_TUD_CDC (1)
#define CFG_TUD_MSC (0)
#define CFG_TUD_HID (0)
#define CFG_TUD_MIDI (0)
#define CFG_TUD_VENDOR (0)

/* USB-CDC FIFO size of TX and RX. */
#define CFG_TUD_CDC_RX_BUFSIZE (64)
#define CFG_TUD_CDC_TX_BUFSIZE (64)

/* USB-CDC Endpoint transfer buffer size. */
#define CFG_TUD_CDC_EP_BUFSIZE (64)

#endif /* TUSB_CONFIG_H */
