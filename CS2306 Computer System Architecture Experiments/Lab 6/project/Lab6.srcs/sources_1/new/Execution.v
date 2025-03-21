`timescale 1ns / 1ps

module Execution (
    input wire clk,
    input wire reset,

    input wire i_stall,

    input wire [31:0] i_PC,

    input wire [31:0] i_input1,
    input wire [31:0] i_input2,
    input wire [ 4:0] i_inputReg1,
    input wire [ 4:0] i_inputReg2,
    input wire [ 4:0] i_shamt,
    input wire [ 3:0] i_ALUCtr,

    input wire [31:0] i_readReg2Data,
    input wire [ 4:0] i_readReg2,
    input wire [ 4:0] i_writeReg,
    input wire        i_memToReg,
    input wire        i_regWrite,
    input wire        i_memRead,
    input wire        i_memWrite,

    input wire        i_beq,
    input wire        i_bne,
    input wire [31:0] i_immediate,

    input wire i_jal,

    input wire [31:0] i_forwardData,
    input wire [ 4:0] i_forwardWriteReg,

    input wire i_branchPredict,

    output wire [31:0] o_PC,

    output wire [31:0] o_result,

    output wire [31:0] o_readReg2,
    output wire [ 4:0] o_writeReg,
    output wire        o_memToReg,
    output wire        o_regWrite,
    output wire        o_memRead,
    output wire        o_memWrite,

    output wire        o_branch,
    output wire [31:0] o_branchAddr,
    output wire        o_predictCorrect,

    output wire o_jal
);

  wire zero;
  wire [31:0] result;

  wire innerForward1 = i_inputReg1 == o_writeReg && o_writeReg != 0;
  wire innerForward2 = i_inputReg2 == o_writeReg && o_writeReg != 0;
  wire innerForward3 = i_readReg2 == o_writeReg && o_writeReg != 0;

  wire memForward1 = i_inputReg1 == i_forwardWriteReg && i_forwardWriteReg != 0;
  wire memForward2 = i_inputReg2 == i_forwardWriteReg && i_forwardWriteReg != 0;
  wire memForward3 = i_readReg2 == i_forwardWriteReg && i_forwardWriteReg != 0;

  wire isBranch = i_beq || i_bne;
  wire branchTaken = (i_beq && zero) || (i_bne && !zero);
  wire [31:0] branchAddr = i_PC + 4 + (i_immediate << 2);
  wire predictCorrect = !isBranch || (i_branchPredict && branchTaken) || (!i_branchPredict && !branchTaken);

  ALU alu (
      .input1(innerForward1 ? o_result : (memForward1 ? i_forwardData : i_input1)),
      .input2(innerForward2 ? o_result : (memForward2 ? i_forwardData : i_input2)),
      .shamt (i_shamt),
      .ALUCtr(i_ALUCtr),

      .zero  (zero),
      .result(result)
  );

  BufferEXMEM buffer (
      .clk  (clk),
      .reset(reset),
      .flush(i_stall),

      .PC(i_PC),

      .ALUResult(result),

      .readReg2(innerForward3 ? o_result : (memForward3 ? i_forwardData : i_readReg2Data)),
      .writeReg(i_writeReg),
      .memToReg(i_memToReg),
      .regWrite(i_regWrite),
      .memRead (i_memRead),
      .memWrite(i_memWrite),

      .branch        (branchTaken),
      .branchAddr    (branchAddr),
      .predictCorrect(predictCorrect),

      .jal(i_jal),

      .PCOut(o_PC),

      .ALUResultOut(o_result),

      .readReg2Out(o_readReg2),
      .writeRegOut(o_writeReg),
      .memToRegOut(o_memToReg),
      .regWriteOut(o_regWrite),
      .memReadOut (o_memRead),
      .memWriteOut(o_memWrite),

      .branchOut        (o_branch),
      .branchAddrOut    (o_branchAddr),
      .predictCorrectOut(o_predictCorrect),

      .jalOut(o_jal)
  );

endmodule
