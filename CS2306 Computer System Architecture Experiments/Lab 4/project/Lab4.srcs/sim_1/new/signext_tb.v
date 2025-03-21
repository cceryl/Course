`timescale 1ns / 1ps

module signext_tb ();

  reg  [15:0] inst;
  wire [31:0] data;

  signext u0 (
      .inst(inst),
      .data(data)
  );

  initial begin
    inst = 16'h0000;
    #100;
    inst = 16'h0001;
    #100;
    inst = 16'h8000;
    #100;
    inst = 16'hffff;
    #100;
    inst = 16'h7fff;
    #100;
    inst = 16'h8001;
    #100;
  end

endmodule
