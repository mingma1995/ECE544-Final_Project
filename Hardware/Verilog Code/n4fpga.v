`timescale 1ns / 1ps

// n4fpga.v - Top level module for ECE 544 Final Project
//
// Copyright: Portland State University, 2019
// 
// Created By:	Ming Ma, Zhe Lu
// Date:		6/11/2019
// Version:		1.0
//
// Description:
// ------------
// This is the top module for ECE 544 Final Project: Android & Gestures controlled car 
//////////////////////////////////////////////////////////////////////
module n4fpga(
    input				     clk,			// 100Mhz clock input
	input				     btnCpuReset,	// CPU reset pushbutton
    input				     uart_rtl_rxd,	// USB UART Rx and Tx on Nexys 4
    output				     uart_rtl_txd,	
    inout     [2:0]          JA,
	                                    // both rows are used 
    inout	  [7:0] 		 JC,	    // JC Pmod connector
	inout	  [2:0]		     JD,	    // JD Pmod connector
	output    [3:0]          LED,
	output    [12:0]         ddr2_sdram_addr, //DDR2 Memory ports
    output    [2:0]          ddr2_sdram_ba,
    output                   ddr2_sdram_cas_n,
    output    [0:0]          ddr2_sdram_ck_n,
    output    [0:0]          ddr2_sdram_ck_p,
    output    [0:0]          ddr2_sdram_cke,
    output    [0:0]          ddr2_sdram_cs_n,
    output    [1:0]          ddr2_sdram_dm,
    inout     [15:0]         ddr2_sdram_dq,
    inout     [1:0]          ddr2_sdram_dqs_n,
    inout     [1:0]          ddr2_sdram_dqs_p,
    output    [0:0]          ddr2_sdram_odt,
    output                   ddr2_sdram_ras_n,
    output                   ddr2_sdram_we_n,
    output                   AUD_PWM,	//Port to control the amplifier to play sound
    output                   AUD_SD		//Shutdown signal. Assign this port to 1
);

// internal variables
wire               ext_uart_rxd;	//Entra uart to send and receive signals from NodeMCU32
wire               ext_uart_txd;
wire               ext_uart2_rxd;
wire               ext_uart2_txd;
wire               echo_front;	//Front distance sensor echo port
wire               trig_front;	//Front distance sensor trig port
wire               echo_back;	//Back distance sensor echo port
wire               trig_back;	//Back distance sensor trig port
// Clock and Reset 
wire				sysclk;     //Wire to connect system clock        
wire				sysreset_n, sysreset;	//Wire to connect negative edge system reset and positive edge system reset.

wire               Dir_0;	//First group of PmodHB3 ports
wire               En_0;
wire               SA_0;
wire               Dir_1;	//Second group of PmodHB3 ports
wire               En_1;
wire               SA_1;


wire [7:0]         GPIO2_0_tri_o; //GPIOs to control 4 LEDs.
wire [7:0]         GPIO_0_tri_o;
wire [7:0]         GPIO3_0_tri_o;
wire [7:0]         GPIO4_0_tri_o;


// make the connections
// system-wide signals
assign sysclk = clk;
assign sysreset_n = btnCpuReset;		// The CPU reset pushbutton is asserted low.  The other pushbuttons are asserted high
										// but the Microblaze for Nexys 4 expects reset to be asserted low
assign sysreset = ~sysreset_n;			// Generate a reset signal that is asserted high for any logic blocks expecting it.

//JC Connectors
assign JC[2] = trig_front;
assign JC[3] = echo_front;
assign JC[4] = trig_back;
assign JC[5] = echo_back;
assign JC[0] = ext_uart_rxd;
assign JC[1] = ext_uart_txd;
assign JC[6] = ext_uart2_rxd;
assign JC[7] = ext_uart2_txd;

//JA Connectors
assign JA[0] = Dir_0;
assign JA[1] = En_0;
assign JA[2] = SA_0;

//JD Connectors
assign JD[0] = Dir_1;
assign JD[1] = En_1;
assign JD[2] = SA_1;

assign LED[0] = {1'b0, GPIO_0_tri_o[6:0]};
assign LED[1] = GPIO2_0_tri_o;
assign LED[2] = GPIO3_0_tri_o;
assign LED[3] = GPIO4_0_tri_o;


//Instantiate the embedded system
  embsys embsys_i
     (
      .Dir_0(Dir_0),
      .Dir_1(Dir_1),
      .En_0(En_0),
      .En_1(En_1),
      .GPIO2_0_tri_o(GPIO2_0_tri_o),
      .GPIO3_0_tri_o(GPIO3_0_tri_o),
      .GPIO4_0_tri_o(GPIO4_0_tri_o),
      .GPIO_0_tri_o(GPIO_0_tri_o),
      .SA_0(SA_0),
      .SA_1(SA_1),
      .ddr2_sdram_addr(ddr2_sdram_addr),
      .ddr2_sdram_ba(ddr2_sdram_ba),
      .ddr2_sdram_cas_n(ddr2_sdram_cas_n),
      .ddr2_sdram_ck_n(ddr2_sdram_ck_n),
      .ddr2_sdram_ck_p(ddr2_sdram_ck_p),
      .ddr2_sdram_cke(ddr2_sdram_cke),
      .ddr2_sdram_cs_n(ddr2_sdram_cs_n),
      .ddr2_sdram_dm(ddr2_sdram_dm),
      .ddr2_sdram_dq(ddr2_sdram_dq),
      .ddr2_sdram_dqs_n(ddr2_sdram_dqs_n),
      .ddr2_sdram_dqs_p(ddr2_sdram_dqs_p),
      .ddr2_sdram_odt(ddr2_sdram_odt),
      .ddr2_sdram_ras_n(ddr2_sdram_ras_n),
      .ddr2_sdram_we_n(ddr2_sdram_we_n),
      .echo_0(echo_front),
      .echo_1(echo_back),
      .ext_uart_rxd(ext_uart_rxd),
      .ext_uart_txd(ext_uart_txd),
      .reset(sysreset_n),
      .sys_clock(sysclk),
      .trig_0(trig_front),
      .trig_1(trig_back),
      .usb_uart_rxd(uart_rtl_rxd),
      .usb_uart_txd(uart_rtl_txd),
      .ext_uar2_rxd(ext_uart2_rxd),
      .ext_uar2_txd(ext_uart2_txd));
        

//Instantiate alert module to play alert.
alert alert(
    .clk(clk), //100Mhz system clock
    .reset(sysreset_n), //System reset
    .enable1(GPIO_0_tri_o[7]), //Enable signal to play the alert.
    .AUD_PWM(AUD_PWM), //Port to control the amplifier to play sound
    .AUD_SD(AUD_SD) //Shutdown signal. Assign this port to 1
    );
   
endmodule

