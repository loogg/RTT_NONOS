#include "board.h"

#ifdef BSP_USING_ON_CHIP_FLASH
#include "drv_flash.h"

#if defined(PKG_USING_FAL)
#include "fal.h"
#endif

//#define DRV_DEBUG
#define LOG_TAG                "drv.flash"
#include <drv_log.h>

#define SM_PAGE_ERASE_SIZE  2048
#define LG_PAGE_ERASE_SIZE  4096


#define SM_MAX_PAGE     256
#define LG_MAX_PAGE     640

struct flash_sector
{
    uint16_t first_sector;
    uint16_t sector_num;
    uint16_t sm_sector_num;
    uint16_t lg_sector_num;
};
typedef struct flash_sector* flash_sector_t;

static uint16_t gd32_flash_get_sector(rt_uint32_t addr)
{
    uint16_t sector;
    uint32_t offset;
    if(addr > FMC_BANK0_END_ADDRESS)
    {
        offset = addr - FMC_BANK0_END_ADDRESS - 1;
        sector = SM_MAX_PAGE + (offset / LG_PAGE_ERASE_SIZE);
    }
    else
    {
        offset = addr - GD32_FLASH_START_ADRESS;
        sector = offset / SM_PAGE_ERASE_SIZE;
    }

    return sector;
}

static void gd32_flash_get_erase(rt_uint32_t addr, size_t size, flash_sector_t sector)
{
    sector->first_sector = gd32_flash_get_sector(addr);
    sector->sector_num = gd32_flash_get_sector(addr + size - 1) - sector->first_sector + 1;

    if (sector->first_sector < SM_MAX_PAGE)
    {
        if ((sector->first_sector + sector->sector_num) <= SM_MAX_PAGE)
        {
            sector->lg_sector_num = 0;
            sector->sm_sector_num = sector->sector_num;
        }
        else
        {
            sector->sm_sector_num = SM_MAX_PAGE - sector->first_sector;
            sector->lg_sector_num = sector->sector_num - sector->sm_sector_num;
        }
    }
    else
    {
        sector->sm_sector_num = 0;
        sector->lg_sector_num = sector->sector_num;
    }
}

int gd32_flash_read(rt_uint32_t addr, rt_uint8_t *buf, size_t size)
{
    size_t i;

    if ((addr + size) > GD32_FLASH_END_ADDRESS)
    {
        LOG_E("read outrange flash size! addr is (0x%p)", (void *)(addr + size));
        return -RT_EINVAL;
    }

    for (i = 0; i < size; i++, buf++, addr++)
    {
        *buf = *(rt_uint8_t *) addr;
    }

    return size;
}

int gd32_flash_write(rt_uint32_t addr, const rt_uint8_t *buf, size_t size)
{
    rt_err_t result        = RT_EOK;
    rt_uint32_t end_addr   = addr + size;

    if (addr % 4 != 0)
    {
        LOG_E("write addr must be 4-byte alignment");
        return -RT_EINVAL;
    }

    if ((end_addr) > GD32_FLASH_END_ADDRESS)
    {
        LOG_E("write outrange flash size! addr is (0x%p)", (void *)(addr + size));
        return -RT_EINVAL;
    }

    fmc_unlock();
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    if (FMC_BANK0_SIZE < FMC_SIZE)
    {
        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_BANK1_END);
        fmc_flag_clear(FMC_FLAG_BANK1_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK1_PGERR);
    }

    while (addr < end_addr)
    {
        if (fmc_word_program(addr, *((rt_uint32_t *)buf)) == FMC_READY)
        {
            /* clear all pending flags */
            fmc_flag_clear(FMC_FLAG_BANK0_END);
            fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
            fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

            if (FMC_BANK0_SIZE < FMC_SIZE)
            {
                /* clear all pending flags */
                fmc_flag_clear(FMC_FLAG_BANK1_END);
                fmc_flag_clear(FMC_FLAG_BANK1_WPERR);
                fmc_flag_clear(FMC_FLAG_BANK1_PGERR);
            }

            if (*(rt_uint32_t *)addr != *(rt_uint32_t *)buf)
            {
                result = -RT_ERROR;
                break;
            }
            addr += 4;
            buf  += 4;
        }
        else
        {
            result = -RT_ERROR;
            break;
        }
    }

    fmc_lock();

    if (result != RT_EOK)
    {
        return result;
    }

    return size;
}

static int gd32_flash_erase_sector(flash_sector_t sector)
{
    fmc_state_enum flash_status;
    rt_err_t result = RT_EOK;
    uint32_t secpos = sector->first_sector;
    for(int i = 0; i < sector->sector_num; i++)
    {
        if(i < sector->sm_sector_num)
        {
            flash_status = fmc_page_erase(secpos * SM_PAGE_ERASE_SIZE + GD32_FLASH_START_ADRESS);
        }
        else
            flash_status = fmc_page_erase((secpos - SM_MAX_PAGE) * LG_PAGE_ERASE_SIZE + FMC_BANK0_END_ADDRESS + 1);

        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_BANK0_END);
        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

        if (FMC_BANK0_SIZE < FMC_SIZE)
        {
            /* clear all pending flags */
            fmc_flag_clear(FMC_FLAG_BANK1_END);
            fmc_flag_clear(FMC_FLAG_BANK1_WPERR);
            fmc_flag_clear(FMC_FLAG_BANK1_PGERR);
        }
        if(flash_status != FMC_READY)
        {
            result = -RT_ERROR;
            break;
        }

        secpos++;
    }
    return result;
}

int gd32_flash_erase(rt_uint32_t addr, size_t size)
{
    rt_err_t result = RT_EOK;
    struct flash_sector sector;

    if ((addr + size) > GD32_FLASH_END_ADDRESS)
    {
        LOG_E("ERROR: erase outrange flash size! addr is (0x%p)\n", (void *)(addr + size));
        return -RT_EINVAL;
    }

    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    if (FMC_BANK0_SIZE < FMC_SIZE)
    {
        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_BANK1_END);
        fmc_flag_clear(FMC_FLAG_BANK1_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK1_PGERR);
    }

    gd32_flash_get_erase(addr, size, &sector);

    result = gd32_flash_erase_sector(&sector);;

    fmc_lock();

    if (result != RT_EOK)
    {
        return result;
    }

    LOG_D("erase done: addr (0x%p), size %d", (void *)addr, size);
    return size;
}

#if defined(PKG_USING_FAL)

static int fal_flash_read(long offset, rt_uint8_t *buf, size_t size);
static int fal_flash_write(long offset, const rt_uint8_t *buf, size_t size);
static int fal_flash_erase(long offset, size_t size);

const struct fal_flash_dev gd32_onchip_flash = { "onchip_flash", GD32_FLASH_START_ADRESS, GD32_FLASH_SIZE, ON_CHIP_FLASH_MAX_ERASE_SIZE, {NULL, fal_flash_read, fal_flash_write, fal_flash_erase} };

static int fal_flash_read(long offset, rt_uint8_t *buf, size_t size)
{
    return gd32_flash_read(gd32_onchip_flash.addr + offset, buf, size);
}

static int fal_flash_write(long offset, const rt_uint8_t *buf, size_t size)
{
    return gd32_flash_write(gd32_onchip_flash.addr + offset, buf, size);
}

static int fal_flash_erase(long offset, size_t size)
{
    return gd32_flash_erase(gd32_onchip_flash.addr + offset, size);
}

#endif
#endif


