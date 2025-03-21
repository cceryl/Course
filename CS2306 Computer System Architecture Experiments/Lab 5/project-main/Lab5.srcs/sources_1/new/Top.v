`timescale 1ns / 1ps

module Top (
    input wire clk,
    input wire reset
);

  wire [31:0] PC;

  wire [31:0] instruction;

  wire [31:0] regReadData1;
  wire [31:0] regReadData2;

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

  wire [3:0] ALUControlCode;
  wire ALUZeroFlag;
  wire [31:0] ALUResult;

  wire [31:0] memReadData;

  wire [31:0] immediate;

  ImmediateExtension immediateExtension (
      .source(instruction[15:0]),
      .opCode(instruction[31:26]),
      .extension(immediate)
  );

  ProgramCounter programCounter (
      .clk(clk),
      .reset(reset),
      .jumpImm(instruction[25:0]),
      .branchImm(immediate),
      .ra(regReadData1),
      .beq(beq),
      .bne(bne),
      .jr(jr),
      .j(j),
      .jal(jal),
      .ALUZero(ALUZeroFlag),
      .programCounter(PC)
  );

  InstructionMemory instructionMemory (
      .clk(clk),
      .reset(reset),
      .address(PC),
      .readData(instruction)
  );

  Control control (
      .opCode(instruction[31:26]),
      .regDst(regDst),
      .ALUSrc(ALUSrc),
      .memToReg(memToReg),
      .regWrite(regWrite),
      .memRead(memRead),
      .memWrite(memWrite),
      .ALUOp(ALUOp)
  );

  BranchControl branchControl (
      .opCode(instruction[31:26]),
      .funct(instruction[5:0]),
      .beq(beq),
      .bne(bne),
      .jr(jr),
      .j(j),
      .jal(jal)
  );

  Registers registers (
      .clk(clk),
      .readReg1(instruction[25:21]),
      .readReg2(instruction[20:16]),
      .writeReg(jal ? 5'd31 : (regDst ? instruction[15:11] : instruction[20:16])),
      .writeData(jal ? PC + 4 : (memToReg ? memReadData : ALUResult)),
      .regWrite(regWrite),
      .readData1(regReadData1),
      .readData2(regReadData2)
  );

  ALU alu (
      .input1(regReadData1),
      .input2(ALUSrc ? immediate : regReadData2),
      .shamt (instruction[10:6]),
      .ALUCtr(ALUControlCode),
      .zero  (ALUZeroFlag),
      .result(ALUResult)
  );

  ALUControl aluControl (
      .ALUOp(ALUOp),
      .funct(instruction[5:0]),
      .out  (ALUControlCode)
  );

  DataMemory dataMemory (
      .clk(clk),
      .address(ALUResult),
      .writeData(regReadData2),
      .memRead(memRead),
      .memWrite(memWrite),
      .readData(memReadData)
  );

endmodule
