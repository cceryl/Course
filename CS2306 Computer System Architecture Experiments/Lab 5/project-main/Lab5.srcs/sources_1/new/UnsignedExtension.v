`timescale 1ns / 1ps

module UnsignedExtension (
    input [15:0] source,
    output reg [31:0] extension
);

  always @(*) begin
    extension = {16'b0000000000000000, source};
  end

endmodule
