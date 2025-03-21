`timescale 1ns / 1ps

module dataMemory (
    input Clk,
    input [31:0] address,
    input [31:0] writeData,
    input memWrite,
    input memRead,
    output reg [31:0] readData
);

  reg [31:0] memory[0:63];

  always @(*) begin
    if (memRead) begin
      readData = memory[address];
    end else begin
      readData = 0;
    end
  end

  always @(negedge Clk) begin
    if (memWrite & ~memRead) begin
      memory[address] = writeData;
    end
  end

endmodule
