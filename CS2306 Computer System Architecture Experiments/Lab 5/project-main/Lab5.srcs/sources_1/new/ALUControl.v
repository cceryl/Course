`timescale 1ns / 1ps

module ALUControl (
    input [3:0] ALUOp,
    input [5:0] funct,

    output reg [3:0] out
);

  always @(ALUOp, funct) begin
    case (ALUOp)
      4'b0000: begin  // R-type
        casex (funct)
          6'b10000x: out = 4'b0100;  // add addu
          6'b10001x: out = 4'b0101;  // sub subu
          6'b100100: out = 4'b0000;  // and
          6'b100101: out = 4'b0001;  // or
          6'b100110: out = 4'b0010;  // xor
          6'b100111: out = 4'b0011;  // nor
          6'b101010: out = 4'b0110;  // slt
          6'b101011: out = 4'b0111;  // sltu
          6'b000000: out = 4'b1000;  // sll
          6'b000010: out = 4'b1001;  // srl
          6'b000011: out = 4'b1010;  // sra
          6'b000100: out = 4'b1011;  // sllv
          6'b000110: out = 4'b1100;  // srlv
          6'b000111: out = 4'b1101;  // srav
          default:   out = 4'b1111;
        endcase
      end

      4'b0001: out = 4'b0100;  // addi addiu lw sw
      4'b0010: out = 4'b0000;  // andi
      4'b0011: out = 4'b0001;  // ori
      4'b0100: out = 4'b0010;  // xori
      4'b0101: out = 4'b1110;  // lui
      4'b0110: out = 4'b0101;  // beq bne
      4'b0111: out = 4'b0110;  // slti
      4'b1000: out = 4'b0111;  // sltiu
      default: out = 4'b1111;
    endcase
  end

endmodule
