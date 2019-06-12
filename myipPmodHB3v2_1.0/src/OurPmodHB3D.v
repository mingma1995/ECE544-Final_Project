`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/05/03 20:36:45
// Design Name: 
// Module Name: OurPmodHB3D
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module OurPmodHB3D(
    input Clk,
    input SA,
    input [6:0] duty_cycle,
    output Dir,
    output En,
    output [31:0] pulse_count
    );
    reg [6:0] pwm_counter = 7'd0;
    reg [6:0] duty_cycle_pre = 7'd0;
    reg [26:0] counter_1s = 27'd0;
    reg SA_pre = 1'd0;
    reg [31:0] pulse_count_reg = 32'd0;
    reg [31:0] pulse_reg;
    
    //PWM Counter
    always @(posedge Clk) begin
    if(duty_cycle != duty_cycle_pre) begin
    pwm_counter <= 7'd0;
    duty_cycle_pre <= duty_cycle;
    end
    else begin
    pwm_counter <= pwm_counter + 1'b1;
    if(pwm_counter >= 7'd99)
    pwm_counter <= 7'd0;
    end
    end
    
    assign En = (pwm_counter < duty_cycle) ? 1:0;
    
    //Count for 1s
     always @(posedge Clk) begin
     counter_1s <= counter_1s + 1'b1;
     if(counter_1s >=27'd99999999)
     counter_1s <= 27'd0;
     end
     
     
     always @(posedge Clk) begin
     if(counter_1s == 27'd99999999) begin
     pulse_reg <= pulse_count_reg;
     pulse_count_reg <= 32'd0;     
     end
     else if(SA) begin
     if(SA_pre != SA) begin
     SA_pre <= SA;
     pulse_count_reg <= pulse_count_reg + 1'b1;    
     end
     end
     else if(!SA) begin
     if(SA_pre != SA) begin
     SA_pre <= SA;     
     end
     end
     end
     
     assign pulse_count = pulse_reg;
endmodule
