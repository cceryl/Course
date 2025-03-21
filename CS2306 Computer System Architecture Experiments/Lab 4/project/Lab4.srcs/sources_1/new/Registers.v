`timescale 1ns / 1ps

module Registers (
    input [25:21] readReg1,
    input [20:16] readReg2,
    input [4:0] writeReg,
    input [31:0] writeData,
    input regWrite,
    input Clk,
    output reg [31:0] readData1,
    output reg [31:0] readData2
);

  reg [31:0] regFile[31:0];

  always @(readReg1, readReg2, writeReg) begin
    readData1 = regFile[readReg1];
    readData2 = regFile[readReg2];
  end

  always @(negedge Clk) begin
    if (regWrite) begin
      regFile[writeReg] = writeData;
    end
  end

endmodule
