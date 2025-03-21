`timescale 1ns / 1ps

module Control (
    input [5:0] opCode,

    output reg regDst,
    output reg ALUSrc,
    output reg memToReg,
    output reg regWrite,
    output reg memRead,
    output reg memWrite,
    output reg [3:0] ALUOp
);

  always @(opCode) begin
    case (opCode)
      6'b000000: begin  // R-type
        regDst = 1'b1;
        ALUSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0000;
      end
      6'b001000: begin  // addi
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0001;
      end
      6'b001001: begin  // addiu
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0001;
      end
      6'b001100: begin  // andi
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0010;
      end
      6'b001101: begin  // ori
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0011;
      end
      6'b001110: begin  // xori
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0100;
      end
      6'b001111: begin  // lui
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0101;
      end
      6'b100011: begin  // lw
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b1;
        regWrite = 1'b1;
        memRead = 1'b1;
        memWrite = 1'b0;
        ALUOp = 4'b0001;
      end
      6'b101011: begin  // sw
        regDst = 1'bx;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b1;
        ALUOp = 4'b0001;
      end
      6'b000100: begin  // beq
        regDst = 1'bx;
        ALUSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0110;
      end
      6'b000101: begin  // bne
        regDst = 1'bx;
        ALUSrc = 1'b0;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0110;
      end
      6'b001010: begin  // slti
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b0111;
      end
      6'b001011: begin  // sltiu
        regDst = 1'b0;
        ALUSrc = 1'b1;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b1000;
      end
      6'b000010: begin  // j
        regDst = 1'bx;
        ALUSrc = 1'bx;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b1111;
      end
      6'b000011: begin  // jal
        regDst = 1'bx;
        ALUSrc = 1'bx;
        memToReg = 1'b0;
        regWrite = 1'b1;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b1111;
      end
      default: begin  // nop
        regDst = 1'bx;
        ALUSrc = 1'bx;
        memToReg = 1'b0;
        regWrite = 1'b0;
        memRead = 1'b0;
        memWrite = 1'b0;
        ALUOp = 4'b1111;
      end
    endcase
  end

endmodule
