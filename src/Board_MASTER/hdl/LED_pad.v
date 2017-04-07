module LED_pad( PCLK,
			   PENABLE,
			   PSEL,
			   PRESETN,
			   PWRITE,
			   PREADY,
			   PSLVERR,
			   PADDR,
			   PWDATA,
			   PRDATA,
               LED
                );
//APB interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;

assign PSLVERR = 0;
assign PREADY = 1;// LED_pad.v


wire BUS_WRITE_EN;
assign BUS_WRITE_EN = (PWRITE && PSEL && PENABLE);

output reg [7:0] LED;

assign PRDATA[7:0] = LED;

always@(posedge PCLK)
begin

    if(~PRESETN)
        LED <= 1;
    else begin
        if(BUS_WRITE_EN)
            LED <= PWDATA[7:0];
    end

end


endmodule