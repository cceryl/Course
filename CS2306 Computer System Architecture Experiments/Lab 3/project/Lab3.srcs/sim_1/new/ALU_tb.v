`timescale 1ns / 1ps

module ALU_tb ();

  reg [31:0] input1;
  reg [31:0] input2;
  reg [3:0] ALUCtr;
  wire zero;
  wire [31:0] result;

  ALU u0 (
      .input1(input1),
      .input2(input2),
      .ALUCtr(ALUCtr),
      .zero  (zero),
      .result(result)
  );

  initial begin
    input1 = 32'd0;
    input2 = 32'd0;
    ALUCtr = 4'b0000;
    #100;
    input1 = 32'd15;
    input2 = 32'd10;
    ALUCtr = 4'b0000;
    #100;
    ALUCtr = 4'b0001;
    #100;
    ALUCtr = 4'b0010;
    #100;
    ALUCtr = 4'b0110;
    #100;
    input1 = 32'd10;
    input2 = 32'd15;
    ALUCtr = 4'b0110;
    #100;
    ALUCtr = 4'b0111;
    #100;
    input1 = 32'd15;
    input2 = 32'd10;
    ALUCtr = 4'b0111;
    #100;
    input1 = 32'd1;
    input2 = 32'd1;
    ALUCtr = 4'b1100;
    #100;
    input1 = 32'd16;
    ALUCtr = 4'b1100;
    #100;
  end

endmodule
