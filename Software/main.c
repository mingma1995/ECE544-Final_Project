/*
 * 	main.c for microblaze running on Nexys A7
 * 	FPGA target board
 *
 *
 * 	Created By:		Zhe Lu
 * 	Last Modified:	06-13-2019
 *
 * 	Description:
 *  ============
 *  This program is the car control system based on the standalone. 
 *  First it reads the control signal via UART communication with NODEMCU which is a WIFI module for reading data from firebase.
 *  Then based on these control signals, it will control the movement of the car by give the motor different rotation direction.
 *	Also, we have a distance function that we can read the distance value and we check it every movement function, if the distance is
 *	less than 15 center meter, stop the car immediate.
 *  
*/

#include "xparameters.h"
#include "xil_types.h"
#include "xil_io.h"
#include "sleep.h"
#include "xuartlite.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xgpio.h"

/************************** Function Prototypes *****************************/
void Move_FWD();
void Move_BACK();
void Move_RIGHT();
void Move_LEFT();
void STOP();
u32 get_Distance(u32 BASEADDR);
/************************** Constant Definitions ****************************/
// Definitions for UART transmission
#define Received_Buffer_size 	1
#define UARTLITE_DEVICE_ID_RX      XPAR_UARTLITE_1_DEVICE_ID
#define UARTLITE_DEVICE_ID_TX      XPAR_UARTLITE_2_DEVICE_ID
// Definitions for GPIO
#define GPIO_0_DEVICE_ID		   XPAR_GPIO_0_DEVICE_ID
#define GPIO_0_OUTPUT_0_CHANNEL		1							//bits[2:0] of this channel store RGB value from HW
#define GPIO_0_OUTPUT_1_CHANNEL		2
#define GPIO_1_DEVICE_ID		   XPAR_GPIO_1_DEVICE_ID
#define GPIO_1_OUTPUT_0_CHANNEL		1							//bits[2:0] of this channel store RGB value from HW
#define GPIO_1_OUTPUT_1_CHANNEL		2
// Definitions for Distance
#define DISTANCE_SENSOR_FRONT	XPAR_OURDISTANCESENSORV4_1_S00_AXI_BASEADDR
#define DISTANCE_SENSOR_BACK	XPAR_OURDISTANCESENSORV4_0_S00_AXI_BASEADDR
#define time_clock				10000000
XGpio		GPIOInst0;			//The instance of the GPIO0
XGpio		GPIOInst1;			//The instance of the GPIO1
XUartLite UartLite_RX;          //The instance of the UartLite Device for Receiving
XUartLite UartLite_TX;			//The instance of the UartLite Device for Receiving

/*
 * The following buffers are used to send and receive data
 * with the UartLite.
 */
u8 SendBuffer[Received_Buffer_size];
u8 ReceiveBuffer[Received_Buffer_size];


 /*
 * The following variables are used for distance sensor signal to stop the or run the car
 */
volatile int distance_front = 0;
volatile int distance_back = 0;

int main(){

	int Status;
	int Received_control = 0;
	/*
	 * Initialize the UartLite driver so that it is ready to use.
	 */

Status = XUartLite_Initialize(&UartLite_RX, UARTLITE_DEVICE_ID_RX);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XUartLite_Initialize(&UartLite_TX, UARTLITE_DEVICE_ID_TX);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLite_RX);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XUartLite_SelfTest(&UartLite_TX);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
		/*
	 * Initialize the GPIO driver so that it is ready to use.
	 */
	Status = XGpio_Initialize(&GPIOInst0, GPIO_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	Status = XGpio_Initialize(&GPIOInst1, GPIO_1_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	// GPIO0 channel 0 is an 8-bit output port.
	// GPIO0 channel 1 is an 8-bit output port.
	XGpio_SetDataDirection(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0x00);
	XGpio_SetDataDirection(&GPIOInst0, GPIO_0_OUTPUT_1_CHANNEL, 0x00);
	// GPIO1 channel 0 is an 8-bit output port.
	// GPIO1 channel 1 is an 8-bit output port.
	XGpio_SetDataDirection(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 0x00);
	XGpio_SetDataDirection(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 0x00);
	// Turn off all LEDs when it starts
	XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
	XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_1_CHANNEL, 0);
	XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 0);
	XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 0);

	while(1)
	{
		// Check the Received control signal from firebase through UART
		XUartLite_Recv(&UartLite_RX, ReceiveBuffer, Received_Buffer_size);
		Received_control = ReceiveBuffer[0];
		// If the control signal equals 0
		if(Received_control == 0)
		{
			// Turn off alert
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
		}
		// If the control signal equals 1
		else if(Received_control == 1)
		{
			// Turn off alert
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
			SendBuffer[0] = 0;
			//Call move forward function 
			Move_FWD();
			//Reset the firebase 
			XUartLite_Send(&UartLite_TX, SendBuffer, Received_Buffer_size);
		}
		// If the control signal equals 2
		else if(Received_control == 2)
		{
			// Turn off alert
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
			SendBuffer[0] = 0;
			// Turn on back two LEDs
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 1);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 1);
			//Call move backward function 			
			Move_BACK();
			// Turn off back two LEDs
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 0);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 0);
			//Reset the firebase
			XUartLite_Send(&UartLite_TX, SendBuffer, Received_Buffer_size);
		}
		// If the control signal equals 3
		else if(Received_control == 3)
		{
			// Turn off alert
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
			SendBuffer[0] = 0;
			// Turn on left two LEDs
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 1);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 1);
			//Call turn left function
			Move_LEFT();
			// Turn off left two LEDs
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_0_CHANNEL, 0);
			//Reset the firebase
			XUartLite_Send(&UartLite_TX, SendBuffer, Received_Buffer_size);
		}
		// If the control signal equals 4
		else if(Received_control == 4)
		{
			// Turn off alert
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
			SendBuffer[0] = 0;
			// Turn on left two LEDs
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_1_CHANNEL, 1);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 1);
			//Call turn right function
			Move_RIGHT();
			// Turn off left two LEDs
			XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_1_CHANNEL, 0);
			XGpio_DiscreteWrite(&GPIOInst1, GPIO_1_OUTPUT_1_CHANNEL, 0);
			//Reset the firebase
			XUartLite_Send(&UartLite_TX, SendBuffer, Received_Buffer_size);
		}
	}
}
/*********************** MOVEMENT-RELATED FUNCTIONS ***********************************/
/****************************************************************************/
/**
* Control the motor to make the car goes forward, also check front distance sensor
* to stop the car immediate.
* @return  *NONE*
*
*****************************************************************************/
void Move_FWD()
{
	//Read the distance value
	distance_front = get_Distance(DISTANCE_SENSOR_FRONT);
	// if the distance less than 15 center meter
	if(distance_front < 15)
	{
		//play alret and stop the car
		XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0x80);
		STOP();
	}
	//otherwise, goes forward
	else
	{
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR+4, 0x00000001);
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR+4, 0x00000001);
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	}
	usleep(1000);
	STOP();
}
/****************************************************************************/
/**
* Control the motor to make the car goes back, also check back distance sensor
* to stop the car immediate.
* @return  *NONE*
*
*****************************************************************************/
void Move_BACK()
{
	//Read the distance value
	distance_back = get_Distance(DISTANCE_SENSOR_BACK);
	// if the distance less than 15 center meter
	XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0);
	if(distance_back < 15)
	{
				//play alret and stop the car
		XGpio_DiscreteWrite(&GPIOInst0, GPIO_0_OUTPUT_0_CHANNEL, 0x80);
		STOP();
	}
		//otherwise, goes backward
	else
	{
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR+4, 0x00000000);
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR+4, 0x00000000);
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	}
	usleep(1000);
	STOP();
}
/****************************************************************************/
/**
* Control the motor to make the car turns right.
* 
* @return  *NONE*
*
*****************************************************************************/
void Move_RIGHT()
{
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR+4, 0x00000001);
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR+4, 0x00000000);
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR, 0x00000000); //write to slv_reg0[6:0](duty_cycle)
	usleep(1000);
	STOP();
}
/****************************************************************************/
/**
* Control the motor to make the car turns left.
* 
* @return  *NONE*
*
*****************************************************************************/
void Move_LEFT()
{

	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR+4, 0x00000000);
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR, 0x00000000); //write to slv_reg0[6:0](duty_cycle)
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR+4, 0x00000001);
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR, 0x00000050); //write to slv_reg0[6:0](duty_cycle)
	usleep(1000);
	STOP();
}
/****************************************************************************/
/**
* Control the motor to make the car stops.
* 
* @return  *NONE*
*
*****************************************************************************/
void STOP()
{
	Xil_Out32(XPAR_MYIPPMODHB3V2_1_S00_AXI_BASEADDR, 0x00000000); //write to slv_reg0[6:0](duty_cycle)
	Xil_Out32(XPAR_MYIPPMODHB3V2_0_S00_AXI_BASEADDR, 0x00000000); //write to slv_reg0[6:0](duty_cycle)
}
/****************************************************************************/
/**
* @param   BASEADDR is the u32 address to select the front distance sensor or back one.
* 
* @return  The distance between the car and the object.
*
*****************************************************************************/
u32 get_Distance(u32 BASEADDR){
	u32 distance_count;
	u32 distance;
	distance_count = Xil_In32(BASEADDR);
	//equation use to caclualte distance
	distance = (distance_count * 340 * 100) / (time_clock * 2);
	return distance;
}

