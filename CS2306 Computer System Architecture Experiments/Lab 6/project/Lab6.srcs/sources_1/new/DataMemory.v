`timescale 1ns / 1ps
`include "ISA.v"

module DataMemory (
    input wire clk,

    input wire [31:0] address,
    input wire [31:0] writeData,
    input wire memRead,
    input wire memWrite,

    output reg [31:0] readData
);

  reg [31:0] memory[0:63];

  always @(*) begin
    readData = memRead ? memory[address>>2] : 32'h00000000;
  end

  always @(posedge clk) begin
    if (memWrite) begin
      memory[address>>2] = writeData;
    end
  end

  initial begin
    $readmemh("Data.txt", memory);
  end

endmodule
