`default_nettype none

module bench;

reg clk_12 = 0;
always #5 clk_12 = !clk_12;

reg uart;
wire uart2;

top top(
        .uart(uart),
        .clk_12(clk_12),
        .uart2(uart2)
);

initial begin
        $dumpfile("bench.vcd");
        $dumpvars(0);
end


initial begin
        uart = 1;
        #1040

`include "inputbench.v"

        #1040;
        #1040;

        $finish;
end

endmodule
