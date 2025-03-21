`timescale 1ns / 1ps

module InstructionMemory (
    input clk,
    input reset,
    input [31:0] address,

    output reg [31:0] readData
);

  reg [31:0] memory[0:63];

  always @(*) begin
    if (reset) begin
      readData = memory[0];
    end else begin
      readData = memory[address>>2];
    end
  end

  initial begin
    $readmemb("Instruction.txt", memory);
  end

endmodule
