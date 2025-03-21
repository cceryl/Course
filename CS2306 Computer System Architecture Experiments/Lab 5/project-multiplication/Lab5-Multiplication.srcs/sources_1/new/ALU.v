`timescale 1ns / 1ps

module ALU (
    input [31:0] input1,
    input [31:0] input2,
    input [4:0] shamt,
    input [3:0] ALUCtr,
    output reg zero,
    output reg [31:0] result
);

  always @(input1, input2, ALUCtr, shamt) begin
    case (ALUCtr)
      4'b0000: result = input1 & input2;  // and
      4'b0001: result = input1 | input2;  // or
      4'b0010: result = input1 ^ input2;  // xor
      4'b0011: result = ~(input1 | input2);  // nor

      4'b0100: result = input1 + input2;  // add
      4'b0101: result = input1 - input2;  // sub

      4'b0110: result = ($signed(input1) < $signed(input2)) ? 32'b1 : 32'b0;  // slt
      4'b0111: result = (input1 < input2) ? 32'b1 : 32'b0;  // sltu

      4'b1000: result = input2 << shamt;  // sll
      4'b1001: result = input2 >> shamt;  // srl
      4'b1010: result = $signed(input2) >>> shamt;  // sra
      4'b1011: result = input2 << input1;  // sllv
      4'b1100: result = input2 >> input1;  // srlv
      4'b1101: result = $signed(input2) >>> input1;  // srav

      4'b1110: result = input2 << 16;  // lui

      default: result = 32'b0; // nop
    endcase
    zero = (result == 32'b0) ? 1'b1 : 1'b0;
  end

endmodule
