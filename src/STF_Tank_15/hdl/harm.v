module harmer( PCLK,
			   PENABLE,
			   PSEL,
			   PRESETN,
			   PWRITE,
			   PREADY,
			   PSLVERR,
			   PADDR,
			   PWDATA,
			   PRDATA,
			   hit,
               hit_int,
               //hard_hit_int
               LED
               //LED1
                );
input hit;
output hit_int;
//output hard_hit_int;
//output LED, LED1;

//APB interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;

assign PSLVERR = 0;
assign PREADY = 1;

reg [2:0] count;
reg [31:0] state; // the state of the hit tank
reg hit_int; // throw interrupt if hit lightly
reg hard_hit_int; // throw interrupt if hit hard
reg [2:0] hit_sync; // synchronize the hit
reg [31:0] check_if_hard; // a count to check if the hit is hard hit
reg [31:0] check_if_self; // a count to check if is hit by itself
reg [3:0] keep_int; // keep the interrupt line high for 8 cycles
reg [31:0] global_time_out; // to test if the tank is stuck in a specific state
reg [7:0] recv_bit; // which bit is receiving
reg [31:0] result; // the real encoded info

wire BUS_WRITE_EN;
assign BUS_WRITE_EN = (PWRITE && PSEL && PENABLE);

assign PRDATA = result;

reg [31:0]id;

wire hit_temp;
assign hit_temp = ~hit;



wire [7:0]LED;
output [7:0] LED;
reg L;
assign LED[7] = L;
assign LED[2:0] = result[2:0];
assign LED[3] = id[0];
assign LED[4] = hit_int;




always@(posedge PCLK)
begin
hit_sync[0] <= hit_temp;
hit_sync[1] <= hit_sync[0];
hit_sync[2] <= hit_sync[1];
if(PRESETN && BUS_WRITE_EN) begin
    id <= PWDATA;
end
if(~PRESETN)
begin // reset
    hit_int <= 0;
    hard_hit_int <= 0;
    hit_sync <= 3'b000;
    count <= 0;
    check_if_hard <= 0;
    check_if_self <= 0;
    state <= 0;
    keep_int <= 0;
    global_time_out <= 0;
    recv_bit <= 2; // need to change if add bits
end
else if(hit_sync[2] == 0 && hit_sync[1] == 1 && state == 0)
begin // it is hit by someone
    L <= 0;
    check_if_self <= check_if_self + 1;
    state <= 1;
    global_time_out <= 0;
    recv_bit <= 2; // need to change if add bits
end
else if(state == 1)
begin
    if(global_time_out == 100000)
    begin
        state <= 10; // reset state
    end
    global_time_out <= global_time_out + 1;

    if(check_if_self < 35000)
        check_if_self <= check_if_self + 1;
    else if(check_if_self == 35000) begin
        check_if_self <= 0;
        recv_bit <= recv_bit - 1;
        result[recv_bit] <= hit_sync[1];
        if(recv_bit == 2) begin //need to change if add bits
            if(hit_sync[1] == ~id[0]) // myself
            begin
                state <= 0;
                global_time_out <= 0;
            end
            else // another tank
            begin
                state <= 2;
                global_time_out <= 0;
            end
        end
        else if(recv_bit == 0) begin
            state <= 10;
            hit_int <= 1;
            global_time_out <= 0;
        end
        else begin
            state <= 2;
            global_time_out <= 0;
        end
    end
end
else if(state == 2)
begin
    if(global_time_out == 100000)
    begin
        state <= 10; // reset state
    end
    global_time_out <= global_time_out + 1;

    if(hit_sync[2] == 0 && hit_sync[1] == 1)
    begin
        check_if_self <= 0;
        state <= 1; // test whether it is a hard hit or light hit
        global_time_out <= 0;
    end
end
/*
else if(state == 3) // test if the hit is light or hard
begin
    if(global_time_out == 100000)
    begin
        state <= 10; // reset state
    end
    global_time_out <= global_time_out + 1;

    if(check_if_hard < 30000)
        check_if_hard <= check_if_hard + 1;
    else if(check_if_hard == 30000)
    begin
        check_if_hard <= 0;
        keep_int <= 0;
        if(hit_sync[1] == 0) // hard hit
        begin
            hard_hit_int <= 1;
            state <= 10;
            global_time_out <= 0;
        end
        else // light hit
        begin
            hit_int <= 1;
            state <= 10;
            global_time_out <= 0;
        end
    end
end
else if(state == 4)
begin
    if(global_time_out == 100000)
    begin
        state <= 10; // reset state
    end
    global_time_out <= global_time_out + 1;

    if(keep_int <100)
        keep_int <= keep_int + 1;
    else
    begin
        keep_int <= 0;
        hard_hit_int <= 0;
        hit_int <= 0;
        state <= 10; // finish hit. Go back.
        global_time_out <= 0;
    end
end
*/
else if(state == 10) // reset state
begin
    L <= 1;
    hit_int <= 0;
    hard_hit_int <= 0;
    //hit_sync <= 3'b000;
    count <= 0;
    check_if_hard <= 0;
    check_if_self <= 0;
    state <= 0;
    keep_int <= 0;
    global_time_out <= 0;
    recv_bit <= 2; // need to change if add bits
end
end
endmodule

