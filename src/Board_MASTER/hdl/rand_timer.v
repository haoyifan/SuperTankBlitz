module rand_timer( PCLK,
                   PENABLE,
                   PSEL,
                   PRESETN,
                   PWRITE,
                   PREADY,
                   PSLVERR,
                   PADDR,
                   PWDATA,
                   PRDATA,
                    );
//APB interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output reg [31:0] PRDATA;
output PREADY, PSLVERR;

assign PSLVERR = 0;
assign PREADY = 1;

reg [31:0]timer;

wire BUS_READ_EN;
assign BUS_READ_EN = (!PWRITE && PSEL && PENABLE);

always@(posedge PCLK)
begin
    if(~PRESETN) // reset
        timer <= 0;
    else
        timer <= timer + 1;
        if(BUS_READ_EN)
            PRDATA <= timer;
end

endmodule