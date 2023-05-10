#ifndef MAHR_SPI_MASTER_H_
#define MAHR_SPI_MASTER_H_

#include <MAHR.h>
#include <ESP32DMASPIMaster.h>

ESP32DMASPI::Master master1;    // SPI Master object for Slave 1.
static const uint32_t BUFFER_SIZE_1 = 32;
uint8_t *spi_master1_tx_buf;
uint8_t *spi_master1_rx_buf;

LowPass<2> lp_er(20, 100e3, false);
LowPass<2> lp_el(10, 100e3, false);

// SPI Master Initialization
void SPIMaster_Setup() {
  Serial.print(F("SPI MASTER initializing..."));
  
  // to use DMA buffer, use these methods to allocate buffer
  spi_master1_tx_buf = master1.allocDMABuffer(BUFFER_SIZE_1);
  spi_master1_rx_buf = master1.allocDMABuffer(BUFFER_SIZE_1);

  // Set Buffers:
  for (uint32_t i = 0; i < BUFFER_SIZE_1; i++){
    spi_master1_tx_buf[i] = i & 0xFF;
  }
  memset(spi_master1_rx_buf, 0, BUFFER_SIZE_1);
  delay(1000);

  master1.setDataMode(SPI_MODE0);            // default: SPI_MODE0
  master1.setFrequency(4000000);             // default: 8MHz (too fast for bread board...)
  master1.setMaxTransferSize(BUFFER_SIZE_1); // default: 4092 bytes

  master1.begin(VSPI, SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS1);
  pinMode(SPI_SS1, OUTPUT);

  Serial.println(F("\tDone"));
}
// Send to/receive from slaves
void SPIMaster_DataUpdate() {
  for(uint8_t i=0; i<2; i++){
    spi_master1_tx_buf[i   ] = Target_RightMotor_mms >> (8*i);
    spi_master1_tx_buf[i+ 8] = Target_LeftMotor_mms  >> (8*i);
    spi_master1_tx_buf[i+16] = zAxis_Speed           >> (8*i);
    spi_master1_tx_buf[i+24] = voice_file            >> (8*i);
  }

  master1.transfer(spi_master1_tx_buf, spi_master1_rx_buf, BUFFER_SIZE_1);

  RightEncoder_Distance = 0;
  LeftEncoder_Distance  = 0;
  for(uint8_t i=0; i<8; i++){
    RightEncoder_Distance |= (int64_t)spi_master1_rx_buf[i+ 8] << (8*i);
    LeftEncoder_Distance  |= (int64_t)spi_master1_rx_buf[i+16] << (8*i);
  }
  RightEncoder_Distance_filtered = (int64_t) lp_er.filt(RightEncoder_Distance);
  LeftEncoder_Distance_filtered  = (int64_t) lp_el.filt(LeftEncoder_Distance);
}

#endif