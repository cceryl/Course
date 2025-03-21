`timescale 1ns / 1ps

module Ctr_tb ();

  reg [5:0] OpCode;
  wire regDst;
  wire aluSrc;
  wire memToReg;
  wire regWrite;
  wire memRead;
  wire memWrite;
  wire branch;
  wire [1:0] aluOp;
  wire jump;

  Ctr u0 (
      .opCode(OpCode),
      .regDst(regDst),
      .aluSrc(aluSrc),
      .memToReg(memToReg),
      .regWrite(regWrite),
      .memRead(memRead),
      .memWrite(memWrite),
      .branch(branch),
      .aluOp(aluOp),
      .jump(jump)
  );

  initial begin
    // legal inputs
    OpCode = 6'b000000;  // R-type: add, sub, and, or, slt
    #100;
    OpCode = 6'b100011;  // lw
    #100;
    OpCode = 6'b101011;  // sw
    #100;
    OpCode = 6'b000100;  // beq
    #100;
    OpCode = 6'b000010;  // jump
    #100;
    // illegal inputs
    OpCode = 6'b000001;  // illegal
    #100;
    OpCode = 6'b001110;  // illegal
    #100;
    OpCode = 6'b000011;  // illegal
    #100;
  end

endmodule
