`timescale 1ns / 1ps

module InstructionDecode (
    input wire clk,
    input wire reset,
    input wire flush,

    input wire i_stall,

    input wire [31:0] i_PC,
    input wire [31:0] i_instruction,

    input wire i_branchPredict,

    input wire [31:0] i_WB_PC,
    input wire [31:0] i_WB_ALUResult,
    input wire [31:0] i_WB_readMemData,
    input wire [ 4:0] i_WB_writeReg,
    input wire        i_WB_memToReg,
    input wire        i_WB_regWrite,
    input wire        i_WB_jal,

    output wire [31:0] o_PC,
    output wire [31:0] o_instruction,

    output wire o_branchPredict,

    output wire o_regDst,
    output wire o_ALUSrc,
    output wire o_memToReg,
    output wire o_regWrite,
    output wire o_memRead,
    output wire o_memWrite,

    output wire o_beq,
    output wire o_bne,

    output wire        o_jump,
    output wire        o_jal,
    output wire [31:0] o_jumpAddr,

    output wire [3:0] o_ALUCtr,

    output wire [31:0] o_readRegData1,
    output wire [31:0] o_readRegData2,

    output wire [31:0] o_immediate
);

  wire regDst;
  wire ALUSrc;
  wire memToReg;
  wire regWrite;
  wire memRead;
  wire memWrite;
  wire [3:0] ALUOp;

  wire beq;
  wire bne;
  wire jr;
  wire j;
  wire jal;

  wire [3:0] ALUCtr;
  wire [31:0] readRegData1;
  wire [31:0] readRegData2;

  wire [31:0] immediate;

  wire [4:0] WB_writeReg = i_WB_jal ? 31 : i_WB_writeReg;
  wire [31:0] WB_writeData = i_WB_jal ? i_WB_PC + 4 : (i_WB_memToReg ? i_WB_readMemData : i_WB_ALUResult);

  wire jump = jr || j || jal;
  wire [31:0] jumpAddr = jr ? readRegData1 : (i_instruction[25:0] << 2) + ((i_PC + 4) & 32'hf0000000);

  Control control (
      .opCode(i_instruction[31:26]),

      .regDst  (regDst),
      .ALUSrc  (ALUSrc),
      .memToReg(memToReg),
      .regWrite(regWrite),
      .memRead (memRead),
      .memWrite(memWrite),
      .ALUOp   (ALUOp)
  );

  BranchControl branchControl (
      .opCode(i_instruction[31:26]),
      .funct (i_instruction[5:0]),

      .beq(beq),
      .bne(bne),
      .jr (jr),
      .j  (j),
      .jal(jal)
  );

  ALUControl aluControl (
      .ALUOp(ALUOp),
      .funct(i_instruction[5:0]),

      .ALUCtr(ALUCtr)
  );

  Registers registers (
      .clk(clk),

      .readReg1(i_instruction[25:21]),
      .readReg2(i_instruction[20:16]),

      .writeReg (WB_writeReg),
      .writeData(WB_writeData),
      .regWrite (i_WB_regWrite),

      .readData1(readRegData1),
      .readData2(readRegData2)
  );

  ImmediateExtension immediateExtension (
      .source(i_instruction[15:0]),
      .opCode(i_instruction[31:26]),

      .extension(immediate)
  );

  BufferIDEX buffer (
      .clk  (clk),
      .reset(reset || flush),
      .stall(i_stall),

      .PC         (i_PC),
      .instruction(i_instruction),

      .readReg1(readRegData1),
      .readReg2(readRegData2),

      .regDst  (regDst),
      .ALUSrc  (ALUSrc),
      .memToReg(memToReg),
      .regWrite(regWrite),
      .memRead (memRead),
      .memWrite(memWrite),
      .ALUCtr  (ALUCtr),

      .beq(beq),
      .bne(bne),

      .jump    (jump),
      .jal     (jal),
      .jumpAddr(jumpAddr),

      .immediate    (immediate),
      .branchPredict(i_branchPredict),

      .PCOut         (o_PC),
      .instructionOut(o_instruction),

      .readReg1Out(o_readRegData1),
      .readReg2Out(o_readRegData2),

      .regDstOut  (o_regDst),
      .ALUSrcOut  (o_ALUSrc),
      .memToRegOut(o_memToReg),
      .regWriteOut(o_regWrite),
      .memReadOut (o_memRead),
      .memWriteOut(o_memWrite),
      .ALUCtrOut  (o_ALUCtr),

      .beqOut(o_beq),
      .bneOut(o_bne),

      .jumpOut    (o_jump),
      .jalOut     (o_jal),
      .jumpAddrOut(o_jumpAddr),

      .immediateOut(o_immediate),
      .branchPredictOut(o_branchPredict)
  );

endmodule
