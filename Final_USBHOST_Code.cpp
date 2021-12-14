#include <cdcacm.h>
#include <usbhub.h>

#include "pgmstrings.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

class ACMAsyncOper : public CDCAsyncOper
{
public:
    uint8_t OnInit(ACM *pacm);
};

uint8_t ACMAsyncOper::OnInit(ACM *pacm)
{
    uint8_t rcode;
    // Set DTR = 1 RTS=1
    rcode = pacm->SetControlLineState(3);

    if (rcode)
    {
        ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
        return rcode;
    }

    LINE_CODING	lc;
    lc.dwDTERate  = 9600;
    //lc.dwDTERate	= 115200;
    lc.bCharFormat	= 0;
    lc.bParityType	= 0;
    lc.bDataBits	= 8;

    rcode = pacm->SetLineCoding(&lc);

    if (rcode)
        ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);

    return rcode;
}

USB     Usb;
//USBHub     Hub(&Usb);
ACMAsyncOper  AsyncOper;
ACM           Acm(&Usb, &AsyncOper);

void setup()
{
  Serial.begin( 9600 );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
      Serial.println("OSCOKIRQ failed to assert");

  delay( 200 );
}

void loop()
{
  /* DEBUG 
  if (Usb.getUsbTaskState() == USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE) {
    Serial.println("USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE");
  }
  else {
   Serial.println(Usb.getUsbTaskState());
  }
   END DEBUG */
    Usb.Task();
   //Serial.println(Acm.isReady());
    if( Acm.isReady()) {
       uint8_t rcode;
      // Serial.println(rcode);
       /* reading the keyboard */
       if(Serial.available()) {
         uint8_t data= Serial.read();
         //Serial.println("Read");
         /* sending to the phone */
         rcode = Acm.SndData(1, &data);
        // Serial.println(rcode);
         
         if (rcode)
            ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
           // Serial.println(rcode);
       }//if(Serial.available()...

       delay(50);

        /* reading the phone */
        /* buffer size must be greater or equal to max.packet size */
        /* it it set to 64 (largest possible max.packet size) here, can be tuned down
        for particular endpoint */
        uint8_t  buf[64];
        for (uint8_t i=0; i<64; i++)
          buf[i] = 0;
        uint16_t rcvd = 64;
        rcode = Acm.RcvData(&rcvd, buf);
        //Serial.println(rcode);
         if (rcode && rcode != hrNAK)
            ErrorMessage<uint8_t>(PSTR("Ret"), rcode);

            
           // Serial.println(rcvd);
            if( rcvd ) { //more than zero bytes received
             // Serial.print("More than zero");
              for(uint16_t i=0; i < rcvd; i++ ) {
                Serial.println((char)buf[i]); //printing on the screen
              }
            }
        //delay(10);
    }//if( Usb.getUsbTaskState() == USB_STATE_RUNNING..
}

