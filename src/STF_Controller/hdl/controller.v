module n64_ctrl(PCLK,
				PENABLE,
				PSEL,
				PRESETN,
				PWRITE,
				PREADY,
				PSLVERR,
				PADDR,
				PWDATA,
				PRDATA,
                //led,
                inoutData,
                //fabint_out,
                fabint);

// APB Bus Interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output reg [31:0] PRDATA;
output PREADY, PSLVERR;

//output [7:0] led;
output fabint;

//output fabint_out;
assign fabint_out = fabint;

reg fabint;

// customized data
inout inoutData;
reg OutData;

assign PREADY = 1'b1;
assign PSLVERR = 1'b0;

reg [8:0] poll_sig;
reg [5:0] read_bit; //count the bit of received signal
reg polling;
reg reading;
reg [3:0] poll_bit; //count the bit of polling signal sent
reg [1:0] four_count; //always counting from 0 to 3
reg [31:0] ctrl_data; //the 32 bit controller data array
reg [32:0] temp_data; //temporary controller data array, pass to ctrl_data when signal receive completed
reg [31:0] idle;
reg idle_flag;

reg [32:0] counter;

reg curr;
reg buff;


wire BUS_READ_EN;
assign BUS_READ_EN = (!PWRITE && PSEL && PENABLE);

assign inoutData = (polling)? OutData : 1'bz;

assign led = ctrl_data[15:8]; // the debug port


always@(posedge PCLK)
begin
   if (BUS_READ_EN) begin // READ
        // return 32 bit array
        PRDATA <= ctrl_data;
   end
end

//using divided clock signal (1MHz)
always@(posedge PCLK) begin
counter <= counter + 1;
if(~PRESETN)
begin
    poll_sig <= 9'b110000000; //polling signal: 000000011
	OutData <= 1;
	four_count <= 0;
    counter <= 0;

    polling <= 1;
	poll_bit <= 0;

	reading <= 0;
	read_bit <= 0;

    idle_flag <= 0;
    idle <= 0;

    fabint <= 0;
end
else if (polling) //sending polling signal (9 bit * 4 us)
begin
        if(counter == 0)
            OutData <= 0;
        else if(counter == 99)
            OutData <= poll_sig[poll_bit];
        else if(counter == 299)
            OutData <= 1;
        else if(counter == 399)
        begin
            poll_bit <= poll_bit + 1;
            counter <= 0;
			if (poll_bit == 8) begin
			    poll_bit <= 0;
				polling <= 0;
				reading <= 1;
				read_bit <= 0;
                idle_flag <= 0;
                idle <= 0;
			end
        end
        else
            OutData <= OutData;
end

else if(reading)
    begin
        curr <= inoutData;
        buff <= curr;
        if(curr == 0 && buff == 1)
            counter <= 0;
        if(counter == 205) begin
            temp_data[31-read_bit] <= inoutData;
            read_bit <= read_bit + 1;
			if (read_bit == 31) begin
				ctrl_data <= temp_data[31:0];
				read_bit <= 0;
				reading <= 0;
				poll_bit <= 0;
				polling <= 0;
                idle_flag <= 1;
                idle <= 0;
                fabint <= 1;
			end
        end
        if(counter == 399)
        begin
            counter <= 0;
            fabint <= 0;
        end
	 end

else if(idle_flag)
begin
    fabint <= 0;
    if(idle < 3957000)
        idle <= idle + 1;
    else
    begin
        idle_flag <= 0;
        idle <= 0;
        polling <= 1;
        poll_bit <= 0;
        four_count <= 0;
        counter <= 0;
    end
end
else begin
    OutData <= 0;
    fabint <= 0;
end
end
endmodule