`timescale 1ns / 1ps
`include "ISA.v"

module Control (
    input wire [5:0] opCode,

    output reg regDst,
    output reg ALUSrc,
    output reg memToReg,
    output reg regWrite,
    output reg memRead,
    output reg memWrite,
    output reg [3:0] ALUOp
);

  always @(*) begin
    case (opCode)
      `OP_RTYPE: begin
        regDst   = 1'b1;
        ALUSrc   = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_RTYPE;
      end
      `OP_ADDI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_ADD;
      end
      `OP_ADDIU: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_ADD;
      end
      `OP_ANDI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_AND;
      end
      `OP_ORI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_OR;
      end
      `OP_XORI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_XOR;
      end
      `OP_LUI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_LUI;
      end
      `OP_LW: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b1;
        regWrite = 1'b1;
        memRead  = 1'b1;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_ADD;
      end
      `OP_SW: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead  = 1'b0;
        memWrite = 1'b1;
        ALUOp    = `ALU_OP_ADD;
      end
      `OP_BEQ: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_BRANCH;
      end
      `OP_BNE: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_BRANCH;
      end
      `OP_SLTI: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_SLT;
      end
      `OP_SLTIU: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_SLTU;
      end
      `OP_J: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_NOP;
      end
      `OP_JAL: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_NOP;
      end
      default: begin
        regDst   = 1'b0;
        ALUSrc   = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead  = 1'b0;
        memWrite = 1'b0;
        ALUOp    = `ALU_OP_NOP;
      end
    endcase
  end

endmodule
