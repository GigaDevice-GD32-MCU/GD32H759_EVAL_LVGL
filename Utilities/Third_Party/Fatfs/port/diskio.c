/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"        /* FatFs lower layer API */
#include "ff.h"
#include "sdcard.h"
#include <stdio.h>
/* Ϊÿ���豸����һ���������?*/
#define ATA                0     // Ԥ��SD��ʹ��
#define SPI_FLASH          1     // �ⲿSPI Flash

#define BLOCKSIZE   512      /* block size in bytes */                                              
/* 4-bit mode cannot be used on this board: SDIO0_DAT3 is PC11, which the eval
   board wires to the LCD as TLI LCD_B4 (see gd32h759i_lcd_eval.c). Without DAT3
   the card never answers, so keep the 1-bit bus. */
//#define BUSMODE_4BIT       /* SD 4-bit bus mode, uncommend this macro to choose 1-bit bus mode */
//#define DMA_MODE             /* SD DMA mode, uncommend this macro to choose polling mode */
sd_card_info_struct sd_cardinfo;

/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
    BYTE pdrv        /* �������?*/
)
{
    DSTATUS status = STA_NOINIT;
    DWORD QSPI_ID;
    
    switch (pdrv) {
        case ATA:    /* SD CARD */
            status = RES_OK;
            break;

        case SPI_FLASH:
            status = STA_NOINIT;
            break;
        default:
            status = STA_NOINIT;
    }
    return status;
}

/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
    BYTE pdrv                /* �������?*/
)
{
    DSTATUS status = STA_NOINIT;
    uint32_t cardstate = 0;
    
    switch (pdrv) {
        case ATA:               /* SD CARD */
            /* initialize the card */
            status = sd_init();
            if(SD_OK == status){
                status = sd_card_information_get(&sd_cardinfo);
            }else{
                return STA_NOINIT;
            }
            if(SD_OK == status){
                status = sd_card_select_deselect(sd_cardinfo.card_rca);
            }else{
                return STA_NOINIT;
            }
            status = sd_cardstatus_get(&cardstate);
            if(cardstate & 0x02000000){
                /* the card is locked */
                return STA_NOINIT;
            }
            /* configure the bus mode and data transfer mode */
            if(SD_OK == status){
                /* set bus mode */
#ifdef BUSMODE_4BIT
                status = sd_bus_mode_config(SDIO_BUSMODE_4BIT, SD_SPEED_DEFAULT);
#else
                status = sd_bus_mode_config(SDIO_BUSMODE_1BIT, SD_SPEED_DEFAULT);
#endif /* BUSMODE_4BIT */
            }else{
                return STA_NOINIT;
            }
            if(SD_OK == status){
                /* set data transfer mode */
#ifdef DMA_MODE
                status = sd_transfer_mode_config( SD_DMA_MODE );
                /* configure the SDIO NVIC */
                nvic_irq_enable(SDIO_IRQn, 0, 0);
#else
                status = sd_transfer_mode_config( SD_POLLING_MODE );
#endif /* DMA_MODE */
            }else{
                return STA_NOINIT;
            }
            if(SD_OK == status){
                /* initialize success */
                return 0;
            }else{
                return STA_NOINIT;
            }
    
        case SPI_FLASH:         /* SPI Flash */
            status = STA_NOINIT;
            break;

        default:
            status = STA_NOINIT;
    }
    return status;
}


/*-----------------------------------------------------------------------*/
/* ����������ȡ�������ݵ�ָ���洢��                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
    BYTE pdrv,        /* �豸�������?0..) */
    BYTE *buff,       /* ���ݻ����� */
    DWORD sector,     /* �����׵�ַ */
    UINT count        /* ��������(1..128) */
)
{
    sd_error_enum status = SD_ERROR;
    
    /* check the correctness of the parameters */
    if(NULL == buff){
        return RES_PARERR;
    }
    if(!count){
        return RES_PARERR;
    }

    switch (pdrv) {
        case ATA:    /* SD CARD */
            /* The GD32H7 SD driver takes a sector (block) index, not a byte
               address: it multiplies by 512 itself for standard capacity
               cards. Passing sector<<9 only worked for sector 0. */
            if(1 == count){
                /* single sector read */
                status = sd_block_read((uint32_t *)(&buff[0]), (uint32_t)sector, BLOCKSIZE);
            }else{
                /* multiple sectors read */
                status = sd_multiblocks_read((uint32_t *)(&buff[0]), (uint32_t)sector, BLOCKSIZE, (uint32_t)count);
            }
            SCB_CleanInvalidateDCache();
            if(SD_OK == status){
                return RES_OK;
            }
            return RES_ERROR;
    
        case SPI_FLASH:
            return RES_ERROR;

        default:
            return RES_PARERR;
    }
}

/*-----------------------------------------------------------------------*/
/* д������������д��ָ�������ռ���                                      */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write (
    BYTE pdrv,              /* �豸�������?0..) */
    const BYTE *buff,       /* ��д�����ݵĻ����� */
    DWORD sector,           /* �����׵�ַ */
    UINT count              /* ��������(1..128) */
)
{
    sd_error_enum status = SD_ERROR;
    if (!count) {
        return RES_PARERR;        /* Check parameter */
    }

    switch (pdrv) {
        case ATA:    /* SD CARD */
            /* Sector index, see the note in disk_read(). */
            if(1 == count){
                /* single sector write */
                status = sd_block_write((uint32_t *)buff, (uint32_t)sector, BLOCKSIZE);
            }else{
                /* multiple sectors write */
                status = sd_multiblocks_write((uint32_t *)buff, (uint32_t)sector, BLOCKSIZE, (uint32_t)count);
            }
            SCB_CleanInvalidateDCache();
            if(SD_OK == status){
                return RES_OK;
            }
            return RES_ERROR;

        case SPI_FLASH:
            return RES_ERROR;

        default:
            return RES_PARERR;
    }
}
#endif


/*-----------------------------------------------------------------------*/
/* ��������                                                              */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,        /* �������?*/
    BYTE cmd,         /* ����ָ�� */
    void *buff        /* д����߶�ȡ���ݵ�ַָ��?*/
)
{
    uint32_t capacity;
    DRESULT status = RES_PARERR;
    switch (pdrv) {
        case ATA:    /* SD CARD */
            switch (cmd) 
            {
                // Get R/W sector size (WORD) 
                case GET_SECTOR_SIZE :
                    *(WORD * )buff = BLOCKSIZE;
                    break;
                // Get erase block size in unit of sector (DWORD)
                case GET_BLOCK_SIZE :
                    *(DWORD * )buff = 1;//sd_cardinfo.card_blocksize;
                    break;

                case GET_SECTOR_COUNT:
                    capacity = sd_card_capacity_get();
                    *(DWORD * )buff = capacity*1024/sd_cardinfo.card_blocksize;
                    break;
                case CTRL_SYNC :
                    break;
            }
            return RES_OK;
    
        case SPI_FLASH:
            return RES_PARERR;
    
        default:
            status = RES_PARERR;
    }
    return status;
}

DWORD get_fattime(void) {
    /* ���ص�ǰʱ���?*/
    return    ((DWORD)(2019 - 1980) << 25)    /* Year 2019 */
            | ((DWORD)1 << 21)                /* Month 1 */
            | ((DWORD)1 << 16)                /* Mday 1 */
            | ((DWORD)0 << 11)                /* Hour 0 */
            | ((DWORD)0 << 5)                 /* Min 0 */
            | ((DWORD)0 >> 1);                /* Sec 0 */
}
