#ifndef MAHR_SPI_SLAVE2_H_
#define MAHR_SPI_SLAVE2_H_

#include <MAHR.h>
#include <ESP32DMASPISlave.h>

ESP32DMASPI::Slave slave2;
static const uint32_t BUFFER_SIZE_2 = 32;
uint8_t *spi_slave2_tx_buf;
uint8_t *spi_slave2_rx_buf;

// SPI Slave2 Initialization
void SPISlave2_Setup() {
  // to use DMA buffer, use these methods to allocate buffer
  spi_slave2_tx_buf = slave2.allocDMABuffer(BUFFER_SIZE_2);
  spi_slave2_rx_buf = slave2.allocDMABuffer(BUFFER_SIZE_2);

  // Set Buffers:
  for (uint32_t i = 0; i < BUFFER_SIZE_2; i++){
    spi_slave2_tx_buf[i] = (0xFF - i) & 0xFF;
  }
  memset(spi_slave2_rx_buf, 0, BUFFER_SIZE_2);
  delay(2000);

  // slave device configuration
  slave2.setDataMode(SPI_MODE0);
  slave2.setMaxTransferSize(BUFFER_SIZE_2);

  slave2.begin(VSPI);
  pinMode(SPI_SS, INPUT);
}
// Send to/receive from the Master
void SPISlave2_DataUpdate() {
  for(uint8_t i=0; i<8; i++){
    spi_slave2_tx_buf[i+ 8] = RightEncoder_Distance >> (8*i);
    spi_slave2_tx_buf[i+16] = LeftEncoder_Distance  >> (8*i);
  }
  
  slave2.wait(spi_slave2_rx_buf, spi_slave2_tx_buf, BUFFER_SIZE_2);

  while (slave2.available()) {
    zAxis_Speed = 0;
    voice_file = 0;
    for(uint8_t i=0; i<2; i++){
      zAxis_Speed      |= spi_slave2_rx_buf[i+16] << (8*i);
      voice_file       |= spi_slave2_rx_buf[i+24] << (8*i);
    }
    slave2.pop();
  }
}

#endif