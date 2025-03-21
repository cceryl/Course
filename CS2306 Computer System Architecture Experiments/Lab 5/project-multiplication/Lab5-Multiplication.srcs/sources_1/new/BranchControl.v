`timescale 1ns / 1ps

module BranchControl (
    input wire [5:0] opCode,
    input wire [5:0] funct,

    output reg beq,
    output reg bne,
    output reg jr,
    output reg j,
    output reg jal
);

  always @(opCode, funct) begin
    beq = (opCode == 6'b000100) ? 1'b1 : 1'b0;
    bne = (opCode == 6'b000101) ? 1'b1 : 1'b0;
    jr = (opCode == 6'b000000 && funct == 6'b001000) ? 1'b1 : 1'b0;
    j = (opCode == 6'b000010) ? 1'b1 : 1'b0;
    jal = (opCode == 6'b000011) ? 1'b1 : 1'b0;
  end

endmodule
