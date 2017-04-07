module servo(
                PENABLE,
				PSEL,
				PRESETN,
				PWRITE,
				PREADY,
				PSLVERR,
				PADDR,
				PWDATA,
				PRDATA,
                PCLK,
                pwm
);

// APB Bus Interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;

assign PREADY = 1'b1;
assign PSLVERR = 1'b0;

output reg pwm;

wire BUS_WRITE_EN;
assign BUS_WRITE_EN = (PWRITE && PSEL && PENABLE);

reg [31:0] pulseWidth;

assign PRDATA = pulseWidth;

always@(posedge PCLK)
begin
   if (BUS_WRITE_EN) begin // WRITE pulseWidth
        pulseWidth <= PWDATA;
   end
end

`define period 2000000
reg [31:0] count;

always @(posedge PCLK)
begin
 if (~PRESETN) begin
    pwm <= 0;
    count <= 0;
 end
 else if (count == `period)
    count <= 0;
 else
    count <= count + 1;
 if (count < pulseWidth)
    pwm <= 1;
 else
    pwm <= 0;
end

endmodule

