`timescale 1ns / 1ps
`include "ISA.v"

module ImmediateExtension (
    input wire [15:0] source,
    input wire [ 5:0] opCode,

    output reg [31:0] extension
);

  wire [31:0] signedExt;
  wire [31:0] unsignedExt;

  assign signedExt   = {{16{source[15]}}, source};
  assign unsignedExt = {16'h0000, source};

  always @(*) begin
    case (opCode)
      `OP_ADDIU: extension = unsignedExt;
      `OP_ANDI:  extension = unsignedExt;
      `OP_ORI:   extension = unsignedExt;
      `OP_XORI:  extension = unsignedExt;
      `OP_SLTIU: extension = unsignedExt;
      default:   extension = signedExt;
    endcase
  end

endmodule
