`timescale 1ns / 1ps
`include "ISA.v"

module ALUControl (
    input wire [3:0] ALUOp,
    input wire [5:0] funct,

    output reg [3:0] ALUCtr
);

  always @(*) begin
    case (ALUOp)
      `ALU_OP_RTYPE: begin
        casex (funct)
          `FUNCT_ADD:  ALUCtr = `ALU_ADD;
          `FUNCT_SUB:  ALUCtr = `ALU_SUB;
          `FUNCT_AND:  ALUCtr = `ALU_AND;
          `FUNCT_OR:   ALUCtr = `ALU_OR;
          `FUNCT_XOR:  ALUCtr = `ALU_XOR;
          `FUNCT_NOR:  ALUCtr = `ALU_NOR;
          `FUNCT_SLT:  ALUCtr = `ALU_SLT;
          `FUNCT_SLTU: ALUCtr = `ALU_SLTU;
          `FUNCT_SLL:  ALUCtr = `ALU_SLL;
          `FUNCT_SRL:  ALUCtr = `ALU_SRL;
          `FUNCT_SRA:  ALUCtr = `ALU_SRA;
          `FUNCT_SLLV: ALUCtr = `ALU_SLLV;
          `FUNCT_SRLV: ALUCtr = `ALU_SRLV;
          `FUNCT_SRAV: ALUCtr = `ALU_SRAV;
          default:     ALUCtr = `ALU_NOP;
        endcase
      end
      `ALU_OP_ADD:    ALUCtr = `ALU_ADD;
      `ALU_OP_AND:    ALUCtr = `ALU_AND;
      `ALU_OP_OR:     ALUCtr = `ALU_OR;
      `ALU_OP_XOR:    ALUCtr = `ALU_XOR;
      `ALU_OP_LUI:    ALUCtr = `ALU_LUI;
      `ALU_OP_BRANCH: ALUCtr = `ALU_SUB;
      `ALU_OP_SLT:    ALUCtr = `ALU_SLT;
      `ALU_OP_SLTU:   ALUCtr = `ALU_SLTU;
      default:        ALUCtr = `ALU_NOP;
    endcase
  end

endmodule
