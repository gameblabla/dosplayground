#define GDC_CONTROL          0x68
#define GDC_CONTROL2         0x6A
#define GDC_PALETTE_INDEX    0xA8
#define GDC_PALETTE_R        0xAC
#define GDC_PALETTE_G        0xAA
#define GDC_PALETTE_B        0xAE
#define GDC_GET_DISP_PLANE   0xA4
#define GDC_SET_DISP_PLANE   0xA4
#define GDC_GET_RENDER_PLANE 0xA6
#define GFC_SET_RENDER_PLANE 0xA6

#define GDC_TEXT_STATUS      0x60
#define GDC_TEXT_COMMAND     0x62
#define GDC_TEXT_PARAM       0x60

#define GDC_GFX_STATUS       0xA0
#define GDC_GFX_COMMAND      0xA2
#define GDC_GFX_PARAM        0xA0

#define GDC_STATUS_READY     (1 << 0)
#define GDC_STATUS_FULL      (1 << 1)
#define GDC_STATUS_EMPTY     (1 << 2)
#define GDC_STATUS_DRAWING   (1 << 3)
#define GDC_STATUS_DMA       (1 << 4)
#define GDC_STATUS_VSYNC     (1 << 5)
#define GDC_STATUS_HBLANK    (1 << 6)
#define GDC_STATUS_LIGHTPEN  (1 << 7)

#define GDC_CMD_START        0xD
#define GDC_CMD_STOP         0xC
#define GDC_CMD_CURSOR_SHAPE 0x4B
#define GDC_CMD_VSYNC_MASTER 0x6F
#define GDC_CMD_VSYNC_SLAVE  0x6E

#define GDC_CTL_COLORS       0x2
#define GDC_CTL_LINES        0x8
#define GDC_CTL2_COLORS      0x0
#define GDC_CTL2_LOCK        0x17
#define GDC_CTL2_ENHANCED    0x20
#define GDC_CTL2_LINES       0x68
#define GDC_CTL2_TEXT_CLOCK  0x82
#define GDC_CTL2_GFX_CLOCK   0x84

#define GDC_PLANE_1          ((unsigned char far*)0xA8000000L)
#define GDC_PLANE_2          ((unsigned char far*)0xB0000000L)
#define GDC_PLANE_3          ((unsigned char far*)0xB8000000L)
#define GDC_PLANE_4          ((unsigned char far*)0xE0000000L)
