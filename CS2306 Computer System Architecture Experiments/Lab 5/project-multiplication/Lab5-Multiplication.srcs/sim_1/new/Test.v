`timescale 1ns / 1ps

module Test ();

  reg clk;
  reg reset;

  reg [3:0] a;
  reg [3:0] b;

  wire led_clk;
  wire led_en;
  wire led_do;

  wire seg_clk;
  wire seg_en;
  wire seg_do;

  Top top (
      .clk_p  (clk),
      .clk_n  (!clk),
      .reset_n(!reset),
      .a      (a),
      .b      (b),
      .led_clk(led_clk),
      .led_en (led_en),
      .led_do (led_do),
      .seg_clk(seg_clk),
      .seg_en (seg_en),
      .seg_do (seg_do)
  );

  initial begin
    clk = 0;
    reset = 0;
    a = 8'd1;
    b = 8'd2;
    #20 reset = 1;
    #20 reset = 0;

    // a = 1, b = 2, expected product = 2
    a = 8'd1;
    b = 8'd2;
    #1500;

    // a = 13, b = 9, expected product = 117
    a = 8'd13;
    b = 8'd9;
    #1500;

    // a = 15, b = 15, expected product = 225
    a = 8'd15;
    b = 8'd15;
    #1500;

    // a = 4, b = 0, expected product = 0
    a = 8'd4;
    b = 8'd0;
    #1500;

    // a = 11, b = 3, expected product = 33
    a = 8'd11;
    b = 8'd3;
  end

  always begin
    #10 clk = ~clk;
  end




endmodule
