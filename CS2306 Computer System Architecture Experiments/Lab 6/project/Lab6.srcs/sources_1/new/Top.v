`timescale 1ns / 1ps

module Top (
    input wire clk,
    input wire reset
);

  // PC & Instruction
  wire [31:0] IF_PC, ID_PC, EX_PC, MEM_PC;
  wire [31:0] IF_instruction, ID_instruction;

  // Register Read
  wire [31:0] ID_regReadData1;
  wire [31:0] ID_regReadData2, EX_regReadData2;

  // Register Write Control
  wire [4:0] EX_writeReg, MEM_writeReg;
  wire ID_memToReg, EX_memToReg, MEM_memToReg;
  wire ID_regWrite, EX_regWrite, MEM_regWrite;

  // ALU Control
  wire ID_regDst;
  wire ID_ALUSrc;
  wire [3:0] ID_ALUCtr;

  // ALU Result
  wire [31:0] EX_ALUResult, MEM_ALUResult;

  // Memory I/O
  wire ID_memRead, EX_memRead;
  wire ID_memWrite, EX_memWrite;
  wire [31:0] MEM_memReadData;

  // Branch Control
  wire ID_beq;
  wire ID_bne;
  wire EX_branch;
  wire [31:0] EX_branchAddr;

  // Jump Controls
  wire ID_jump;
  wire ID_jal, EX_jal, MEM_jal;
  wire [31:0] ID_jumpAddr;

  // Immediate
  wire [31:0] ID_immediate;

  // Forwarding
  wire [31:0] MEM_forwardData;
  wire [4:0] MEM_forwardWriteReg;

  // Load-Use Hazard
  wire MEM_stall = EX_memRead && (EX_writeReg == ID_instruction[20:16] || EX_writeReg == ID_instruction[15:11] && ID_ALUSrc == 0)
                   && (ID_instruction[31:26] != `OP_LW) && !ID_jump;

  // Branch Prediction
  wire IF_branchPredict, ID_branchPredict;
  wire EX_predictCorrect;

  InstructionFetch instructionFetch (
      .clk  (clk),
      .reset(reset),
      .flush(ID_jump || !EX_predictCorrect),

      .i_stall(MEM_stall),

      .i_jump    (ID_jump),
      .i_jumpAddr(ID_jumpAddr),

      .i_branch    (EX_branch),
      .i_branchAddr(EX_branchAddr),

      .i_predictCorrect(EX_predictCorrect),
      .i_predictPC     (EX_PC),

      .o_PC         (IF_PC),
      .o_instruction(IF_instruction),

      .o_branchPredict(IF_branchPredict)
  );

  InstructionDecode instructionDecode (
      .clk  (clk),
      .reset(reset),
      .flush(!EX_predictCorrect),

      .i_stall(MEM_stall),

      .i_PC         (IF_PC),
      .i_instruction(IF_instruction),

      .i_branchPredict(IF_branchPredict),

      .i_WB_PC         (MEM_PC),
      .i_WB_ALUResult  (MEM_ALUResult),
      .i_WB_readMemData(MEM_memReadData),
      .i_WB_writeReg   (MEM_writeReg),
      .i_WB_memToReg   (MEM_memToReg),
      .i_WB_regWrite   (MEM_regWrite),
      .i_WB_jal        (MEM_jal),

      .o_PC(ID_PC),
      .o_instruction(ID_instruction),

      .o_branchPredict(ID_branchPredict),

      .o_regDst  (ID_regDst),
      .o_ALUSrc  (ID_ALUSrc),
      .o_memToReg(ID_memToReg),
      .o_regWrite(ID_regWrite),
      .o_memRead (ID_memRead),
      .o_memWrite(ID_memWrite),

      .o_beq(ID_beq),
      .o_bne(ID_bne),

      .o_jump    (ID_jump),
      .o_jal     (ID_jal),
      .o_jumpAddr(ID_jumpAddr),


      .o_ALUCtr(ID_ALUCtr),

      .o_readRegData1(ID_regReadData1),
      .o_readRegData2(ID_regReadData2),

      .o_immediate(ID_immediate)
  );

  Execution execution (
      .clk  (clk),
      .reset(reset),

      .i_stall(MEM_stall),

      .i_PC(ID_PC),

      .i_input1   (ID_regReadData1),
      .i_input2   (ID_ALUSrc ? ID_immediate : ID_regReadData2),
      .i_inputReg1(ID_instruction[25:21]),
      .i_inputReg2(ID_ALUSrc ? 5'b0 : ID_instruction[20:16]),
      .i_shamt    (ID_instruction[10:6]),
      .i_ALUCtr   (ID_ALUCtr),

      .i_readReg2Data(ID_regReadData2),
      .i_readReg2(ID_instruction[20:16]),
      .i_writeReg(ID_regDst ? ID_instruction[15:11] : ID_instruction[20:16]),
      .i_memToReg(ID_memToReg),
      .i_regWrite(ID_regWrite),
      .i_memRead(ID_memRead),
      .i_memWrite(ID_memWrite),

      .i_beq      (ID_beq),
      .i_bne      (ID_bne),
      .i_immediate(ID_immediate),

      .i_jal(ID_jal),

      .i_forwardData    (MEM_forwardData),
      .i_forwardWriteReg(MEM_forwardWriteReg),

      .i_branchPredict(ID_branchPredict),

      .o_PC(EX_PC),

      .o_result(EX_ALUResult),

      .o_readReg2(EX_regReadData2),
      .o_writeReg(EX_writeReg),
      .o_memToReg(EX_memToReg),
      .o_regWrite(EX_regWrite),
      .o_memRead (EX_memRead),
      .o_memWrite(EX_memWrite),

      .o_branch        (EX_branch),
      .o_branchAddr    (EX_branchAddr),
      .o_predictCorrect(EX_predictCorrect),

      .o_jal(EX_jal)
  );

  Memory memory (
      .clk  (clk),
      .reset(reset),

      .i_PC(EX_PC),

      .i_address  (EX_ALUResult),
      .i_writeData(EX_regReadData2),
      .i_memWrite (EX_memWrite),
      .i_memRead  (EX_memRead),

      .i_writeReg(EX_writeReg),
      .i_memToReg(EX_memToReg),
      .i_regWrite(EX_regWrite),
      .i_jal     (EX_jal),

      .o_PC(MEM_PC),

      .o_ALUResult  (MEM_ALUResult),
      .o_readMemData(MEM_memReadData),
      .o_writeReg   (MEM_writeReg),
      .o_memToReg   (MEM_memToReg),
      .o_regWrite   (MEM_regWrite),
      .o_jal        (MEM_jal),

      .o_forwardData    (MEM_forwardData),
      .o_forwardWriteReg(MEM_forwardWriteReg)
  );

endmodule
