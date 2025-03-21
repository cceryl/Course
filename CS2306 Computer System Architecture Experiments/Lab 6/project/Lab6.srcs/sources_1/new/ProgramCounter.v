`timescale 1ns / 1ps
`include "ISA.v"

module ProgramCounter (
    input wire clk,
    input wire reset,
    input wire stall,

    input wire        jump,
    input wire [31:0] jumpAddr,

    input wire        branch,
    input wire [31:0] branchAddr,

    input wire        predict,
    input wire [31:0] instruction,

    output reg [31:0] programCounter
);

  // Decode current instruction for branch prediction
  wire isBranch = instruction[31:26] == `OP_BEQ || instruction[31:26] == `OP_BNE;
  wire [16:0] branchImm = programCounter + 4 + (instruction[15:0] << 2);
  wire [31:0] branchTarget = {{16{branchImm[15]}}, branchImm};

  always @(negedge clk, posedge reset, posedge jump, posedge branch) begin
    if (reset) begin
      programCounter = 32'b0;
    end else if (branch) begin
      programCounter = branchAddr;
    end else if (jump) begin
      programCounter = jumpAddr;
    end else if (!stall) begin
      programCounter = isBranch && predict ? branchTarget : programCounter + 4;
    end
  end

endmodule
