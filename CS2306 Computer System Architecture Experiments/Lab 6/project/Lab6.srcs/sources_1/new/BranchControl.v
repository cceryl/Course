`timescale 1ns / 1ps
`include "ISA.v"

module BranchControl (
    input wire [5:0] opCode,
    input wire [5:0] funct,

    output reg beq,
    output reg bne,
    output reg jr,
    output reg j,
    output reg jal
);

  always @(*) begin
    beq = opCode == `OP_BEQ;
    bne = opCode == `OP_BNE;
    jr  = opCode == `OP_RTYPE && funct == `FUNCT_JR;
    j   = opCode == `OP_J;
    jal = opCode == `OP_JAL;
  end

endmodule
