`timescale 1ns / 1ps

module Test ();

  reg clk;
  reg reset;

  Top top (
      .clk  (clk),
      .reset(reset)
  );

  initial begin
    clk   = 0;
    reset = 0;
    #20 reset = 1;
    #20 reset = 0;
  end

  always begin
    #10 clk = ~clk;
  end

endmodule
