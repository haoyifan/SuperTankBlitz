///////////////////////////////////////////////////////////////////////////////////////////////////
// Company: <Name>
//
// File: audio.v
// File history:
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//
// Description: 
//
// <Description here>
//
// Targeted device: <Family::SmartFusion> <Die::A2F200M3F> <Package::484 FBGA>
// Author: <Name>
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 
`define RESET_STATE 2'b0
`define WAITING_STATE 2'b01
`define SENDING_STATE 2'b10

module audio(
/*** APB3 BUS INTERFACE ***/
input PCLK, // clock
input PRESERN, // system reset
input PSEL, // peripheral select
input PENABLE, // distinguishes access phase
output wire PREADY, // peripheral ready signal
output wire PSLVERR, // error signal
input PWRITE, // distinguishes read and write cycles
input [31:0] PADDR, // I/O address
input wire [31:0] PWDATA, // data from processor to I/O device (32 bits)
output reg [31:0] PRDATA, // data to processor from I/O device (32-bits)
/*** I/O PORTS DECLARATION ***/
output reg data, // port to LED
output reg clk,
output reg reset
//output [1:0]state,
//output reg led
); 
assign PSLVERR = 0;
assign PREADY = 1;
reg [1:0]state;
reg sending;
reg led;
reg [4:0]data_bit;
reg sending_begin;
reg [31:0]reset_count;
reg [31:0]sending_count;
reg [31:0]cmd;
//////////////////////////////////
// end of APB3 interface header //
//////////////////////////////////
wire busy;
assign busy = 1;
//As the only thing we read is the switch, we can constantly drive it!
always @(posedge PCLK) // register control for APB3 writes
begin
    if(!PRESERN) begin
        clk <= 1;
        data <= 1;
        reset <= 1;
        state <= 2'b0;
        reset_count <= 32'b0;
    end
    else begin
        if(PWRITE && PENABLE && PSEL) begin
            cmd <= PWDATA;
            sending <= 1;
            led <= 0;
        end
        if(state == 2'b0) begin
            reset_count <= reset_count + 1;
            if(reset_count == 100000)
                reset <= 0;
            if(reset_count == 700000)
                reset <= 1;
            if(reset_count == 30700000) begin
                clk <= 1;
                data <= 1;
                reset_count <= 0;
                state <= 2'b01;
            end
        end
        if(state == 2'b01) begin
            clk <= 1;
            data <= 1;
            if(sending && busy) begin
                sending_count <= 0;
                state <= 2'b10;
                sending <= 0;
                data_bit <= 0;
                led <= 1;
                clk <= 0;
                sending_begin <= 0;
            end
            else
                state <= 2'b01;
        end
        if(state == 2'b10) begin
            sending_count <= sending_count + 1;
            if(!sending_begin && sending_count == 200000) begin
                sending_begin <= 1;
                sending_count <= 0;
            end
            if(sending_begin && sending_count == 1)
                clk <= 0;
            if(sending_begin && sending_count == 20000)
                data <= cmd[data_bit];
            if(sending_begin && sending_count == 40000)
                clk <= 1;
            if(sending_begin && sending_count == 60000) begin
                sending_count <= 0;
                data_bit <= data_bit + 1;
            end
            if(data_bit == 16)
                state <= 2'b01;
        end
    end
end

endmodule

