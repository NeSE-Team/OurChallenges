`default_nettype none


module uart_down_counter(
        input clk,
        input rst,
        input effective,
        output ready
);

// 12Mhz / 115200 baud == 104 clocks per baud
localparam bauds_per_clock = 104;
reg [$clog2(bauds_per_clock)-1:0] downcounter = 0;

always @(posedge clk) begin
        if (rst) begin
                downcounter <= 0;
        end else if (effective) begin
                downcounter <= bauds_per_clock - 1;
        end else if (downcounter != 0) begin
                downcounter <= downcounter - 1;
        end
end

assign ready = (downcounter == 0);

endmodule

module uart_put(
        input clk,
        input rst,
        input [7:0] x,
        input effective,
        output uart,
        output ready
);

reg [3:0] state = 0;
reg [7:0] chr = 0;

assign ready = (state == 0);

wire [10:0] choice;

assign choice = {1'b1, chr, 2'b01};

wire counter_finish;

reg transit = 0;

uart_down_counter counter(
        .clk(clk),
        .rst(rst),
        .effective(transit),
        .ready(counter_finish)
);

reg r_uart = 1;

always @(posedge clk) begin
        if (rst) begin
                state <= 0;
                chr <= 0;
                transit <= 0;
                r_uart <= 1;
        end else if (effective & (state == 0)) begin
                state <= 1;
                chr <= x;
                transit <= 1;
                r_uart <= 1;
        end else if ((state != 0) & counter_finish & ~transit) begin
                if(state != 4'd11) begin
                        state <= state + 1;
                        transit <= 1;
                        r_uart <= choice[state];
                end else begin
                        state <= 0;
                        r_uart <= 1;
                end
        end else begin
                transit <= 0;
        end
end

assign uart = r_uart;

endmodule

module ffmul(
        input clk,
        input rst,
        input [63:0] x,
        input [63:0] y,
        input [63:0] pol,
        input effective,
        output [63:0] result,
        output ready
);

reg [63:0] rx = 0;
reg [63:0] ry = 0;
reg [63:0] rpol = 0;

assign ready = (rx == 0);
reg [63:0] rresult = 0;
assign result = rresult;

always @(posedge clk) begin
        if (rst) begin
                rx <= 0;
                ry <= 0;
                rpol <= 0;
                rresult <= 0;
        end else if (effective) begin
                rx <= x;
                ry <= y;
                rpol <= pol;
                rresult <= 0;
        end else if (rx != 0) begin
                if (rx[0]) begin
                        rresult <= rresult ^ ry;
                end
                rx <= rx[63:1];
                if (ry[63]) begin
                        ry <= {ry[62:0],1'b0} ^ rpol;
                end else begin
                        ry <= {ry[62:0],1'b0};
                end
        end
end

endmodule

module top(
        input uart,
        input clk_12,
        output uart2
);

reg [7:0] my_memory [0:95];

initial begin
        $readmemb("memory.bin", my_memory);
end

wire [63:0] target1;
wire [63:0] target2;

assign target1 = {my_memory[87],my_memory[86],my_memory[85],my_memory[84],my_memory[83],my_memory[82],my_memory[81],my_memory[80]};
assign target2 = {my_memory[95],my_memory[94],my_memory[93],my_memory[92],my_memory[91],my_memory[90],my_memory[89],my_memory[88]};

reg [3:0] rst_counter = 4'b1111;
always @(posedge clk_12) begin
        if (rst_counter != 3'b0) begin
                rst_counter <= rst_counter - 1;
        end
end
wire rst = (rst_counter != 3'b0);

// 12Mhz / 115200 baud == 104 clocks per baud
localparam bauds_per_clock = 104;


`define STATE_IDLE 32'd0
`define STATE_START 32'd1
`define STATE_DATA 32'd2
`define STATE_STOP 32'd3

reg [31:0] state = `STATE_IDLE;
wire downcounter_start;
reg [$clog2(bauds_per_clock)-1:0] downcounter = 0;

reg [3:0] bitn = 0;

always @(posedge clk_12) begin
        if (rst) begin
                bitn <= 0;
                state <= `STATE_IDLE;
        end else begin
                case (state)
                        `STATE_IDLE: begin
                                if (uart == 0) begin
                                        state <= `STATE_START;
                                end
                        end
                        `STATE_START: begin
                                if (downcounter == 0) begin
                                        state <= `STATE_DATA;
                                        bitn <= 7;
                                end
                        end
                        `STATE_DATA: begin
                                if (downcounter == 0) begin
                                        if (bitn == 0) begin
                                                state <= `STATE_STOP;
                                        end else begin
                                                bitn <= bitn - 1;
                                        end
                                end
                        end
                        `STATE_STOP: begin
                                if (downcounter == 0) begin
                                        state <= `STATE_IDLE;
                                end
                        end
                endcase
        end
end

assign downcounter_start =      (state == `STATE_IDLE && uart == 0) ? 1 : 
                                (state == `STATE_START && downcounter == 0) ? 1 :
                                (state == `STATE_DATA && downcounter == 0) ? 1 : 0;


always @(posedge clk_12) begin
        if (rst) begin
                downcounter <= 0;
        end else begin
                if (downcounter_start) begin
                        downcounter <= bauds_per_clock - 1;
                end else begin
                        if (downcounter != 0) begin
                                downcounter <= downcounter - 1;
                        end
                end
        end
end

wire sample_bit = (state == `STATE_DATA && downcounter == bauds_per_clock / 2);

reg [7:0] cur_byte = 0;

always @(posedge clk_12) begin
        if (sample_bit) begin
                cur_byte[7-bitn] <= uart;
        end
end

wire byte_strobe = (state == `STATE_STOP && downcounter == bauds_per_clock / 2);

reg unlock_flag = 0;

reg locked = 0;

reg [3:0] char_counter = 0;

reg [127:0] char_buf = 0;
integer i;

always @(posedge clk_12) begin
        if (rst) begin
                char_counter <= 0;
                locked <= 0;
                char_buf <= 0;
        end else begin
                if (byte_strobe & ~locked) begin
                        if(char_counter == 4'd15) begin
                                locked <= 1;
                        end
                        char_counter <= char_counter + 1;
                        char_buf[char_counter*8 +: 8] <= cur_byte ^ 8'd42;
                end else if (locked & unlock_flag) begin
                        locked <= 0;
                end
        end
end

reg [3:0] stage = 0;

always @(posedge clk_12) begin
        if (rst) begin
                stage <= 0;
        end else if (stage == 0) begin
                if (locked) begin
                        stage <= 1;
                end
        end else if (stage == 1) begin
                // only one frame
                stage <= 2;
        end else if (stage == 2) begin
                if(ready1 & ready2)
                        stage <= 3;
        end else if (stage == 3) begin
                // only one frame
                stage <= 4;
        end else if (stage == 4) begin
                if(ready1 & ready2 & ready3) begin
                        stage <= 5;
                end
        end else if (stage == 5) begin
                if((result1 == target1)
                &  (result2 == target2)) begin
                        // correct
                        stage <= 6;
                end else begin
                        // wrong
                        stage <= 7;
                end
        end else if ((stage == 6)|(stage == 7)) begin
                stage <= 8;
        end else if (stage == 8) begin
                if(unlock_flag)
                        stage <= 0;
        end
end



wire ready1;
wire [63:0] result1;

wire ready2;
wire [63:0] result2;

wire ready3;
wire [63:0] result3;

// output buffer range

reg [7:0] start_off = 0;
reg [7:0] end_off = 0;
reg sending = 0;
reg [7:0] chr = 0;
reg send_signal = 0;
wire send_finish;

reg [63:0] enc_key = 0;

reg enc_key_gen = 0;

reg align_bound = 0;


always @(posedge clk_12) begin
        if (rst) begin
                start_off <= 0;
                end_off <= 0;
                sending <= 0;
                chr <= 0;
                send_signal <= 0;
                unlock_flag <= 0;
                align_bound <= 0;
        end else if (~sending&((stage == 6)|(stage == 7))) begin
                sending <= 1;
                send_signal <= 0;
                if (stage == 6) begin
                        start_off <= 0;
                        end_off <= 8'h40;
                end else if (stage == 7) begin
                        start_off <= 8'h40;
                        end_off <= 8'h50;
                end
                align_bound <= 0;
        end else if (sending & send_finish) begin
                if (start_off == end_off) begin
                        send_signal <= 0;
                        sending <= 0;
                        unlock_flag <= 1;
                        start_off <= 0;
                        end_off <= 0;
                        chr <= 0;
                        align_bound <= 0;
                end else if (send_signal == 0) begin
                        send_signal <= 1;
                        chr <= enc_key[(start_off&7)*8 +: 8]^my_memory[start_off];
                        start_off <= start_off + 1;
                        if ((start_off&7) == 7) begin
                                align_bound <= 1;
                        end else begin
                                align_bound <= 0;
                        end
                end else begin
                        align_bound <= 0;
                end
        end else if (unlock_flag) begin
                unlock_flag <= 0;
        end else begin
                send_signal <= 0;
        end
end

reg last_ready3 = 0;

always @(posedge clk_12) begin
        if (rst) begin
                enc_key <= 0;
                enc_key_gen <= 0;
        end else if(stage == 1) begin
                enc_key_gen <= 1;
                enc_key <= char_buf[127:64]+char_buf[63:0];
                // TODO: more keys
        end else if(stage == 7) begin
                enc_key <= 64'hc456a1adda0b49cf;
        end else if(align_bound) begin
                enc_key_gen <= 1;
        end else begin
                enc_key_gen <= 0;
                if (ready3 & ~last_ready3) begin
                        enc_key <= result3;
                end
        end
        last_ready3 <= ready3;
end
ffmul ff1(
        .clk(clk_12),
        .rst(rst),
        .x((char_buf[63:0]&{64{stage==1}})|
                (result2&{64{stage==3}})),
        .y(64'h12345678deadc32f),
        .pol(64'h30410c084000aa3),
        .effective((stage==1)|(stage==3)),
        .result(result1),
        .ready(ready1)
);

ffmul ff2(
        .clk(clk_12),
        .rst(rst),
        .x((char_buf[127:64]&{64{stage==1}})|
                (result1&{64{stage==3}})),
        .y(64'h113355770044baef),
        .pol(64'h283),
        .effective((stage==1)|(stage==3)),
        .result(result2),
        .ready(ready2)
);

ffmul ff3(
        .clk(clk_12),
        .rst(rst),
        .x(enc_key),
        .y(64'hceba1234deaf2019),
        .pol(64'h80000000001005),
        .effective(enc_key_gen),
        .result(result3),
        .ready(ready3)
);

uart_put putter(
        .clk(clk_12),
        .rst(rst),
        .x(chr),
        .effective(send_signal),
        .uart(uart2),
        .ready(send_finish)
);

/*
localparam key_length = 8;
// Hack hack, lovely hack.
reg [key_length-1:0] given_0;
reg [key_length-1:0] given_1;
reg [key_length-1:0] given_2;
reg [key_length-1:0] given_3;
reg [key_length-1:0] given_4;
reg [key_length-1:0] given_5;
reg [key_length-1:0] given_6;
reg [key_length-1:0] given_7;

always @(posedge clk_12) begin
        if (rst) begin
                given_0 <= 0;
                given_1 <= 0;
                given_2 <= 0;
                given_3 <= 0;
                given_4 <= 0;
                given_5 <= 0;
                given_6 <= 0;
                given_7 <= 0;
        end else begin
                if (byte_strobe) begin
                        given_0 <= {given_0[6:0], cur_byte[0]};
                        given_1 <= {given_1[6:0], cur_byte[1]};
                        given_2 <= {given_2[6:0], cur_byte[2]};
                        given_3 <= {given_3[6:0], cur_byte[3]};
                        given_4 <= {given_4[6:0], cur_byte[4]};
                        given_5 <= {given_5[6:0], cur_byte[5]};
                        given_6 <= {given_6[6:0], cur_byte[6]};
                        given_7 <= {given_7[6:0], cur_byte[7]};
                end
        end
end

`include "want.v"

wire open =   (want_0 == given_0) &&
                (want_1 == given_1) &&
                (want_2 == given_2) &&
                (want_3 == given_3) &&
                (want_4 == given_4) &&
                (want_5 == given_5) &&
                (want_6 == given_6) &&
                (want_7 == given_7);

assign led_green = !open;
assign led_red = open;

*/

endmodule
