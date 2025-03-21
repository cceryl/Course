`timescale 1ns / 1ps

module Top (
    input wire clk_p,
    input wire clk_n,
    input wire reset_n,

    input wire [3:0] a,
    input wire [3:0] b,

    output wire led_clk,
    output wire led_en,
    output wire led_do,

    output wire seg_clk,
    output wire seg_en,
    output wire seg_do
);

  wire clk;
  wire clk_o;
  wire clkDisplay;

  IBUFGDS IBUFGDS_inst (
      .O (clk_o),
      .I (clk_p),
      .IB(clk_n)
  );

  assign clk = clk_o;

  reg [2:0] clkdiv;
  always @(posedge clk) clkdiv <= clkdiv + 1;
  assign clkDisplay = clkdiv[2];

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

  wire [31:0] product;

  ImmediateExtension immediateExtension (
      .source(instruction[15:0]),
      .opCode(instruction[31:26]),
      .extension(immediate)
  );

  ProgramCounter programCounter (
      .clk(clk),
      .reset(1'b0),
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
      .reset(1'b0),
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
      .reset(1'b0),
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
      .writeDataDMA1({28'b0, a}),
      .writeDataDMA2({28'b0, b}),
      .readData(memReadData),
      .readDataDMA(product)
  );

  display display (
      .clk(clkDisplay),
      .rst(1'b0),
      .en(8'b00001111),
      .data(product),
      .dot(8'b00000000),
      .led(~16'b0),
      .led_clk(led_clk),
      .led_en(led_en),
      .led_do(led_do),
      .seg_clk(seg_clk),
      .seg_en(seg_en),
      .seg_do(seg_do)
  );

endmodule
