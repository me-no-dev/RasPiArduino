#include "Arduino.h"
#include "RF24.h"

RF24::RF24(uint8_t _cepin, uint8_t _cspin):
  ce_pin(_cepin), csn_pin(_cspin), wide_band(false), p_variant(false), 
  payload_size(32), ack_payload_available(false), dynamic_payloads_enabled(false), ack_payload_length(0),
  pipe0_reading_address(0){}

void RF24::spiInit(){
  digitalWrite(SS, HIGH);
  pinMode(SS, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  // Initialize pins
  pinMode(ce_pin,OUTPUT);
  pinMode(csn_pin,OUTPUT);
}

uint8_t RF24::spiTransfer(uint8_t data){
  return SPI.transfer(data);
}

void RF24::csn(int mode){
  digitalWrite(csn_pin,mode);
}

void RF24::ce(int level){
  digitalWrite(ce_pin,level);
}

void RF24::begin(void){
  spiInit();
  
  ce(LOW);
  csn(HIGH);
  delay(5) ;

  // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
  // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
  // sizes must never be used. See documentation for a more complete explanation.
  setRetries(4, 15);//delay 4x250us, count 15
  setPALevel(RF24_PA_MAX);
  if(setDataRate(RF24_250KBPS)) p_variant = true;
  setDataRate(RF24_1MBPS);
  setCRCLength(RF24_CRC_16);
  write_register(DYNPD, 0);// Disable dynamic payloads, to match dynamic_payloads_enabled setting
  setChannel(0x3F);
  
  // Reset current status. Notice reset and flush is the last thing we do
  write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));
  flush_rx();
  flush_tx();
}




uint8_t RF24::flush_rx(void){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(FLUSH_RX);
  csn(HIGH);
  return status;
}

uint8_t RF24::flush_tx(void){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(FLUSH_TX);
  csn(HIGH);
  return status;
}

uint8_t RF24::get_status(void){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(NOP);
  csn(HIGH);
  return status;
}

void RF24::toggle_features(void){
  csn(LOW);
  spiTransfer(ACTIVATE);
  spiTransfer(0x73);
  csn(HIGH);
}

bool RF24::isPVariant(void){
  return p_variant ;
}

void RF24::setPayloadSize(uint8_t size){
  if(payload_size == 0 && size > 0){
    disableDynamicPayloads();
    payload_size = min(size, 32);
  } else if(payload_size > 0 && size == 0){
    enableDynamicPayloads();
  }
}

uint8_t RF24::getPayloadSize(void){
  return payload_size;
}

uint8_t RF24::read_register(uint8_t reg, uint8_t* buf, uint8_t len){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(R_REGISTER | (REGISTER_MASK & reg));
  while (len--) *buf++ = spiTransfer(0xff);
  csn(HIGH);
  return status;
}

uint8_t RF24::read_register(uint8_t reg){
  csn(LOW);
  spiTransfer(R_REGISTER | (REGISTER_MASK & reg));
  uint8_t result = spiTransfer(0xff);
  csn(HIGH);
  return result;
}

uint8_t RF24::write_register(uint8_t reg, const uint8_t* buf, uint8_t len){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(W_REGISTER | (REGISTER_MASK & reg));
  while (len--) spiTransfer(*buf++);
  csn(HIGH);
  return status;
}

uint8_t RF24::write_register(uint8_t reg, uint8_t value){
  uint8_t status;
  csn(LOW);
  status = spiTransfer(W_REGISTER | (REGISTER_MASK & reg));
  spiTransfer(value);
  csn(HIGH);
  return status;
}

uint8_t RF24::write_payload(const void* buf, uint8_t len){
  uint8_t status;
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
  uint8_t data_len = (payload_size > 0)?min(len,payload_size):min(len,32);
  uint8_t blank_len = (payload_size == 0) ? 0 : payload_size - data_len;
  csn(LOW);
  status = spiTransfer( W_TX_PAYLOAD );
  while (data_len--) spiTransfer(*current++);
  while (blank_len--) spiTransfer(0);
  csn(HIGH);
  return status;
}

uint8_t RF24::read_payload(void* buf, uint8_t len){
  uint8_t status;
  uint8_t* current = reinterpret_cast<uint8_t*>(buf);
  uint8_t data_len = (payload_size > 0)?min(len,payload_size):min(len,32);
  uint8_t blank_len = (payload_size == 0) ? 0 : payload_size - data_len;
  csn(LOW);
  status = spiTransfer( R_RX_PAYLOAD );
  while (data_len--) *current++ = spiTransfer(0xff);
  while (blank_len--) spiTransfer(0xff);
  csn(HIGH);
  return status;
}

void RF24::writeAckPayload(uint8_t pipe, const void* buf, uint8_t len){
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
  uint8_t data_len = (payload_size > 0)?min(len,payload_size):min(len,32);
  csn(LOW);
  spiTransfer(W_ACK_PAYLOAD | (pipe & B111));
  while (data_len--) spiTransfer(*current++);
  csn(HIGH);
}

void RF24::powerDown(void){
  write_register(CONFIG,read_register(CONFIG) & ~_BV(PWR_UP));
}

void RF24::powerUp(void){
  write_register(CONFIG,read_register(CONFIG) | _BV(PWR_UP));
}

void RF24::setChannel(uint8_t channel){
  write_register(RF_CH, min(channel,127));
}

bool RF24::testCarrier(void){
  return (read_register(CD) &1);
}

bool RF24::testRPD(void){
  return (read_register(RPD) &1) ;
}

void RF24::setRetries(uint8_t delay, uint8_t count){
 write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf));
}

void RF24::setAutoAck(bool enable){
  if ( enable )
    write_register(EN_AA, B111111);
  else
    write_register(EN_AA, 0);
}

void RF24::setAutoAck( uint8_t pipe, bool enable ){
  if (pipe <= 6){
    uint8_t en_aa = read_register(EN_AA);
    if(enable){
      en_aa |= _BV(pipe);
    } else {
      en_aa &= ~_BV(pipe);
    }
    write_register(EN_AA, en_aa);
  }
}

void RF24::enableDynamicPayloads(void){
  write_register(FEATURE,read_register(FEATURE) | _BV(EN_DPL));
  if (!read_register(FEATURE)){
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) | _BV(EN_DPL));
  }
  write_register(DYNPD,read_register(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));
  payload_size = 0;
}

void RF24::disableDynamicPayloads(void){
  write_register(FEATURE,read_register(FEATURE) & ~_BV(EN_DPL));
  if (!read_register(FEATURE)){
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) & ~_BV(EN_DPL));
  }
  write_register(DYNPD,0);
  payload_size = 32;
}

void RF24::enableAckPayload(void){
  write_register(FEATURE,read_register(FEATURE) | _BV(EN_ACK_PAY));
  if (!read_register(FEATURE)){
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) | _BV(EN_ACK_PAY));
  }
}

void RF24::disableAckPayload(void){
  write_register(FEATURE,read_register(FEATURE) & ~_BV(EN_ACK_PAY));
  if (!read_register(FEATURE)){
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) & ~_BV(EN_ACK_PAY));
  }
}

void RF24::setPALevel(rf24_pa_dbm_e level){
  uint8_t setup = read_register(RF_SETUP) ;
  setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
  if (level == RF24_PA_MAX){
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
  } else if (level == RF24_PA_HIGH){
    setup |= _BV(RF_PWR_HIGH) ;
  } else if (level == RF24_PA_LOW){
    setup |= _BV(RF_PWR_LOW);
  } else if (level == RF24_PA_MIN){
    // nothing
  } else if (level == RF24_PA_ERROR){
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
  }
  write_register(RF_SETUP, setup);
}

rf24_pa_dbm_e RF24::getPALevel(void){
  rf24_pa_dbm_e result = RF24_PA_ERROR ;
  uint8_t power = read_register(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
  if (power == (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH))){
    result = RF24_PA_MAX ;
  } else if (power == _BV(RF_PWR_HIGH)){
    result = RF24_PA_HIGH ;
  } else if (power == _BV(RF_PWR_LOW)){
    result = RF24_PA_LOW ;
  } else {
    result = RF24_PA_MIN ;
  }
  return result ;
}

bool RF24::setDataRate(rf24_datarate_e speed){
  bool result = false;
  uint8_t setup = read_register(RF_SETUP) ;
  wide_band = false ;
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
  if(speed == RF24_250KBPS){
    wide_band = false ;
    setup |= _BV( RF_DR_LOW ) ;
  } else {
    if (speed == RF24_2MBPS){
      wide_band = true ;
      setup |= _BV(RF_DR_HIGH);
    } else {
      wide_band = false ;
    }
  }
  write_register(RF_SETUP,setup);
  if (read_register(RF_SETUP) == setup){
    result = true;
  } else {
    wide_band = false;
  }
  return result;
}

rf24_datarate_e RF24::getDataRate( void ){
  rf24_datarate_e result ;
  uint8_t dr = read_register(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
  
  if (dr == _BV(RF_DR_LOW)){
    result = RF24_250KBPS ;
  } else if (dr == _BV(RF_DR_HIGH)){
    result = RF24_2MBPS ;
  } else {
    result = RF24_1MBPS ;
  }
  return result ;
}

void RF24::setCRCLength(rf24_crclength_e length){
  uint8_t config = read_register(CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;
  if (length == RF24_CRC_DISABLED){
    // Do nothing, we turned it off above. 
  } else if (length == RF24_CRC_8){
    config |= _BV(EN_CRC);
  } else {
    config |= _BV(EN_CRC);
    config |= _BV( CRCO );
  }
  write_register( CONFIG, config ) ;
}

rf24_crclength_e RF24::getCRCLength(void){
  rf24_crclength_e result = RF24_CRC_DISABLED;
  uint8_t config = read_register(CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;
  if (config & _BV(EN_CRC)){
    if (config & _BV(CRCO))
      result = RF24_CRC_16;
    else
      result = RF24_CRC_8;
  }
  return result;
}

void RF24::disableCRC(void){
  uint8_t disable = read_register(CONFIG) & ~_BV(EN_CRC) ;
  write_register(CONFIG, disable) ;
}

bool RF24::whatHappened(bool& tx_ok,bool& tx_fail,bool& rx_ready){
  uint8_t status = write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  tx_ok = status & _BV(TX_DS);
  tx_fail = status & _BV(MAX_RT);
  rx_ready = status & _BV(RX_DR);
  return ((status & (_BV(TX_DS) | _BV(MAX_RT) | _BV(RX_DR))) > 0);
}

bool RF24::whatHappened(bool& tx_ok,bool& tx_fail,bool& rx_ready,bool& tx_full, uint8_t& rx_pipe){
  uint8_t status = write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  tx_ok = status & _BV(TX_DS);
  tx_fail = status & _BV(MAX_RT);
  rx_ready = status & _BV(RX_DR);
  tx_full = status & _BV(TX_FULL);
  rx_pipe = (status & 0x0E) >> 1;
  return ((status & (_BV(TX_DS) | _BV(MAX_RT) | _BV(RX_DR) | _BV(TX_FULL))) > 0);
}

void RF24::startListening(void){
  write_register(CONFIG, read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
  write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  if (pipe0_reading_address) write_register(RX_ADDR_P0, reinterpret_cast<const uint8_t*>(&pipe0_reading_address), 5);
  flush_rx();
  flush_tx();
  ce(HIGH);
  delayMicroseconds(130);
}

void RF24::stopListening(void){
  ce(LOW);
  flush_tx();
  flush_rx();
}

uint8_t RF24::getDynamicPayloadSize(void){
  uint8_t result = 0;
  csn(LOW);
  spiTransfer(R_RX_PL_WID);
  result = spiTransfer(0xff);
  csn(HIGH);
  return result;
}


// READING

static const uint8_t child_pipe[6] = { RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5 };
static const uint8_t child_payload_size[6] = { RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5 };
static const uint8_t child_pipe_enable[6] = { ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5 };

void RF24::openReadingPipe(uint8_t child, uint64_t address){
  if (child == 0) pipe0_reading_address = address;
  if (child <= 6){
    // For pipes 2-5, only write the LSB
    if ( child < 2 )
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), 5);
    else
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), 1);

    write_register(child_payload_size[child],payload_size);
    write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));
  }
}

bool RF24::available(void){
  return available(NULL);
}

bool RF24::available(uint8_t* pipe_num){
  uint8_t status = get_status();
  bool result = (status & _BV(RX_DR));

  if (result){
    if (pipe_num) *pipe_num = (status >> RX_P_NO) & B111;
    write_register(STATUS,_BV(RX_DR) );
    if (status & _BV(TX_DS)){
      write_register(STATUS,_BV(TX_DS));
    }
  }

  return result;
}

bool RF24::read(void* buf, uint8_t len){
  read_payload(buf, len);
  return read_register(FIFO_STATUS) & _BV(RX_EMPTY);
}

// WRITING

void RF24::openWritingPipe(uint64_t value){
  write_register(RX_ADDR_P0, reinterpret_cast<uint8_t*>(&value), 5);
  write_register(TX_ADDR, reinterpret_cast<uint8_t*>(&value), 5);
  write_register(RX_PW_P0, payload_size);
}

void RF24::startWrite( const void* buf, uint8_t len ){
  write_register(CONFIG, ( read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
  delayMicroseconds(255);
  write_payload(buf, len);
  ce(HIGH);
  delayMicroseconds(20);
  ce(LOW);
}

// BLOCKING WRITE

bool RF24::write( const void* buf, uint8_t len ){
  bool result = false;

  // Begin the write
  startWrite(buf,len);

  // ------------
  // At this point we could return from a non-blocking write, and then call
  // the rest after an interrupt

  // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
  // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
  // is flaky and we get neither.

  // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
  // if I tighted up the retry logic.  (Default settings will be 1500us.
  // Monitor the send
  uint8_t observe_tx;
  uint8_t status;
  uint32_t sent_at = millis();
  const uint32_t timeout = 500; //ms to wait for timeout
  do{
    status = read_register(OBSERVE_TX,&observe_tx,1);
  } while(!(status & (_BV(TX_DS) | _BV(MAX_RT))) && (millis() - sent_at < timeout));

  // The part above is what you could recreate with your own interrupt handler,
  // and then call this when you got an interrupt
  // ------------

  // Call this when you get an interrupt
  // The status tells us three things
  // * The send was successful (TX_DS)
  // * The send failed, too many retries (MAX_RT)
  // * There is an ack packet waiting (RX_DR)
  bool tx_ok, tx_fail;
  whatHappened(tx_ok,tx_fail,ack_payload_available);

  result = tx_ok;

  // Handle the ack packet
  if (ack_payload_available){
    ack_payload_length = getDynamicPayloadSize();
  }

  powerDown();
  flush_tx();
  return result;
}

bool RF24::isAckPayloadAvailable(void){
  bool result = ack_payload_available;
  ack_payload_available = false;
  return result;
}
