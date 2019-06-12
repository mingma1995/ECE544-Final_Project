// Distance Sensor module for altrosonic distance sensor.
// Use 10Mhz clock.
module distance_sensor(
	input clk,	//10Mhz clock
	input echo,	//Echo port to receive reflect signal
	output trig,	//Trig port to enable distance sensor. Set at least 10us.
	output [31:0] distance_count	//Count value used to calculate real distance.
);

reg [31:0] counter;
reg [31:0] dist_counter;
reg trig_reg;

always @(posedge clk) begin
if(counter == 32'd0) 
begin
	trig_reg <= 1'b1;
end
else if(counter == 32'd1000) 
begin //keep trig high at least 10us to start distance sensor
	trig_reg <= 1'b0;
end
end

always @(posedge clk) begin
if(counter < 32'd6500000) //Measurement cycle greater than 60ms
	begin
	counter <= counter + 1'b1;
	if(echo == 1'b1) 
		begin
		dist_counter <= dist_counter + 1'b1;
		end
	end
else 
begin //clear counters
counter <= 32'd0;
dist_counter <= 32'd0;
end
end

assign trig = trig_reg;
assign distance_count = dist_counter;
endmodule