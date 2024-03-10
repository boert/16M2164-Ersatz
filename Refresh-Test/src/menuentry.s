;--------------------------------------------------------------------------
;  menuentry.s - generate menu entry for programms
;
;  Copyright (c) 2015, 2024 boert, CC BY-SA 2.0 de
;--------------------------------------------------------------------------

    .area _CODE

    .db 0x7f
    .db 0x7f
    .str 'REFRESH'
    .db 0x01
    jp init
