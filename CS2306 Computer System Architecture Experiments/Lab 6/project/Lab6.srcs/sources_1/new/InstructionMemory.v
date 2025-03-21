`timescale 1ns / 1ps
`include "ISA.v"

module InstructionMemory (
    input wire clk,

    input wire [31:0] address,

    output reg [31:0] instruction
);

  reg [31:0] memory[0:63];

  always @(*) begin
    instruction = memory[address>>2];
  end

  initial begin
    $readmemb("Instruction.txt", memory);
  end

endmodule
