`timescale 1ns / 1ps

module sim;
  reg  clock;
  reg  reset;
  reg  start;
  reg  check;
  wire finish;
  wire correct;

  always #5 clock = ~clock;

  FPGAShell shell (
      .clock  (clock),
      .reset  (reset),
      .start  (start),
      .check  (check),
      .finish (finish),
      .correct(correct)
  );

  initial begin
    clock = 1'b0;
    reset = 1'b0;
    start = 1'b0;
    check = 1'b0;
    #10 reset = 1'b0;
    #20 start = 1'b1;
    #30 start = 1'b0;
    #200 check = 1'b1;
    #210 check = 1'b0;
  end

endmodule
