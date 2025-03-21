`timescale 1ns / 1ps

module DataMemory (
    input wire clk,
    input wire [31:0] address,
    input wire [31:0] writeData,
    input wire memWrite,
    input wire memRead,
    input wire [31:0] writeDataDMA1,
    input wire [31:0] writeDataDMA2,

    output reg [31:0] readData,
    output reg [31:0] readDataDMA
);

  reg [31:0] memory[0:63];

  always @(*) begin
    if (memRead) begin
      if (memWrite) begin
        readData = writeData;
      end else begin
        readData = memory[address>>2];
      end
    end else begin
      readData = 0;
    end
    readDataDMA = memory[2];
  end

  always @(posedge clk) begin
    memory[0] = writeDataDMA1;
    memory[1] = writeDataDMA2;
    if (memWrite) memory[2] = writeData;
  end

  generate
    for (genvar i = 0; i < 64; i = i + 1) begin
      initial begin
        memory[i] = 32'h0;
      end
    end
  endgenerate

endmodule
