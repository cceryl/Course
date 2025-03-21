`timescale 1ns / 1ps
`include "ISA.v"

module ALU (
    input wire [31:0] input1,
    input wire [31:0] input2,
    input wire [ 4:0] shamt,
    input wire [ 3:0] ALUCtr,

    output reg zero,
    output reg [31:0] result
);

  always @(*) begin
    case (ALUCtr)
      `ALU_AND: result = input1 & input2;
      `ALU_OR:  result = input1 | input2;
      `ALU_XOR: result = input1 ^ input2;
      `ALU_NOR: result = ~(input1 | input2);

      `ALU_ADD: result = input1 + input2;
      `ALU_SUB: result = input1 - input2;

      `ALU_SLT:  result = ($signed(input1) < $signed(input2)) ? 32'b1 : 32'b0;
      `ALU_SLTU: result = (input1 < input2) ? 32'b1 : 32'b0;

      `ALU_SLL:  result = input2 << shamt;
      `ALU_SRL:  result = input2 >> shamt;
      `ALU_SRA:  result = $signed(input2) >>> shamt;
      `ALU_SLLV: result = input2 << input1;
      `ALU_SRLV: result = input2 >> input1;
      `ALU_SRAV: result = $signed(input2) >>> input1;

      `ALU_LUI: result = input2 << 16;

      default: result = 32'b0;
    endcase

    zero = result == 32'b0;
  end

endmodule
