`timescale 1ns / 1ps

module ALUCtr (
    input [1:0] ALUOp,
    input [5:0] funct,
    output reg [3:0] ALUCtrOut
);

  always @(ALUOp, funct) begin
    casex ({
      ALUOp, funct
    })
      8'b00_xxxxxx: begin  // add
        ALUCtrOut = 4'b0010;
      end
      8'b01_xxxxxx: begin  // sub
        ALUCtrOut = 4'b0110;
      end
      8'b1x_xx0000: begin  // add
        ALUCtrOut = 4'b0010;
      end
      8'b1x_xx0010: begin  // sub
        ALUCtrOut = 4'b0110;
      end
      8'b1x_xx0100: begin  // and
        ALUCtrOut = 4'b0000;
      end
      8'b1x_xx0101: begin  // or
        ALUCtrOut = 4'b0001;
      end
      8'b1x_xx1010: begin  // slt
        ALUCtrOut = 4'b0111;
      end
      default: begin  // invalid
        ALUCtrOut = 4'bxxxx;
      end
    endcase
  end

endmodule