`timescale 1ns / 1ps

module Const2 #(
   parameter DATA_W = 32
) (
   //control
   input clk,
   input rst,

   input running,
   input run,

   output [DATA_W-1:0] out0,
   output [DATA_W-1:0] out1,

   input [DATA_W-1:0] constant,  // config
   input [DATA_W-1:0] constant2  // config
);

   assign out0 = constant;
   assign out1 = constant2;

endmodule
