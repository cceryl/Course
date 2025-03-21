`timescale 1ns / 1ps

module dataMemory_tb ();

  reg Clk;
  reg [31:0] address;
  reg [31:0] writeData;
  reg memWrite;
  reg memRead;
  wire [31:0] readData;

  dataMemory dataMemory (
      .Clk(Clk),
      .address(address),
      .writeData(writeData),
      .memWrite(memWrite),
      .memRead(memRead),
      .readData(readData)
  );

  parameter PERIOD = 200;
  always #(PERIOD / 2) Clk = !Clk;

  initial begin
    Clk = 1'b0;
    address = 32'h0;
    writeData = 32'h0;
    memWrite = 1'b0;
    memRead = 1'b0;
    #185;  // current time: 185
    address   = 32'h7;
    writeData = 32'he0000000;
    memWrite  = 1'b1;
    #100;  // current time: 285
    address   = 32'h6;
    writeData = 32'hffffffff;
    #190;  // current time: 475
    address   = 32'h7;
    memWrite  = 1'b0;
    memRead   = 1'b1;
    #80;  // current time: 555
    address = 32'h8;
    writeData = 32'haaaaaaaa;
    memWrite = 1'b1;
    #80;  // current time: 635
    address = 32'h6;
    memWrite = 1'b0;
  end

endmodule
