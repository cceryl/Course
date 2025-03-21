`timescale 1ns / 1ps

module SignedExtension (
    input [15:0] source,
    output reg [31:0] extension
);

  always @(*) begin
    if (source[15] == 1) begin
      extension = {16'b1111111111111111, source};
    end else begin
      extension = {16'b0000000000000000, source};
    end
  end

endmodule
