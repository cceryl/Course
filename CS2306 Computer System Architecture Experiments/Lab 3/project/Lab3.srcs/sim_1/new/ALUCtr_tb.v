`timescale 1ns / 1ps

module ALUCtr_tb ();

  reg  [1:0] ALUOp;
  reg  [5:0] funct;
  wire [3:0] ALUCtrOut;

  ALUCtr u0 (
      .ALUOp(ALUOp),
      .funct(funct),
      .ALUCtrOut(ALUCtrOut)
  );

  initial begin
    ALUOp = 2'b00;
    funct = 6'bxxxxxx;
    #100;  // add
    ALUOp = 2'bx1;
    funct = 6'bxxxxxx;
    #100;  // sub
    ALUOp = 2'b1x;
    funct = 6'bxx0000;
    #100;  // add
    ALUOp = 2'b1x;
    funct = 6'bxx0010;
    #100;  // sub
    ALUOp = 2'b1x;
    funct = 6'bxx0100;
    #100;  // and
    ALUOp = 2'b1x;
    funct = 6'bxx0101;
    #100;  // or
    ALUOp = 2'b1x;
    funct = 6'bxx1010;
    #100;  // slt
  end

endmodule
