`timescale 1ns / 1ps

module BufferIFID (
    input wire clk,
    input wire reset,
    input wire stall,

    input wire [31:0] instruction,
    input wire [31:0] PC,

    input wire branchPredict,

    output reg [31:0] instructionOut,
    output reg [31:0] PCOut,

    output reg branchPredictOut
);

  always @(negedge clk, posedge reset) begin
    if (reset) begin
      instructionOut   = {32{1'b1}};
      PCOut            = {32{1'b1}};

      branchPredictOut = 1'b0;
    end else if (!stall) begin
      instructionOut   = instruction;
      PCOut            = PC;

      branchPredictOut = branchPredict;
    end
  end

endmodule

module BufferIDEX (
    input wire clk,
    input wire reset,
    input wire stall,

    input wire [31:0] PC,
    input wire [31:0] instruction,

    input wire branchPredict,

    input wire regDst,
    input wire ALUSrc,
    input wire memToReg,
    input wire regWrite,
    input wire memRead,
    input wire memWrite,

    input wire beq,
    input wire bne,

    input wire        jump,
    input wire        jal,
    input wire [31:0] jumpAddr,

    input wire [3:0] ALUCtr,

    input wire [31:0] readReg1,
    input wire [31:0] readReg2,

    input wire [31:0] immediate,

    output reg [31:0] PCOut,
    output reg [31:0] instructionOut,

    output reg branchPredictOut,

    output reg regDstOut,
    output reg ALUSrcOut,
    output reg memToRegOut,
    output reg regWriteOut,
    output reg memReadOut,
    output reg memWriteOut,

    output reg beqOut,
    output reg bneOut,

    output reg        jumpOut,
    output reg        jalOut,
    output reg [31:0] jumpAddrOut,

    output reg [3:0] ALUCtrOut,

    output reg [31:0] readReg1Out,
    output reg [31:0] readReg2Out,

    output reg [31:0] immediateOut
);

  always @(negedge clk, posedge reset) begin
    if (reset) begin
      PCOut            = {32{1'b1}};
      instructionOut   = {32{1'b1}};

      branchPredictOut = 1'b0;

      regDstOut        = 1'b0;
      ALUSrcOut        = 1'b0;
      memToRegOut      = 1'b0;
      regWriteOut      = 1'b0;
      memReadOut       = 1'b0;
      memWriteOut      = 1'b0;

      beqOut           = 1'b0;
      bneOut           = 1'b0;

      jumpOut          = 1'b0;
      jalOut           = 1'b0;
      jumpAddrOut      = 32'b0;

      ALUCtrOut        = 4'b0;

      readReg1Out      = 32'b0;
      readReg2Out      = 32'b0;

      immediateOut     = 32'b0;
    end else if (!stall) begin
      PCOut            = PC;
      instructionOut   = instruction;

      branchPredictOut = branchPredict;

      regDstOut        = regDst;
      ALUSrcOut        = ALUSrc;
      memToRegOut      = memToReg;
      regWriteOut      = regWrite;
      memReadOut       = memRead;
      memWriteOut      = memWrite;

      beqOut           = beq;
      bneOut           = bne;

      jumpOut          = jump;
      jalOut           = jal;
      jumpAddrOut      = jumpAddr;

      ALUCtrOut        = ALUCtr;

      readReg1Out      = readReg1;
      readReg2Out      = readReg2;

      immediateOut     = immediate;
    end
  end

endmodule

module BufferEXMEM (
    input wire clk,
    input wire reset,
    input wire flush,

    input wire [31:0] PC,

    input wire [31:0] ALUResult,

    input wire [31:0] readReg2,
    input wire [ 4:0] writeReg,
    input wire        memToReg,
    input wire        regWrite,
    input wire        memRead,
    input wire        memWrite,

    input wire        branch,
    input wire [31:0] branchAddr,
    input wire        predictCorrect,

    input wire jal,

    output reg [31:0] PCOut,

    output reg [31:0] ALUResultOut,

    output reg [31:0] readReg2Out,
    output reg [ 4:0] writeRegOut,
    output reg        memToRegOut,
    output reg        regWriteOut,
    output reg        memReadOut,
    output reg        memWriteOut,

    output reg        branchOut,
    output reg [31:0] branchAddrOut,
    output reg        predictCorrectOut,

    output reg jalOut
);

  always @(negedge clk, posedge reset) begin
    if (reset || flush) begin
      PCOut             = {32{1'b1}};

      ALUResultOut      = 32'b0;

      readReg2Out       = 32'b0;
      writeRegOut       = 5'b0;
      memToRegOut       = 1'b0;
      regWriteOut       = 1'b0;
      memReadOut        = 1'b0;
      memWriteOut       = 1'b0;

      branchOut         = 1'b0;
      branchAddrOut     = 32'b0;
      predictCorrectOut = 1'b1;

      jalOut            = 1'b0;
    end else begin
      PCOut             = PC;

      ALUResultOut      = ALUResult;

      readReg2Out       = readReg2;
      writeRegOut       = writeReg;
      memToRegOut       = memToReg;
      regWriteOut       = regWrite;
      memReadOut        = memRead;
      memWriteOut       = memWrite;

      branchOut         = branch;
      branchAddrOut     = branchAddr;
      predictCorrectOut = predictCorrect;

      jalOut            = jal;
    end
  end

endmodule

module BufferMEMWB (
    input wire clk,
    input wire reset,

    input wire [31:0] PC,

    input wire [31:0] ALUResult,
    input wire [31:0] readMemData,
    input wire [ 4:0] writeReg,
    input wire        memToReg,
    input wire        regWrite,
    input wire        jal,

    output reg [31:0] PCOut,

    output reg [31:0] ALUResultOut,
    output reg [31:0] readMemDataOut,
    output reg [ 4:0] writeRegOut,
    output reg        memToRegOut,
    output reg        regWriteOut,
    output reg        jalOut
);

  always @(negedge clk, posedge reset) begin
    if (reset) begin
      PCOut          = {32{1'b1}};

      ALUResultOut   = 32'b0;
      readMemDataOut = 32'b0;
      writeRegOut    = 5'b0;
      memToRegOut    = 1'b0;
      regWriteOut    = 1'b0;
      jalOut         = 1'b0;
    end else begin
      PCOut          = PC;

      ALUResultOut   = ALUResult;
      readMemDataOut = readMemData;
      writeRegOut    = writeReg;
      memToRegOut    = memToReg;
      regWriteOut    = regWrite;
      jalOut         = jal;
    end
  end

endmodule
