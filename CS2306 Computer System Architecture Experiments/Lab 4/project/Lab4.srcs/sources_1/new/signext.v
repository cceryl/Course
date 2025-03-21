`timescale 1ns / 1ps

module signext (
    input [15:0] inst,
    output reg [31:0] data
);

  always @(*) begin
    if (inst[15] == 1) begin
      data = {16'b1111111111111111, inst};
    end else begin
      data = {16'b0000000000000000, inst};
    end
  end

endmodule
