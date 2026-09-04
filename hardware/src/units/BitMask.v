`timescale 1ns / 1ps

module BitMask(
   input         clk,
   input         rst,

   input         running,
   input         run,

   input [31:0]  in0,      
   output [31:0] out0,

   input [31:0]  mask
);

assign out0 = (in0 & mask);

endmodule