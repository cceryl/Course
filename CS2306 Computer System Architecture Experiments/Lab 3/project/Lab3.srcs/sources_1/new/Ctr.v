`timescale 1ns / 1ps

module Ctr (
    input [5:0] opCode,
    output reg regDst,
    output reg aluSrc,
    output reg memToReg,
    output reg regWrite,
    output reg memRead,
    output reg memWrite,
    output reg branch,
    output reg [1:0] aluOp,
    output reg jump
);

  always @(opCode) begin
    case (opCode)
      6'b000000: begin  // R-type: add, sub, and, or, slt
        regDst = 1'b1;
        aluSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        branch = 1'b0;
        aluOp = 2'b10;
        jump = 1'b0;
      end
      6'b100011: begin  // lw
        regDst = 1'b0;
        aluSrc = 1'b1;
        memToReg = 1'b1;
        regWrite = 1'b1;
        memRead = 1'b1;
        memWrite = 1'b0;
        branch = 1'b0;
        aluOp = 2'b00;
        jump = 1'b0;
      end
      6'b101011: begin  // sw
        regDst = 1'b0;  // don't care
        aluSrc = 1'b1;
        memToReg = 1'b0;  // don't care
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b1;
        branch = 1'b0;
        aluOp = 2'b00;
        jump = 1'b0;
      end
      6'b000100: begin  // beq
        regDst = 1'b0;  // don't care
        aluSrc = 1'b0;
        memToReg = 1'b0;  // don't care
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        branch = 1'b1;
        aluOp = 2'b01;
        jump = 1'b0;
      end
      6'b000010: begin  // jump
        regDst = 1'b0;
        aluSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        branch = 1'b0;
        aluOp = 2'b00;
        jump = 1'b1;
      end
      default: begin  // nop
        regDst = 1'b0;
        aluSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        branch = 1'b0;
        aluOp = 2'b00;
        jump = 1'b0;
      end
    endcase
  end

endmodule
