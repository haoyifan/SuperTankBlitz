module shoot( PCLK,
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
              tr //triger identifier
);
//APB interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output reg [31:0] PRDATA;
output PREADY, PSLVERR;

assign PSLVERR = 0;
assign PREADY = 1;

output shooting;
output tr;

reg shooting; // represents the shooting signal
reg [2:0] sw_sync; // synchronize SW0
reg [2:0] sw_sync1; // synchronize SW1
reg [31:0] count; // divide the 100 MHz into 38 kHz
reg [7:0] out_bit; // count which pulse it is sending
reg [7:0] cmd_bit; // count which command bit is sending
reg [31:0] ending; // represents the length of the LOW or HIGH
reg [2:0] state; // represents the state in the state diagram
reg hit_strength; // reprensents whether it is a hard hit(1) or light hit(0)

reg [31:0] cmd;
reg trig;

`define FINISH_OUT 6
`define DIVISION 2632
`define QUARTER 658
`define THIRD_QUARTER 1974


assign tr = trig;
// shooting

wire BUS_WRITE_EN;
assign BUS_WRITE_EN = (PWRITE && PSEL && PENABLE);

always@(posedge PCLK)
begin
	if(PRESETN && BUS_WRITE_EN) begin
		trig <= 1;
		cmd <= PWDATA;
	end
    if(~PRESETN) // reset
    begin
        shooting <= 0;
        sw_sync <= 0;
        sw_sync1 <= 0;
        count <= 0;
        out_bit <= 0;
        ending <= 0;
        state <= 0;
        hit_strength <= 0;
    end
    else if(trig == 1) begin
        trig <= 0;
        cmd_bit <= 2;
        if(cmd[2] == 0)
            ending <= 6;
        else
            ending <= 18;
        shooting <= 1;
        count <= count + 1;
        state <= 1; // entering sending ID state
    end
/*
    else if(trig == 1 && cmd[0] == 0) // transition from state 0 to state 1
    begin
		trig <= 0;
        ending <= 6;
        hit_strength <= cmd[0];
        shooting <= 1;
        count <= count + 1;
        state <= 1; // entering sending ID state
    end
    else if(trig == 1 && cmd[0] == 1) // transition from state 0 to state 1
    begin
		trig <= 0;
        ending <= 12;
        hit_strength <= cmd[0];
        shooting <= 1;
        count <= count + 1;
        state <= 1; // entering sending ID state
    end
*/
    else if(state == 1)
    begin
        if(count < 2632)
        begin
            count <= count + 1;
            if(count < 1974) // clock division
                shooting <= 1;
            else
                shooting <= 0;
        end
        else
        begin
            count <= 0;
            out_bit <= out_bit + 1;
            shooting <= 0;
            if(out_bit == ending)
            begin
                state <= 2;
                out_bit <= 0;
            end
        end
    end
    else if(state == 2)
    begin
    if(count < 40000)
        count <= count + 1;
    else
    begin
        count <= 0;
        cmd_bit <= cmd_bit - 1;
        if(cmd[cmd_bit - 1] == 0)
            ending <= 6;
        else
            ending <= 18;
        if(cmd_bit == 0)
            state <= 0;
        else
            state <= 1;
    end
    end
end

endmodule

