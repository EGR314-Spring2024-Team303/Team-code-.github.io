
// include header files 
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart2.h"
#include "mcc_generated_files/i2c1_master.h"
#include "mcc_generated_files/examples/i2c1_master_example.h"
#include "mcc_generated_files/spi2.h"

//defining sensors address for the Atmospheric Pressure and Temp Sensors
#define SM9543_ADDRESS      0x28
#define TC74_I2C_SLAVE_ADDR 0x4C

// defining 8 bit data for motor direction control in SPI protocol
uint8_t fwd = 0b11101111;
uint8_t bckwd = 0b11101101; 
uint8_t data;

// defining data address variable for Temperature sensors in I2C protocol in 16bit
uint16_t sensor_val_1 = 0;
// defining data address variable for Atmospheric Pressure   sensors in I2C protocol in 16bit
uint16_t sensor_val_2 = 0;

// flash Red and Blue lights void function
void flashLights() {
    for (int i = 0; i < 10; i++) {
        RED_SetHigh();
        __delay_ms(500);
        RED_SetLow();
        BLUE_SetHigh();
        __delay_ms(500);
        BLUE_SetLow();
    }
}
/*
                         Main application
 */
void main(void)
{
    // Initialize the device
        SYSTEM_Initialize();     
    // Initialize UART
        EUSART1_Initialize();
        EUSART2_Initialize();
    // Initialize I2C & SPI
        I2C1_Initialize();
        SPI2_Initialize();
    // open SPI 
        SPI2_Open(SPI2_DEFAULT);
        
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
        
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    int Dir = 0; // Initializes Direction of the motor to be 0 (Down )
    // amount of loop for the ESP32 on Atmospheric 
    int ii = 0 ; 
    //run count on ATM 
    float read_count; 
    // amount of loop for ESP 32 on Temp 
    int i = 0 ; 
    //run count on TEMP
    float read_count2;
    
    
    while(1)
    {
        //Motor Diver code
        // Read the data from the temp sensors
        sensor_val_1 = I2C1_Read1ByteRegister(TC74_I2C_SLAVE_ADDR,0x00);
        
        if (Dir==0){ // Raising Flag Condition   
            if (sensor_val_1 > 35 ){
              
            IO_RB4_SetLow();                      //reset CSN 
            data = SPI2_ExchangeByte(fwd);        //Declare Direction
            IO_RB4_SetHigh();                     // set CSN high (turn on) 
            __delay_ms(1000);                     // Motor Duration
            IO_RB4_SetLow();                      // set CSN low (turn off)
            flashLights();                        // flash red and blue lights
            __delay_ms(5000);                     // Delay
            Dir=1;                                // setting conditions to stop further attempts at raising the flag higher 
            printf(" Dir = %d ",Dir);                           
            }
        }
        else { //Setting flag back to initial position
            if(sensor_val_1 < 35 ){
               
                IO_RB4_SetLow();                  // Reset CSN
                data = SPI2_ExchangeByte(bckwd);  // Declare Direction
                IO_RB4_SetHigh();                 // Set CSN high (Turns on) 
                __delay_ms(1000);                 // Motor Duration
                IO_RB4_SetLow();                  // Set CSN Low  (Turn off)
                Dir=0;                            // Setting conditions to stop further attempts at lowering the flag
                __delay_ms(5000);                 // Delay                   
                printf(" Dir = %d ",Dir);
            }
         }
           
       sensor_val_1 = I2C1_Read1ByteRegister(TC74_I2C_SLAVE_ADDR,0x00);  // Read the data from the temp sensor        
       sensor_val_2 = I2C1_Read2ByteRegister( SM9543_ADDRESS,0x00 );     // Read the data from the atmospheric pressure sensors
       
       
           // use LED to check if atmospheric pressure sensor got blocked
            if (sensor_val_2==0){
                LED_SetHigh();
                flashLights();
            }
            else{
                LED_SetLow();  
            } 
       
       // ESP32 Data Exchange 
       if(ii<100)
       {
           ii++;
           read_count=ii/10;
           // print atmospheric pressure sensor value to  terminal 
           printf(" Atm (mBar) = %d ",sensor_val_2/8);
       }
       else 
       {
           ii=0;
       }
       if (i < 100)
       {
          i++;
          read_count2= i/10;
          // print Temp sensor value to  terminal
          printf("Temp(c) = %d \r",sensor_val_1);
       }
       else
       {
           i =0;
       }
   }
}
/**
 End of File
*/


