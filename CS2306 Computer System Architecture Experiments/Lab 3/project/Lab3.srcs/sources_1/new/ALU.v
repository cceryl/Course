`timescale 1ns / 1ps

module ALU (
    input [31:0] input1,
    input [31:0] input2,
    input [3:0] ALUCtr,
    output reg zero,
    output reg [31:0] result
);

  always @(input1, input2, ALUCtr) begin
    case (ALUCtr)
      4'b0000: result = input1 & input2;  // and
      4'b0001: result = input1 | input2;  // or
      4'b0010: result = input1 + input2;  // add
      4'b0110: result = input1 - input2;  // sub
      4'b0111: result = (input1 < input2) ? 32'b1 : 32'b0;  // slt
      4'b1100: result = ~(input1 | input2);  // nor
      default: result = 32'b0;  // Default to 0
    endcase
    zero = (result == 32'b0) ? 1'b1 : 1'b0;
  end

endmodule
