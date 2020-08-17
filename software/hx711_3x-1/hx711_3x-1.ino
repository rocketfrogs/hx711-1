// ADC HX711 reader
// by AD & DS
//
// DATASHEET => https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf
//
// This code is doing that :
//
//  -----------------------<----------------------------------------------------------------------------------------<------------------------------------------------------------------------------------------------------------<-------
//  |                                                                                                                                                                                                                                   |            
//  |      ---------------------------------------             --------------------------------------------------------------------       --------------------------------------------       ---------------------------------------    |
//  v      |                                     |             |                                                                  |       |                                          |       |                                     |    ^
//  |      |      Waiting for the the ADC        |   dout LOW  |                   When data is ready, we toggle                  |       |                                          |       |                                     |    |
//  -->--->|     to be ready to spit data        | ----------> |                the clock pin (pd_sck) DOWN and UP                | ----> |   We toggle the clock pin an aditionnal  | ----> |   We concatenate the 3 8bit arrays  |-----
//  |      |  ( dout is set to LOW by the ADC    |   |         |           and we read the value of the data pin (dout)           |       |     64 or 128 time to set the gain       |       |      to make the value              |
//  ^      |    when it's ready to spit data )   |   v         |     we repeat that until we fill up 3 x 8 bit array (= 24 bit)   |       |            for the next reading          |       |                                     |
//  |      |                                     |   |         |    We fill the array left to right (1=> 10 => 101 => 1011 etc.)  |       |                                          |       |                                     |
//  |      ---------------------------------------   |         |     all that is done by the fonction shiftIn set to MSBFIRST     |       --------------------------------------------       ---------------------------------------                                       |
//  |                                                |         |                                                                  |
//  --------<--- wait [delay_ms] ms -----<------------         --------------------------------------------------------------------
//                                     dout HIGH


//// global variable declaration ////

// Pins ADC1
int pd_sck1 = 3;  // Clock pin
int dout1 = 2;    // Data out pin

// Pins ADC2
int pd_sck2 = 4;  // Clock pin
int dout2 = 5;    // Data out pin

// Pins ADC3
int pd_sck3 = 6;  // Clock pin
int dout3 = 7;    // Data out pin

int gain = 64;   // set gain

//// Variable initialisation ////
  
int GAIN = 1;       // We initialize the GAIN to 1 = 128 clock signal before aquisitions, this should be overwiten in the setup anytway
int delay_ms = 10;  // set the wait delay to 10 ms (this is the time the loop will wait if the ADC is not ready to spit data)

///////////////////////////////////////////////////////
//////  ///////////////////////////////////////////////
//////  ///////////////////////////////////////////////
//////  ///////////                         ///////////
//////  ///////////                         ///////////
//\        ////////          SETUP          ///////////
///\      /////////                         ///////////
////\    //////////                         ///////////
/////\  ///////////////////////////////////////////////
///////////////////////////////////////////////////////
void setup() {
  
  // Starting Serial
    Serial.begin(9600);

// ADC1
  // PinMode definition
    pinMode(pd_sck1, OUTPUT);
    pinMode(dout1, INPUT_PULLUP);
    
// ADC2
  // PinMode definition
    pinMode(pd_sck2, OUTPUT);
    pinMode(dout2, INPUT_PULLUP);

// ADC3
  // PinMode definition
    pinMode(pd_sck3, OUTPUT);
    pinMode(dout3, INPUT_PULLUP);

  // gain calculation
  switch (gain) {
    case 128:   // channel A, gain factor 128
      GAIN = 1;
      break;
    case 64:    // channel A, gain factor 64
      GAIN = 3;
      break;
    case 32:    // channel B, gain factor 32
      GAIN = 2;
      break;
  }
}
///////////////////////////////////////////////////////
///  //////////////////////////////////////////////////
///  //////////////////////////////////////////////////
///  /// ///////////                        ///////////
///  ///   /////////                        ///////////
///          ///////      END OF SETUP      ///////////
////////   /////////                        ///////////
//////// ///////////                        ///////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//////  ///////////////////////////////////////////////
//////  ///////////////////////////////////////////////
//////  ///////////                         ///////////
//////  ///////////                         ///////////
//\        ////////           LOOP          ///////////
///\      /////////                         ///////////
////\    //////////                         ///////////
/////\  ///////////////////////////////////////////////
///////////////////////////////////////////////////////
void loop() {

/// HX711 ADC1 read
    
      // Wait for the chip to become ready.
      // This is a blocking implementation and will
      // halt the sketch until a load cell is connected.
      
      while (digitalRead(dout1) != LOW) { 
      
        delay(delay_ms);
    
      }
    
    // Define structures for reading data into.
      unsigned long value = 0;
      uint8_t data[3] = { 0 };
      uint8_t filler = 0x00;
    
      // Protect the read sequence from system interrupts.  If an interrupt occurs during
      // the time the pd_sck signal is high it will stretch the length of the clock pulse.
      // If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
      // power down mode during the middle of the read sequence.  While the device will
      // wake up when pd_sck goes low again, the reset starts a new conversion cycle which
      // forces dout high until that cycle is completed.
      //
      // The result is that all subsequent bits read by shiftIn() will read back as 1,
      // corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
      // interrupts during the sequence and then restores the interrupt mask to its previous
      // state after the sequence completes, insuring that the entire read-and-gain-set
      // sequence is not interrupted.  The macro has a few minor advantages over bracketing
      // the sequence between `noInterrupts()` and `interrupts()` calls.
    noInterrupts();
    
      // Pulse the clock pin 24 times to read the data.
      data[2] = shiftIn(dout1, pd_sck1, MSBFIRST);
      data[1] = shiftIn(dout1, pd_sck1, MSBFIRST);
      data[0] = shiftIn(dout1, pd_sck1, MSBFIRST);
    
      // Set the channel and the gain factor for the next reading using the clock pin.
      for (unsigned int i = 0; i < GAIN; i++) {
        digitalWrite(pd_sck1, HIGH);
        digitalWrite(pd_sck1, LOW);
      }
    
    
      interrupts();
    
      
      // Replicate the most significant bit to pad out a 32-bit signed integer
      if (data[2] & 0x80) {
        filler = 0xFF;
      } else {
        filler = 0x00;
      }
    
        // Construct a 32-bit signed integer
      value = ( static_cast<unsigned long>(filler) << 24
          | static_cast<unsigned long>(data[2]) << 16
          | static_cast<unsigned long>(data[1]) << 8
          | static_cast<unsigned long>(data[0]) );
    
    
          // value is wat we care about
    
    unsigned long value1 = 0;
    value1 = value;

//// Done with ADC1

/// HX711 ADC2 read
    
      // Wait for the chip to become ready.
      // This is a blocking implementation and will
      // halt the sketch until a load cell is connected.
      
      while (digitalRead(dout2) != LOW) { 
      
        delay(delay_ms);
    
      }
    
    // Define structures for reading data into. /// we don't redeclare as it has been done before
      value = 0;
      data[3] = { 0 };
      filler = 0x00;
    
      // Protect the read sequence from system interrupts.  If an interrupt occurs during
      // the time the pd_sck signal is high it will stretch the length of the clock pulse.
      // If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
      // power down mode during the middle of the read sequence.  While the device will
      // wake up when pd_sck goes low again, the reset starts a new conversion cycle which
      // forces dout high until that cycle is completed.
      //
      // The result is that all subsequent bits read by shiftIn() will read back as 1,
      // corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
      // interrupts during the sequence and then restores the interrupt mask to its previous
      // state after the sequence completes, insuring that the entire read-and-gain-set
      // sequence is not interrupted.  The macro has a few minor advantages over bracketing
      // the sequence between `noInterrupts()` and `interrupts()` calls.
    noInterrupts();
    
      // Pulse the clock pin 24 times to read the data.
      data[2] = shiftIn(dout2, pd_sck2, MSBFIRST);
      data[1] = shiftIn(dout2, pd_sck2, MSBFIRST);
      data[0] = shiftIn(dout2, pd_sck2, MSBFIRST);
    
      // Set the channel and the gain factor for the next reading using the clock pin.
      for (unsigned int i = 0; i < GAIN; i++) {
        digitalWrite(pd_sck2, HIGH);
        digitalWrite(pd_sck2, LOW);
      }
    
    
      interrupts();
    
      
      // Replicate the most significant bit to pad out a 32-bit signed integer
      if (data[2] & 0x80) {
        filler = 0xFF;
      } else {
        filler = 0x00;
      }
    
        // Construct a 32-bit signed integer
      value = ( static_cast<unsigned long>(filler) << 24
          | static_cast<unsigned long>(data[2]) << 16
          | static_cast<unsigned long>(data[1]) << 8
          | static_cast<unsigned long>(data[0]) );
    
    
          // value is wat we care about
    
    unsigned long value2 = 0;
    value2 = value;

//// Done with ADC2

/// HX711 ADC2 read
    
      // Wait for the chip to become ready.
      // This is a blocking implementation and will
      // halt the sketch until a load cell is connected.
      
      while (digitalRead(dout3) != LOW) { 
      
        delay(delay_ms);
    
      }
    
    // Define structures for reading data into. /// we don't redeclare as it has been done before
      value = 0;
      data[3] = { 0 };
      filler = 0x00;
    
      // Protect the read sequence from system interrupts.  If an interrupt occurs during
      // the time the pd_sck signal is high it will stretch the length of the clock pulse.
      // If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
      // power down mode during the middle of the read sequence.  While the device will
      // wake up when pd_sck goes low again, the reset starts a new conversion cycle which
      // forces dout high until that cycle is completed.
      //
      // The result is that all subsequent bits read by shiftIn() will read back as 1,
      // corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
      // interrupts during the sequence and then restores the interrupt mask to its previous
      // state after the sequence completes, insuring that the entire read-and-gain-set
      // sequence is not interrupted.  The macro has a few minor advantages over bracketing
      // the sequence between `noInterrupts()` and `interrupts()` calls.
    noInterrupts();
    
      // Pulse the clock pin 24 times to read the data.
      data[2] = shiftIn(dout3, pd_sck3, MSBFIRST);
      data[1] = shiftIn(dout3, pd_sck3, MSBFIRST);
      data[0] = shiftIn(dout3, pd_sck3, MSBFIRST);
    
      // Set the channel and the gain factor for the next reading using the clock pin.
      for (unsigned int i = 0; i < GAIN; i++) {
        digitalWrite(pd_sck3, HIGH);
        digitalWrite(pd_sck3, LOW);
      }
    
    
      interrupts();
    
      
      // Replicate the most significant bit to pad out a 32-bit signed integer
      if (data[2] & 0x80) {
        filler = 0xFF;
      } else {
        filler = 0x00;
      }
    
        // Construct a 32-bit signed integer
      value = ( static_cast<unsigned long>(filler) << 24
          | static_cast<unsigned long>(data[2]) << 16
          | static_cast<unsigned long>(data[1]) << 8
          | static_cast<unsigned long>(data[0]) );
    
    
          // value is wat we care about
    
    unsigned long value3 = 0;
    value3 = value;

//// Done with ADC3

/// Spiting out all data collected during that round to serial

Serial.print("{");
Serial.print("\"ADC1:\"");
Serial.print(value1);
Serial.print(",");
Serial.print("\"ADC2:\"");
Serial.print(value2);
Serial.print(",");
Serial.print("\"ADC3:\"");
Serial.print(value3);
Serial.print("}");
Serial.println("");

}
///////////////////////////////////////////////////////
///  //////////////////////////////////////////////////
///  //////////////////////////////////////////////////
///  /// ///////////                        ///////////
///  ///   /////////                        ///////////
///          ///////       END OF LOOP      ///////////
////////   /////////                        ///////////
//////// ///////////                        ///////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
