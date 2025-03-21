`timescale 1ns / 1ps
`include "ISA.v"

module Registers (
    input wire clk,

    input wire [25:21] readReg1,
    input wire [20:16] readReg2,
    
    input wire [ 4: 0] writeReg,
    input wire [31: 0] writeData,
    input wire regWrite,

    output reg [31:0] readData1,
    output reg [31:0] readData2
);

  reg [31:0] regFile[31:0];

  // Forwarding
  always @(*) begin
    readData1 = regWrite && writeReg == readReg1 ? writeData : regFile[readReg1];
    readData2 = regWrite && writeReg == readReg2 ? writeData : regFile[readReg2];
  end

  always @(posedge clk) begin
    if (regWrite && writeReg != 0) begin
      regFile[writeReg] = writeData;
    end
  end

  initial begin
    $readmemh("Register.txt", regFile);
  end

endmodule
