`timescale 1ns / 1ps

module Registers_tb ();

  reg [25:21] readReg1;
  reg [20:16] readReg2;
  reg [4:0] writeReg;
  reg [31:0] writeData;
  reg regWrite;
  reg Clk;
  wire [31:0] readData1;
  wire [31:0] readData2;

  Registers u0 (
      .readReg1(readReg1),
      .readReg2(readReg2),
      .writeReg(writeReg),
      .writeData(writeData),
      .regWrite(regWrite),
      .Clk(Clk),
      .readData1(readData1),
      .readData2(readData2)
  );

  parameter PERIOD = 200;
  always #(PERIOD / 2) Clk = !Clk;

  initial begin
    Clk = 1'b0;
    readReg1 = 5'd0;
    readReg2 = 5'd0;
    writeReg = 5'd0;
    writeData = 32'd0;
    regWrite = 1'b0;
    #285;  // current time: 285
    writeReg  = 5'd21;
    writeData = 32'd4294901760;
    regWrite  = 1'b1;
    #200;  // current time: 485
    writeReg  = 5'd10;
    writeData = 32'd65535;
    #200;  // current time: 685
    writeReg  = 5'd0;
    writeData = 32'd0;
    regWrite  = 1'b0;
    #50;  // current time: 735
    readReg1 = 5'd21;
    readReg2 = 5'd10;
  end

endmodule
