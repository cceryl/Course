`timescale 1ns / 1ps

module ProgramCounter (
    input wire clk,
    input wire reset,

    input wire [25:0] jumpImm,
    input wire [31:0] branchImm,
    input wire [31:0] ra,

    input wire beq,
    input wire bne,
    input wire jr,
    input wire j,
    input wire jal,
    input wire ALUZero,

    output reg [31:0] programCounter
);

  wire [31:0] jumpAddr = (jumpImm << 2) + ((programCounter + 4) & 32'hf0000000);
  wire [31:0] branchAddr = (branchImm << 2) + programCounter + 4;

  always @(negedge clk or posedge reset) begin
    if (reset) begin
      programCounter = 32'h00000000;
    end else if (beq && ALUZero) begin
      programCounter = branchAddr;
    end else if (bne && !ALUZero) begin
      programCounter = branchAddr;
    end else if (jr) begin
      programCounter = ra;
    end else if (j) begin
      programCounter = jumpAddr;
    end else if (jal) begin
      programCounter = jumpAddr;
    end else begin
      programCounter = programCounter + 4;
    end
  end

  initial begin
    programCounter = 32'h00000000;
  end

endmodule
