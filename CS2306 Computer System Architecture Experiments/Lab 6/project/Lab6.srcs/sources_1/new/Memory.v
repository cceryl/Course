`timescale 1ns / 1ps

module Memory (
    input wire clk,
    input wire reset,

    input wire [31:0] i_PC,

    input wire [31:0] i_address,
    input wire [31:0] i_writeData,
    input wire        i_memWrite,
    input wire        i_memRead,

    input wire [4:0] i_writeReg,
    input wire       i_memToReg,
    input wire       i_regWrite,
    input wire       i_jal,

    output wire [31:0] o_PC,

    output wire [31:0] o_ALUResult,
    output wire [31:0] o_readMemData,
    output wire [ 4:0] o_writeReg,
    output wire        o_memToReg,
    output wire        o_regWrite,
    output wire        o_jal,

    output wire [31:0] o_forwardData,
    output wire [ 4:0] o_forwardWriteReg
);

  wire [31:0] readData;

  assign o_forwardData     = o_memToReg ? o_readMemData : o_ALUResult;
  assign o_forwardWriteReg = o_regWrite ? o_writeReg : 0;

  DataMemory dataMemory (
      .clk(clk),

      .address  (i_address),
      .writeData(i_writeData),
      .memRead  (i_memRead),
      .memWrite (i_memWrite),

      .readData(readData)
  );

  BufferMEMWB buffer (
      .clk  (clk),
      .reset(reset),

      .PC(i_PC),

      .ALUResult  (i_address),
      .readMemData(readData),
      .writeReg   (i_writeReg),
      .memToReg   (i_memToReg),
      .regWrite   (i_regWrite),
      .jal        (i_jal),

      .PCOut(o_PC),

      .ALUResultOut  (o_ALUResult),
      .readMemDataOut(o_readMemData),
      .writeRegOut   (o_writeReg),
      .memToRegOut   (o_memToReg),
      .regWriteOut   (o_regWrite),
      .jalOut        (o_jal)
  );

endmodule
