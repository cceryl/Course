`timescale 1ns / 1ps

module InstructionFetch (
    input wire clk,
    input wire reset,
    input wire flush,

    input wire i_stall,

    input wire        i_jump,
    input wire [31:0] i_jumpAddr,

    input wire        i_branch,
    input wire [31:0] i_branchAddr,

    input wire        i_predictCorrect,
    input wire [31:0] i_predictPC,

    output wire [31:0] o_PC,
    output wire [31:0] o_instruction,

    output wire o_branchPredict
);

  wire [31:0] PC;
  wire [31:0] instruction;

  reg  [15:0] branchPredictionTable;

  always @(posedge clk) begin
    if (reset) begin
      branchPredictionTable = 16'b0;
    end else if (!i_predictCorrect) begin
      branchPredictionTable[i_predictPC[6:3]] <= !branchPredictionTable[i_predictPC[6:3]];
    end
  end

  ProgramCounter programCounter (
      .clk  (clk),
      .reset(reset),
      .stall(i_stall),

      .jump    (i_jump),
      .jumpAddr(i_jumpAddr),

      .branch    (i_branch),
      .branchAddr(i_branchAddr),

      .predict    (branchPredictionTable[PC[6:3]]),
      .instruction(instruction),

      .programCounter(PC)
  );

  InstructionMemory instructionMemory (
      .clk(clk),

      .address(PC),

      .instruction(instruction)
  );

  BufferIFID buffer (
      .clk  (clk),
      .reset(reset || flush),
      .stall(i_stall),

      .instruction(instruction),
      .PC         (PC),

      .branchPredict(branchPredictionTable[PC[6:3]]),

      .instructionOut(o_instruction),
      .PCOut         (o_PC),

      .branchPredictOut(o_branchPredict)
  );

endmodule
