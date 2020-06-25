#include "sd.h"
#include "uart.h"

static int is_hcs;  /* high capcacity(SDHC) */

static inline void delay(unsigned long tick)
{
  while (tick--);
}

static void pin_setup(void)
{
  SD_SET(GPIO_GPFSEL4, 0x24000000);
  SD_SET(GPIO_GPFSEL5, 0x924);
  SD_SET(GPIO_GPPUD, 0);
  delay(15000);
  SD_SET(GPIO_GPPUDCLK1, 0xffffffff);
  delay(15000);
  SD_SET(GPIO_GPPUDCLK1, 0);
}

static void sdhost_setup(void)
{
  unsigned tmp;
  SD_SET(SDHOST_PWR, 0);
  SD_SET(SDHOST_CMD, 0);
  SD_SET(SDHOST_ARG, 0);
  SD_SET(SDHOST_TOUT, SDHOST_TOUT_DEFAULT);
  SD_SET(SDHOST_CDIV, 0);
  SD_SET(SDHOST_HSTS, SDHOST_HSTS_MASK);
  SD_SET(SDHOST_CFG, 0);
  SD_SET(SDHOST_CNT, 0);
  SD_SET(SDHOST_SIZE, 0);
  SD_GET(SDHOST_DBG, tmp);
  tmp &= ~SDHOST_DBG_MASK;
  tmp |= SDHOST_DBG_FIFO;
  SD_SET(SDHOST_DBG, tmp);
  delay(250000);
  SD_SET(SDHOST_PWR, 1);
  delay(250000);
  SD_SET(SDHOST_CFG, SDHOST_CFG_SLOW | SDHOST_CFG_INTBUS | SDHOST_CFG_DATA_EN);
  SD_SET(SDHOST_CDIV, SDHOST_CDIV_DEFAULT);
}

static int wait_sd(void)
{
  int cnt = 1000000;
  unsigned cmd;
  do
  {
    if(cnt == 0)
    {
      return -1;
    }
    SD_GET(SDHOST_CMD, cmd);
    --cnt;
  }
  while(cmd & SDHOST_NEW_CMD);
  return 0;
}

static int sd_cmd(unsigned cmd, unsigned arg)
{
  SD_SET(SDHOST_ARG, arg);
  SD_SET(SDHOST_CMD, cmd | SDHOST_NEW_CMD);
  return wait_sd();
}

static int sdcard_setup(void)
{
  unsigned tmp;
  sd_cmd(GO_IDLE_STATE | SDHOST_NO_REPONSE, 0);
  sd_cmd(SEND_IF_COND, VOLTAGE_CHECK_PATTERN);
  SD_GET(SDHOST_RESP0, tmp);
  if(tmp != VOLTAGE_CHECK_PATTERN)
  {
    return -1;
  }
  while(1)
  {
    if(sd_cmd(APP_CMD, 0) == -1)
    {
      /* MMC card or invalid card status */
      /* currently not support */
      continue;
    }
    sd_cmd(SD_APP_OP_COND, SDCARD_3_3V | SDCARD_ISHCS);
    SD_GET(SDHOST_RESP0, tmp);
    if(tmp & SDCARD_READY)
    {
      break;
    }
    delay(1000000);
  }

  is_hcs = tmp & SDCARD_ISHCS;
  sd_cmd(ALL_SEND_CID | SDHOST_LONG_RESPONSE, 0);
  sd_cmd(SEND_RELATIVE_ADDR, 0);
  SD_GET(SDHOST_RESP0, tmp);
  sd_cmd(SELECT_CARD, tmp);
  sd_cmd(SET_BLOCKLEN, 512);
  return 0;
}

static int wait_fifo(void)
{
  int cnt = 1000000;
  unsigned int hsts;
  do
  {
    if(cnt == 0)
    {
      return -1;
    }
    SD_GET(SDHOST_HSTS, hsts);
    --cnt;
  }
  while((hsts & SDHOST_HSTS_DATA) == 0);
  return 0;
}

static void set_block(int size, int cnt)
{
  SD_SET(SDHOST_SIZE, size);
  SD_SET(SDHOST_CNT, cnt);
}

static void wait_finish(void)
{
  unsigned dbg;
  do
  {
    SD_GET(SDHOST_DBG, dbg);
  }
  while((dbg & SDHOST_DBG_FSM_MASK) != SDHOST_HSTS_DATA);
}

void readblock(int block_idx, void * buf)
{
  unsigned * buf_u = (unsigned *)buf;
  int succ = 0;
  if(!is_hcs)
  {
    block_idx <<= 9;
  }
  do
  {
    set_block(512, 1);
    sd_cmd(READ_SINGLE_BLOCK | SDHOST_READ, (unsigned)block_idx);
    for(int i = 0; i < 128; ++i)
    {
      wait_fifo();
      SD_GET(SDHOST_DATA, buf_u[i]);
    }
    unsigned hsts;
    SD_GET(SDHOST_HSTS, hsts);
    if(hsts & SDHOST_HSTS_ERR_MASK)
    {
      SD_SET(SDHOST_HSTS, SDHOST_HSTS_ERR_MASK);
      sd_cmd(STOP_TRANSMISSION | SDHOST_BUSY, 0);
    }
    else
    {
      succ = 1;
    }
  }
  while(!succ);
  wait_finish();
}

void writeblock(unsigned block_idx, void * buf)
{
  unsigned * buf_u = (unsigned *)buf;
  int succ = 0;
  if(!is_hcs)
  {
    block_idx <<= 9;
  }
  do
  {
    set_block(512, 1);
    sd_cmd(WRITE_SINGLE_BLOCK | SDHOST_WRITE, block_idx);
    for(int i = 0; i < 128; ++i)
    {
      wait_fifo();
      SD_SET(SDHOST_DATA, buf_u[i]);
    }
    unsigned hsts;
    SD_GET(SDHOST_HSTS, hsts);
    if(hsts & SDHOST_HSTS_ERR_MASK)
    {
      SD_SET(SDHOST_HSTS, SDHOST_HSTS_ERR_MASK);
      sd_cmd(STOP_TRANSMISSION | SDHOST_BUSY, 0);
    }
    else
    {
      succ = 1;
    }
  }
  while(!succ);
  wait_finish();
}

void sd_init(void)
{
  pin_setup();
  sdhost_setup();
  sdcard_setup();
  uart_puts("SD card driver init complete\n");
}

