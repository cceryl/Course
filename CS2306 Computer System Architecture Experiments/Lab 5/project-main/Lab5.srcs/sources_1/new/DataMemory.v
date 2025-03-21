`timescale 1ns / 1ps

module DataMemory (
    input clk,
    input [31:0] address,
    input [31:0] writeData,
    input memWrite,
    input memRead,

    output reg [31:0] readData
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
  end

  always @(posedge clk) begin
    if (memWrite) begin
      memory[address>>2] = writeData;
    end
  end

  initial begin
    $readmemh("Data.txt", memory);
  end

endmodule
