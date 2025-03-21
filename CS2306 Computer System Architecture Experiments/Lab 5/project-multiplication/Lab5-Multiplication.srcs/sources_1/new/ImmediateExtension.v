`timescale 1ns / 1ps

module ImmediateExtension (
    input wire [15:0] source,
    input wire [ 5:0] opCode,

    output reg [31:0] extension
);

  wire [31:0] signedExt;
  wire [31:0] unsignedExt;

  SignedExtension signedExtension (
      .source(source),
      .extension(signedExt)
  );

  UnsignedExtension unsignedExtension (
      .source(source),
      .extension(unsignedExt)
  );

  always @(*) begin
    case (opCode)
      6'b001001: extension = unsignedExt;  // addiu
      6'b001100: extension = unsignedExt;  // andi
      6'b001101: extension = unsignedExt;  // ori
      6'b001110: extension = unsignedExt;  // xori
      6'b001011: extension = unsignedExt;  // sltiu
      default:   extension = signedExt;
    endcase
  end

endmodule
