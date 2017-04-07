module HarmShoot( PCLK,
			  PENABLE,
			  PSEL,
			  PRESETN,
			  PWRITE,
			  PREADY,
			  PSLVERR,
			  PADDR,
			  PWDATA,
			  PRDATA,
              shooting,
              hit,
              hit_int
              //LED
);
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;

assign PSLVERR = 0;
assign PREADY = 1;

wire tr;
wire hit_int_temp;

//output [7:0] LED;

output shooting;
input hit;
output hit_int;

assign PSEL1 = PWDATA[3] & PSEL; // shoot addr: 00
assign PSEL2 = ~PWDATA[3] & PSEL; // harm addr: 03
//assign LED = PRDATA[7:0];

/*
if(PADDR == 2'h00 ) 
    PSEL1 = PSEL;
else 
    PSEL1 = 0;
if(PADDR == 2'h03)
    PSEL2 = PSEL;
else 
    PSEL2 = 0;
*/
reg [31:0] timer;
reg state;
reg listen; // harm module working when listen == 1

assign hit_int = listen && hit_int_temp;

always@(posedge PCLK) begin
    if(~PRESETN) begin // reset
        listen <= 1;
        timer <= 0;
        state <= 0;
    end
    else begin
        if(state == 0) begin
            timer <= 0;
            listen <= 1;
            if(tr)
                state <= 1;
        end
        else if(state == 1) begin
            listen <= 0;
            timer <= timer + 1;
            if(timer >= 500000)
                state <= 0;
        end
    end
end
shoot s1(PCLK,PENABLE,PSEL1,PRESETN,PWRITE,PREADY1,PSLVERR1,PADDR,PWDATA, ,shooting, tr);
harmer h1(PCLK,PENABLE,PSEL2,PRESETN,PWRITE,PREADY2,PSLVERR2,PADDR,PWDATA, PRDATA,hit,hit_int_temp,);


endmodule

