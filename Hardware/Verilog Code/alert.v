`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Portland State University
// Engineer: Ming Ma
// 
// Create Date: 06/11/2019
// Design Name: 
// Module Name: Audio_PWM
// Project Name: ECE 544 Final Project
// Target Devices: Nexys 4 DDR
// Tool Versions: 
// Description: This module will control the FPGA to play some sounds by controlling AUD_PWM and AUD_SD. If audio_data > audio_counter, set AUD_PWM = 1
// Otherwise, set AUD_PWM = 0. The AUD_SD means shutdown and we just assign this port to value 1. 
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module alert(
    input clk, //100Mhz system clock
    input reset, //system reset
    input enable1, //Enable signal to play the alert.
    output AUD_PWM, //Port to control the amplifier to play sound
    output AUD_SD //Shutdown signal. Assign this port to 1
    );
    
    wire [7:0]    audio_data1; //audio data read from distribute memory for alert.
    reg  [7:0]    audio_counter1 = 8'b0; //audio counter for the initial BGM
    reg           AUD_PWM_I = 1'b0;
    reg  [17:0]   S1_counter = 18'b0; //Address counter to control the read data from memory for initial BGM
    parameter integer CLK_FREQUENCY_HZ = 100000000; //100Mhz
    parameter integer UPDATE_FREQUENCY_16KHZ  = 16000; //Value used to calculate the counter number for 16K frequency
    reg  [14:0]   clk_cnt_16Khz; //Counter for generating 16K frequency
    wire [14:0]   top_cnt_16Khz = ((CLK_FREQUENCY_HZ / UPDATE_FREQUENCY_16KHZ) - 1); // max value of counter for 16Khz clock 
    reg           clock16Khz;                // update 16Khz clock enable
    
    
    
    //Generate 16K clock frequency from 100M clock.
    always @(posedge clk or negedge reset) begin
     if (!reset) begin
            clk_cnt_16Khz <= {15{1'b0}};
     end
        
     else if (clk_cnt_16Khz == top_cnt_16Khz) begin
            clock16Khz     <= 1'b1;
            clk_cnt_16Khz <= {15{1'b0}};
     end
        
     else begin
            clock16Khz     <= 1'b0;
            clk_cnt_16Khz <= clk_cnt_16Khz + 1'b1;
     end
    end
    
    //Generate Audio_PWM signal for alert.
    always @(posedge clk or negedge reset) begin
     if(!reset) begin
        AUD_PWM_I <= 1'b0;
        audio_counter1 <= 8'b0;
     end
     else if(enable1) begin
        audio_counter1 <= audio_counter1 + 1'b1;
        if(audio_counter1 >= audio_data1) //Compare the audio data of alert with its audio counter
            AUD_PWM_I <= 1'b0;
        else
            AUD_PWM_I <= 1'b1;
     end
    end
    
	//Control the address counters for reading audio data from memory.
    always @(posedge clock16Khz or negedge reset) begin
     if(!reset) begin
		S1_counter <= 18'b0;
     end
     else if(enable1) 
        begin
        if(S1_counter == 18'd149999)
            S1_counter <= 18'd0;
        else
            S1_counter <= S1_counter + 1'b1;
        end   
    end
    
 //Read alert data from BRAM.
sound Initial_BGM (
.clka(clk),    // input wire clka
.ena(enable1),      // input wire ena
.addra(S1_counter),  // input wire [17 : 0] addra
.douta(audio_data1)  // output wire [7 : 0] douta
 );
 
   
assign AUD_PWM = AUD_PWM_I;
assign AUD_SD = 1'b1; //Assign this port to 1 to enable shutdown music.
    
endmodule