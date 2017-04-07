module button(input clk, input b, output int);

wire button_db;

reg int;
debouncer D1 (clk, b, button_db);

reg button1;
reg button2;
reg button3;

always @(posedge clk) begin
    button1 <= button_db;
    button2 <= button1;
    button3 <= button2;
    
    if( button3 == 0 && button2 == 1) begin
        int <= 1;
    end
    else begin
        int <= 0;
    end
end



endmodule

module debouncer(
    input clk, //this is a 50MHz clock provided on FPGA pin PIN_Y2
    input PB,  //this is the input to be debounced
    output reg PB_state  //this is the debounced switch
);
/*This module debounces the pushbutton PB.
 *It can be added to your project files and called as is:
 *DO NOT EDIT THIS MODULE
 */

// Synchronize the switch input to the clock
reg PB_sync_0;
always @(posedge clk) PB_sync_0 <= PB; 
reg PB_sync_1;
always @(posedge clk) PB_sync_1 <= PB_sync_0;

// Debounce the switch
reg [15:0] PB_cnt;
always @(posedge clk)
if(PB_state==PB_sync_1)
    PB_cnt <= 0;
else
begin
    PB_cnt <= PB_cnt + 1'b1;  
    if(PB_cnt == 16'hffff) PB_state <= ~PB_state;  
end
endmodule