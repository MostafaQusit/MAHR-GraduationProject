#ifndef MAHR_SPI_SLAVE1_H_
#define MAHR_SPI_SLAVE1_H_

#include <MAHR.h>
#include <ESP32DMASPISlave.h>

ESP32DMASPI::Slave slave1;
static const uint32_t BUFFER_SIZE_1 = 32;
uint8_t *spi_slave1_tx_buf;
uint8_t *spi_slave1_rx_buf;

// SPI Slave1 Initialization
void SPISlave1_Setup() {
  // to use DMA buffer, use these methods to allocate buffer
  spi_slave1_tx_buf = slave1.allocDMABuffer(BUFFER_SIZE_1);
  spi_slave1_rx_buf = slave1.allocDMABuffer(BUFFER_SIZE_1);

  // Set Buffers:
  for (uint32_t i = 0; i < BUFFER_SIZE_1; i++){
    spi_slave1_tx_buf[i] = (0xFF - i) & 0xFF;
  }
  memset(spi_slave1_rx_buf, 0, BUFFER_SIZE_1);

  // slave device configuration
  slave1.setDataMode(SPI_MODE0);
  slave1.setMaxTransferSize(BUFFER_SIZE_1);

  slave1.begin(VSPI);
  pinMode(SPI_SS, INPUT);
  delay(1000);
}
// Send to/receive from the Master
void SPISlave1_DataUpdate() {
  for(uint8_t i=0; i<8; i++){
    spi_slave1_tx_buf[i+ 8] = RightEncoder_Distance >> (8*i);
    spi_slave1_tx_buf[i+16] = LeftEncoder_Distance  >> (8*i);
  }

  slave1.wait(spi_slave1_rx_buf, spi_slave1_tx_buf, BUFFER_SIZE_1);

  while (slave1.available()) {
    Target_RightMotor_mms = 0;
    Target_LeftMotor_mms  = 0;
    zAxis_Speed           = 0;
    voice_file            = 0;
    for(uint8_t i=0; i<2; i++){
      Target_RightMotor_mms |= ( int16_t)spi_slave1_rx_buf[i   ] << (8*i);
      Target_LeftMotor_mms  |= ( int16_t)spi_slave1_rx_buf[i+ 8] << (8*i);
      zAxis_Speed           |= ( int16_t)spi_slave1_rx_buf[i+16] << (8*i);
      voice_file            |= (uint16_t)spi_slave1_rx_buf[i+24] << (8*i);
    }
    slave1.pop();
  }
}

#endif